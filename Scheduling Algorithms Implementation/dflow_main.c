/* Main program for testing invocations to dflow_calc */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dflow_calc.h"


/// Quota of program buffer size increase
#define PROG_SIZE_QUOTA 1000

/// Resize program buffer
/// \param[in] maxSize New program buffer size limit
/// \param[in] oldBuf The old buffer used for the program. May be NULL for new allocation.
/// \returns The allocated program buffer of given maxSize entries
InstInfo *allocProgBuffer(size_t maxSize, InstInfo *oldBuf)
{
    InstInfo *newBuf = realloc(oldBuf, maxSize * sizeof(InstInfo));
    if (newBuf == NULL) {
        printf("ERROR: Failed allocating program buffer for %zu instructions!\n", maxSize);
        exit(1);
    }
    return newBuf;
}

char *remove_white_spaces(char *str)
{
    int i = 0, j = 0;
    while (str[i])
    {
        if (str[i] != ' ')
        str[j++] = str[i];
        i++;
    }
    str[j] = '\0';
    return str;
}

/// readTrace: Read a program file formatted with {dst src1 src2} triplets
/// \param[in] filename The trace file name
/// \param[out] prog The program trace (array) that was read from the given file. Allocated in this function. Should be freed by the caller.
/// \returns >0 The number of elements (instructions) in the returned prog[] , <0 error reading the trace file or creating the array
int readProgram(const char *filename, InstInfo **prog) {
    int i;
    int lineNum = 0;
    char curLine[81];
    char *curField, *endOfVal;
    long int fieldVal[7];
    char *tokenizerEntry; // Entry "tag" for strtok()
    InstInfo *progBuf = NULL; // Current program buffer (may be resized as needed)
    size_t maxInsts = 0; // Current limit on the program buffer

    *prog = NULL; // Initialize in case of exit with error

    FILE *progFile = fopen(filename, "r");
    if (progFile == NULL) {
        printf("ERROR: Failed openning the program file: %s\n", filename);
        return -1;
    }

    // Read program file lines
    while (fgets(curLine, sizeof(curLine), progFile) != NULL) {
        //printf("*** curLine=%s\n", curLine);
        if (lineNum == maxInsts) { // Need to resize buffer
            maxInsts += PROG_SIZE_QUOTA;
            progBuf = allocProgBuffer(maxInsts, progBuf);
        }
        tokenizerEntry = curLine;
        while (isspace(*tokenizerEntry)) ++tokenizerEntry; // Strip leading whitespace
        if ((tokenizerEntry[0] == 0) || (tokenizerEntry[0] == '#'))
            continue; // Ignore empty lines and comments (lines that start with '#')
        // Parse line of 6-7 decimal numbers (opcode + register indices: op dst src1 src2 src3 id) or (block opcode + register indices: op dst src1 src2 src3 id)
        for (i = 0; i < 7; ++i) {
            curField = strtok(tokenizerEntry, " \t\n\r");
            if (curField == NULL && i<6) {
                printf("ERROR: Error parsing instruction #%u of %s\n", lineNum, filename);
                return -2;
            }
            if(fieldVal[0] != -2 && i == 6) fieldVal[6] = 0;
            else fieldVal[i] = strtol(curField, &endOfVal, 10);
            if (endOfVal[0] != 0) {
                printf("ERROR: Failed parsing field %d of line #%u of %s: %s\n", i, lineNum, filename, curLine);
                return -2;
            }
            tokenizerEntry = NULL; // for next tokens should provide strtok NULL
        }
        progBuf[lineNum].isBlock = (fieldVal[0] == -2);
        if(progBuf[lineNum].isBlock) {
            progBuf[lineNum].opcode = fieldVal[1];
            progBuf[lineNum].dstIdx = fieldVal[2];
            progBuf[lineNum].src1Idx = fieldVal[3];
            progBuf[lineNum].src2Idx = fieldVal[4];
            progBuf[lineNum].src3Idx = fieldVal[5];
            progBuf[lineNum].InstID = fieldVal[6];
        } else {
            progBuf[lineNum].opcode = fieldVal[0];
            progBuf[lineNum].dstIdx = fieldVal[1];
            progBuf[lineNum].src1Idx = fieldVal[2];
            progBuf[lineNum].src2Idx = fieldVal[3];
            progBuf[lineNum].src3Idx = fieldVal[4];
            progBuf[lineNum].InstID = fieldVal[5];
        }
        ++lineNum;
    }

    fclose(progFile);
    *prog = progBuf; // To return to caller
    return lineNum;
}

/// readOpsLatency: Read data file for opcodes execution latency
/// \param[in] opFname The filename of the file with the respective data (one decimal number per line)
/// \param[out] opsLatency Pointer to an array of MAX_OPS entries that would be filled with the respective opcode latency
/// \returns int The number of opcodes info filled in opsLatency[]. The other entries are set to 0.
int readOpsLatency(const char* opFname, unsigned int opsLatency[]) {
    int numOps = 0;
    char curLine[81];
    char *endOfVal;
    FILE *opcodeFile;

    for (int i = 0; i < MAX_OPS; ++i)
        opsLatency[i] = 0; // Initialize for opcodes that would not be included in the info file
    opcodeFile = fopen(opFname, "r");
    if (opcodeFile == NULL) {
        printf("ERROR: Failed openning %s\n", opFname);
        return -1;
    }
    while (fgets(curLine, sizeof(curLine), opcodeFile) != NULL) { // Read available opcode latency data
        if (numOps >= MAX_OPS) {
            printf("ERROR: Opcodes latency file has more opcodes than maximum supported\n");
            return -3;
        }
        opsLatency[numOps++] = strtol(curLine, &endOfVal, 10);
        while (isspace(*endOfVal)) ++endOfVal; // Strip trailing spaces
        if (endOfVal[0] != 0) { // Verify that parsing ended at end of the line
            printf("ERROR: Failed parsing opcode latency at line %d of %s\n", numOps, opFname);
            return -2;
        }
    }
    fclose(opcodeFile);
    return numOps;
}

void usage(void) {
    printf("Usage: dflow_calc <Config File Path>\n");
    exit(1);
}


int main(int argc, const char *argv[]) {
    const char *configFilePath = argv[1];
    const char *opFname;
    FILE *configFile = fopen(argv[1], "r");
    char curLine[81];
    char *tokenizerEntry;
    int lineNum = 0;
    int config[10];
    char *endOfVal;
    for (int i = 0; i < 10; ++i) {
        config[i] = -1;
    }
    int param = 0;
    bool end = false;
    while (fgets(curLine, sizeof(curLine), configFile) != NULL && param < 11) {
        tokenizerEntry = curLine;
        while (isspace(*tokenizerEntry)) ++tokenizerEntry; // Strip leading whitespace
        if ((tokenizerEntry[0] == 0) || (tokenizerEntry[0] == '#'))
            continue; // Ignore empty lines and comments (lines that start with '#')
        char *curField;
        for (int i = 0; i < 3; ++i) {
            curField = strtok(tokenizerEntry, " \t\n\r");
            if (curField == NULL) {
                printf("ERROR: Error parsing line #%u of %s\n", lineNum, configFilePath);
                exit(1);
            }
            if( i == 2 && param == 10) {
                opFname = curField;
                end = true;
                break;
            }
            if(i == 2 && param < 10) {
                config[param] = strtol(curField, &endOfVal, 10);

                if (endOfVal[0] != 0) {
                    printf("ERROR: Failed parsing field %d of line #%u of %s: %s\n", i, lineNum, configFilePath, curLine);
                    exit(1);
                }
            }
            tokenizerEntry = NULL; // for next tokens should provide strtok NULL
        }
        if(end) break;
        param++;
        ++lineNum;
    }
    unsigned int opsLatency[MAX_OPS];
    InstInfo *theProg;
    int progLen, numOps;
    ProgCtx ctx;


    Execution_units.IN_ORDER = config[0];
    Execution_units.ALU_UNITS = config[1];
    Execution_units.FP_UNITS = config[2];
    Execution_units.LOAD_UNITS = config[3];
    Execution_units.STORE_UNITS = config[4];
    Execution_units.BRANCH_UNITS = config[5];
    Execution_units.WINDOW_SIZE = config[6];
    Execution_units.THREAD_NUM = config[7];
    Execution_units.scheduler = config[8];
    Execution_units.slot = config[9];

    //remove_white_spaces(opFname);
    numOps = readOpsLatency(opFname, opsLatency);
    if (numOps < 0)
        exit(1);

    ProgCtx *threads = malloc(sizeof(ProgCtx)* Execution_units.THREAD_NUM);
    if(!threads) exit(1);
    const char** paths[Execution_units.THREAD_NUM];
    int threadsNum = 0;
    while (fgets(curLine, sizeof(curLine), configFile) != NULL && threadsNum < Execution_units.THREAD_NUM) {
        tokenizerEntry = curLine;
        while (isspace(*tokenizerEntry)) ++tokenizerEntry; // Strip leading whitespace
        if ((tokenizerEntry[0] == 0) || (tokenizerEntry[0] == '#'))
            continue; // Ignore empty lines and comments (lines that start with '#')
        char *curField;
        for (int i = 0; i < 3; ++i) {
            curField = strtok(tokenizerEntry, " \t\n\r");
            if (curField == NULL) {
                printf("ERROR: Error parsing line #%u of %s\n", lineNum, configFilePath);
                exit(1);
            }
            if(i == 2) {
                const char *progName = curField;
                printf("Reading the program file %s ... ", progName);
                progLen = readProgram(progName, &theProg);
                if (progLen <= 0) {
                    printf("Error reading program file %s!\n", progName);
                    exit(1);
                }
                printf("Found %d instructions\n", progLen);
                if (progLen > 500000) {
                    progLen = 500000;
                }
                // Analyze the program
                ctx = analyzeProg(opsLatency, theProg, progLen);
                if (ctx == PROG_CTX_NULL) {
                    printf("Error on invocation to analyzeCtx()\n");
                    exit(2);
                }
                threads[threadsNum] = ctx;
            }
            tokenizerEntry = NULL; // for next tokens should provide strtok NULL
        }
        threadsNum++;
        ++lineNum;
    }
    if(threadsNum <  Execution_units.THREAD_NUM){
        printf("Error in threads number\n");
        exit(1);
    }


    double alu_utilization=0.0, fp_utilization=0.0, load_utilization = 0.0, store_utilization = 0.0, branch_utilization = 0.0;
    double ipc_rr = 0;
    if(Execution_units.scheduler == ROUND_ROBIN) {
        if (Execution_units.IN_ORDER) {
            ipc_rr = get_MT_IPC_RR_IN_ORDER(threads, &alu_utilization, &fp_utilization, &load_utilization,
                                            &store_utilization, &branch_utilization);
            printf("get_MT_IPC_RR()==%f  alu_utilization==%f fp_utilization==%f load_utilization==%f store_utilization==%f branch_utilization==%f\n",
                   ipc_rr, alu_utilization, fp_utilization, load_utilization, store_utilization, branch_utilization);
        } else {
            ipc_rr = get_MT_IPC_RR_OOO(threads, &alu_utilization, &fp_utilization, &load_utilization,
                                       &store_utilization, &branch_utilization);
            printf("get_MT_IPC_RR_OOO()==%f  alu_utilization==%f fp_utilization==%f load_utilization==%f store_utilization==%f branch_utilization==%f\n",
                   ipc_rr, alu_utilization, fp_utilization, load_utilization, store_utilization, branch_utilization);
        }
    }
    if(Execution_units.scheduler == SJF) {
        double alu_utilization_sjf = 0.0, fp_utilization_sjf = 0.0, load_utilization_sjf = 0.0, store_utilization_sjf = 0.0, branch_utilization_sjf = 0.0;
        double ipc_sjf = get_MT_IPC_SJF(threads, &alu_utilization_sjf, &fp_utilization_sjf, &load_utilization_sjf,
                                        &store_utilization_sjf, &branch_utilization_sjf);
        printf("get_MT_IPC_SJF1()==%f   alu_utilization==%f fp_utilization==%f load_utilization==%f store_utilization==%f branch_utilization==%f\n",
               ipc_sjf, alu_utilization_sjf, fp_utilization_sjf, load_utilization_sjf, store_utilization_sjf,
               branch_utilization_sjf);
    }
    if(Execution_units.scheduler == FIFO) {
        printf("get_MT_IPC_FF()==%f\n", FIFOMT(threads));
    }
    // Read instruction specific queries (if any)
    freeProgCtx(ctx);
    free(theProg); // We keep theProg up to here to allow your analyzer to use it - if it wants
    return 0;
}
