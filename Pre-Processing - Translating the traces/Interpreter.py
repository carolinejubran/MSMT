###################

LOAD = "0"
STORE = "1"
ALU = "2"
FP = "3"
BRANCH = "4"
NOP = "-1"
BLOCK = "-2"
ALU_WITH_NO_REGISTERS = "5"
CONSTANT = "0x0"

###################
Instructions = {
    "mov": {LOAD},
    "call": {BRANCH},
    "push": {STORE},
    "add": {ALU},
    "sub": {ALU},
    "lea": {ALU},
    "j": {BRANCH},
    "rdtsc": {LOAD},  # file2.write("0 0 -1 -1\n")  # eax  file2.write("0 2 -1 -1\n")  # edx
    "shl": {ALU},
    "or": {ALU},
    "xor": {ALU},
    "test": {ALU},
    "cmp": {ALU},
    "sar": {ALU},
    "shr": {ALU},
    "neg": {ALU},
    "jz": {BRANCH},
    "ret": {BRANCH},  # pop +jmp
    "pop": {LOAD},
    "imul": {ALU},
    "movhpd": {FP},  # MOVHPD xmm, m64 , MOVHPD m64, xmm
    "rol": {ALU},
    "movd": {LOAD},  #### can be store also
    "vpcmpeqb": {ALU},  ## only with registers
    'movhps': {FP},  ##It cannot be used for register to register or memory to memory moves. store/load
    "ror": {ALU},
    'movq': {LOAD},  #### can be store also
    'movdqu': {LOAD},  #### can be store also
    'movdqa': {LOAD},  #### can be store also
    'vcvtsi2sd': {FP},  # reg,memory/reg
    'vpxor': {ALU},
    'vpor': {ALU},
    'bnd': {BRANCH},
    'movlpd': {FP},  ##It cannot be used for register to register or memory to memory moves. store/load
    'setbe': {ALU},  # have only one destination  no sources
    'vpandn': {ALU},
    'punpckhdq': {ALU},
    'movzx': {LOAD},  ####cant be store .!
    'vdivsd': {FP},  ### +load
    'vpinsrq': {ALU},
    'not': {ALU},  # dst = src1
    'nop': {NOP},
    'punpckldq': {ALU},
    'vxorpd': {FP},
    'lfence\n': {BLOCK},
    'div': {ALU},
    'psubb': {ALU},
    'psrldq': {ALU},
    'setnle': {ALU},  # dst //load
    'vpmovmskb': {LOAD},
    'pminub': {ALU},
    'vmovq': {LOAD},  # can b e also store
    'rep': {ALU},
    # repeats a string instruction number of times. The REP is a prefix for the instruction -- check which command
    'vmovd': {LOAD},  # mov doubleword with MMX and XMM technologies registers (the same as movd)
    'vpslldq': {ALU},  # shift src1(register/mem) by src2(immediate) and store in dest(register) -- can be load
    'xchg': {ALU},  # swap between src(register/mem) and dest(register/mem). store/load
    'mul': {ALU},  # dest = dest(register) * src(register/mem) -- load
    'pcmpistri': {ALU},
    # compares between dst(register) and src1(register/mem) and stores the comparison in register ECX -- load
    'vpsubb': {ALU},  # dst(register) = src1(register) - src2(register/mem)-- load
    'vmovsd': {LOAD},  # moves data from src(register/mem) to dst(register/mem) -- load/store
    'bt': {STORE},
    # test bit and store result in CF flag - no dst(it is CF flag) src1(register/mem) src2(register/immediate) -- load -- no 2 registers nor 2 mem
    'movbe': {LOAD},  # swap between src1(register/mem) and dst(mem/register) -- load/store
    'cmovnbe': {LOAD},  # conditional move if not below or equal (EFLAGS) then a mov is done - load/store
    'cmovbe': {LOAD},  # Move if below or equal  (EFLAGS) then a mov is done - load/store
    'sbb': {ALU},  # dst(register/mem) = src1(register/mem/imm) + carry on (CF flag) - dst -- load/store
    'cmovz': {LOAD},  # conditional move (use EFLAGS reg) -- load/store
    'cmovs': {LOAD},  # conditional move (use EFLAGS reg) -- load/store
    'blsr': {ALU},  # dst(register) = src(register/mem) --> it also changes some of the EFLAGS register -- load
    'movups': {LOAD},  # moves src to dst (can be registers or memories) -- load/store
    'nop\n': {NOP},
    'cmovl': {LOAD},  # conditional move (use EFLAGS reg) -- load/store
    'movsx': {LOAD},
    # Copies the contents of the source operand (register or memory location) to the destination operand (register) and sign extends the value
    'punpcklwd': {ALU},
    # Unpacks and interleaves the low-order data elements of the destination operand (first operand) and source operand (second operand) into the destination operand - src(mem/register) dst(register) -- load
    'cmovb': {LOAD},  # conditional move (use EFLAGS reg)
    'vzeroupper\n': {ALU},
    # The instruction zeros the bits in position 128 and higher of all YMM registers. The lower 128-bits of the registers (the corresponding XMM registers) are unmodified
    'xgetbv': {LOAD},  # Restore state components specified by EDX:EAX from mem.
    'palignr': {ALU},
    # concatenates the dest(reg) and the source(src1- reg/mem) into an intermediate composite, shifts the composite at byte granularity to the right by a constant immediate(src2), and extracts the right-aligned result into the dest -- load
    'por': {ALU},
    # Performs a bitwise logical OR operation on the source operand (second operand- reg/mem) and the destination operand (first operand- reg) and stores the result in the destination operand -- load
    'pcmpeqb': {ALU},
    # Performs a SIMD compare in the dest(reg) and the source(reg/mem). If equal, the corresponding data element in the destination operand is set to all 1s; otherwise, it is set to all 0s. -- load/store
    'pcmpeqd': {ALU},  # the same as before for doubleword instead of byte -- load/store
    'setnbe': {ALU},
    # sets the destination(reg/mem) to 0 or 1 depending on the settings of the status flags (CF, SF, OF, ZF, and PF) in the EFLAGS register -- store
    'shrx': {ALU},
    # Shifts the bits of src1(reg/mem) to the right by a COUNT value specified in src2(reg). The result is written to the destination operand. -- load
    'andn': {ALU},
    # Performs a bitwise logical AND of src1(reg) with src2(reg/mem). The result is stored in dst(reg). -- load
    'cqo': {ALU},
    # copies the sign (bit 63) of the value in the RAX register into every bit position in the RDX register.
    'vpbroadcastb': {ALU},
    # Broadcasts a 8-bit, 16-bit, 32-bit or 64-bit value from a general-purpose register (the second operand) to all the locations in the destination vector register (the first operand) using the writemask k1.
    'blsi': {ALU},
    # Extracts the lowest set bit from src(reg/mem) and set the corresponding bit in the destination register. If no bits are set in the source operand, BLSI sets all the bits in the destination to 0 and sets ZF and CF. -- load
    'pslld': {ALU},
    # Shifts the bits in the individual data elements in dest(reg) to the left by the number of bits specified in the src (mem/reg) -- load
    'vpbroadcastq': {ALU},
    'setnz': {ALU},  # sets dst(reg/mem) by the condition set on flags -- store
    'punpcklqdq': {ALU},
    ##Unpacks and interleaves the low-order data elements of the destination operand (first operand) and source operand (second operand) into the destination operand - src(mem/register) dst(register) -- load
    'shlx': {ALU},
    # Shifts the bits of src1(reg/mem) to the left by a COUNT value specified in src2(reg). The result is written to the destination operand. -- load
    'vsubsd': {FP},
    # Subtract the low double-precision floating-point value in src2(reg/mem) from src1(reg) and stores the double-precision floating-point result in the low quadword of dst(reg)-- load
    'cmpxchg': {ALU},
    # Compares the value in the AL,AX,EAX,or RAX with dst(reg/mem). If the two values are equal, the src(reg) is loaded into dst. Otherwise, dst is loaded into the AL,AX,EAX or RAX -- store
    'data16': {NOP},  # the same as NOP (halt the exec -- do nothing)
    'vmovaps': {FP},
    # Moves 4, 8 or 16 single-precision floating-point values from src(reg/mem) to dst(reg/mem) -- load/store
    'vucomisd': {FP},
    # Performs an unordered compare of the double-precision FP values in the low quadwords of src1(reg) and src2(reg/mem), and sets the ZF, PF, and CF flags in the EFLAGS register according to the result -- load
    'tzcnt': {ALU},
    # TZCNT counts the number of trailing least significant zero bits in src(reg/mem) and returns the result in dst(reg) -- load
    'vpcmpgtb': {ALU},
    # Performs a SIMD compare in src1(reg) and src2(reg/mem). If equal, the corresponding data element in dst(reg) is set to all 1s; otherwise, it is set to all 0s. -- load
    'vaddsd': {FP},  # dst - reg, src1 - reg, src2 - reg/mem -- load
    'cmovle': {LOAD},  ##conditional move (use EFLAGS reg)-- load/store
    'pmovmskb': {LOAD},
    # Creates a mask made up of the most significant bit of each byte of src(mem/reg) and stores the result in the low byte or word of dst(reg) -- load
    'pxor': {ALU},  # dst = dst (reg) xor src (reg/mem) -- load
    'dec': {ALU},  # dst = dst -1 -- dst is reg or mem -- store/load
    'cmovnle': {LOAD},  ##conditional move (use EFLAGS reg)-- load/store
    'vmulsd': {FP},  ## dst(reg) = src1(reg) * src2(reg/mem) -- load
    'and': {ALU},  # dst(reg/mem) = dst * src1(reg/mem/imm) -- load/store
    'punpckhqdq': {ALU},
    # Unpacks and interleaves the high-order data elements of the destination operand (first operand) and source operand (second operand) into the destination operand - src(mem/register) dst(register) -- load
    'movaps': {LOAD},  # FP move - src(reg/mem) dst(reg/mem) -- load/store
    'syscall': {LOAD},  # saves instruction in RCX and loads RIP from the IA32_LSTAR MSR -- load
    'lock': {BLOCK},
    'pslldq': {ALU},  # shift - src is imm and dst is reg
    'vmaxsd': {FP},
    # Compares the low double-precision floating-point values in src1 and src2, and returns the maximum value to the low quadword dst -- load (dst-reg, src1-reg, src2-reg/mem)
    'cpuid': {ALU},
    ##Returns processor identification and feature information to the EAX, EBX, ECX, and EDX registers, as determined by input entered in EAX (in some cases, ECX as well)
    'psllq': {ALU},
    # Shifts the bits in the individual data elements in dst(reg) to the left by the number of bits specified in src1(reg/mem/imm) -- load
    'bswap': {ALU},  # Reverses the byte order of a 32-bit or 64-bit (destination) register.
    'inc': {ALU},  # dst = dst + 1 -- dst is reg or mem -- store/load
    'vpand': {ALU},  # dst(reg) = src1(reg) and src2(reg/mem) --- load
    'paddd': {ALU},  # dst(reg) = dst + src(reg/mem) -- load
    'cdqe': {ALU},
    # The CWDE (convert word to double-word) instruction copies the sign (bit 15) of the word in the AX register into the high 16 bits of the EAX register.
    'vmovups': {FP},
    # Move unaligned packed single-precision floating-point from src(reg/mem) to dst(reg/mem) -- load/store
    'idiv': {ALU},
    # Divides the (signed) value in the AX, DX:AX, or EDX:EAX (dividend) by src(reg/mem) (divisor) and stores the result in the AX (AH:AL), DX:AX, or EDX:EAX registers -- load
    'pcmpgtd': {ALU},  # if (dst > src) dst = all 1s : dst = all 0s -- dst-reg, src-reg/mem -- load
    'paddq': {ALU},  # dst(reg) = dst + src(reg/mem) -- load
    'bsr': {ALU},
    # Searches the source for the MSB (1 bit). If a MSB is found, its bit index is stored in the destination. If source == 0, the content of the destination is undefined -- dst-reg, src-reg/mem -- load
    'vpcmpistri': {ALU},
    # Perform a packed comparison of string data with implicit lengths, generating an index, and storing the result in ECX. dst(ECX), src1(reg), src2(reg/mem), src3(imm) -- load
    'vinserti128': {ALU},
    # Insert 128 bits of integer data from xmm3/m128 and the remaining values from ymm2 into ymm1 -- VINSERTI128 ymm1, ymm2, xmm3/m128, imm8 (ymm1,ymm2,xmm3 = registers, m128 = mem, imm8 = immediate) --load
    'setle': {ALU},
    # Sets the destination operand to 0 or 1 depending on the settings of the status flags -- dst can be reg or mem -- store
    'bsf': {ALU},
    ##Searches the source for the LSB (1 bit). If a LSB is found, its bit index is stored in the destination. If source == 0, the content of the destination is undefined -- dst-reg, src-reg/mem -- load
    'vminsd': {FP},  # if( src1 < src2) dst = src1, else dst = src2 -- src1(reg),src2(reg/mem), dst(reg) -- load
    'punpcklbw': {ALU},
    # Unpacks and interleaves the low-order data elements of the destination operand (first operand) and source operand (second operand) into the destination operand - src(mem/register) dst(register) -- load
    'xsavec': {STORE},  # Save state components specified by EDX:EAX to mem with compaction
    'vmovdqa': {LOAD},  # Move aligned packed integer values from src1(reg/mem) to dst(reg/mem) -- load/store
    'cmovnz': {ALU},  ##conditional move (use EFLAGS reg)-- load/store
    'setb': {ALU},
    # Sets the destination operand to 0 or 1 depending on the settings of the status flags-- dst is reg or mem -- store
    'pshufb': {ALU},  # Shuffle bytes in dst(reg) according to contents of src1(reg/mem) -- load
    'cmovns': {LOAD},  ##conditional move (use EFLAGS reg)-- load/store
    'vpalignr': {ALU},
    # Concatenate src1(reg) and src2(reg/mem), extract byte aligned result shifted to the right by constant value in imm8(src3) and result is stored in dst(reg) -- load
    'pshufd': {ALU},
    # Shuffle the doublewords in src1(reg/mem) based on the encoding in imm8(src2) and store the result in dst(reg) -- load
    'cmovnl': {LOAD},  ##conditional move (use EFLAGS reg)-- load/store
    'vmovdqu': {LOAD},  # Move unaligned packed integer values from src1(reg/mem) to dst(reg/mem) -- load/store
    'movsxd': {LOAD},  # Move doubleword to quadword with sign-extension -- src1(reg/mem), dst(reg) -- load
    'cmovnb': {LOAD},  ##conditional move (use EFLAGS reg)-- load/store
    'setz': {ALU},
    # Sets the destination operand to 0 or 1 depending on the settings of the status flags-- dst is reg or mem -- store
    'setnl': {ALU},
    # Sets the destination operand to 0 or 1 depending on the settings of the status flags-- dst is reg or mem -- store
    'shld': {ALU},
    # Shift dst(reg/mem) to left src2(imm/reg CL) places while shifting bits from src1(reg) in from the right -- store
    'fnstcw': {STORE},
    # Store FPU control word to dst(mem) without checking for pending unmasked floating-point exceptions.
    'movsd': {LOAD},
    # For legacy mode, move dword from address DS:(E)SI to ES:(E)DI. For 64-bit mode move dword from address (R|E)SI to (R|E)DI.
    'vmovapd': {FP},
    # Move aligned packed double-precision floating-point values from src1(reg/mem) to dst(reg/mem) -- load/store
    'mulx': {ALU},
    # Performs an unsigned multiplication of implicit src(EDX/RDX) and src1(reg/mem) and stores the low half of the result in dst2(reg), the high half of the result in dst1(reg) -- load
    'andpd': {FP},  # dst (reg) = dst and src(reg/mem)  FP values -- load
    'ucomisd': {FP},
    # Compare low double-precision floating-point values in src1(reg) and src2(reg/mem) and set the EFLAGS flags accordingly -- load
    'vpminub': {ALU},
    # Compare packed unsigned byte integers in src1(reg) and src2(reg/mem) and store packed minimum values in dst(reg).
    'movapd': {LOAD},
    # Move aligned packed double-precision floating-point values from src(reg/mem) to dst(reg/mem) -- load/store
    'sarx': {ALU},
    # Shifts the bits of src1(reg/mem) to the right by a COUNT value specified in src2(reg). The result is written to dst(reg) -- load
    'setnb': {ALU},
    # Sets the destination operand to 0 or 1 depending on the settings of the status flags-- dst is reg or mem -- store
    'adc': {ALU},
    # Adds dst(reg/mem), src(reg/mem/imm), and the carry (CF) flag and stores the result in dst -- load/store
    'cdq': {ALU},
    # The CDQ instruction copies the sign (bit 31) of the value in the EAX register into every bit position in the EDX register.
    'movmskpd': {LOAD},
    # Extracts the sign bits from the packed double-precision floating-point values in src(reg), formats them into a 2-bit mask, and stores the mask in dst(reg)
    'repne': {ALU},
    ##repeats a string instruction number of times. The REPNE is a prefix for the instruction -- check which command
    'vcvtsi2ss': {FP},
    # Convert one signed doubleword integer from src(reg/mem) to one single-precision floating-point value in dst(reg) -- load
    'addsd': {FP},
    # Add the low double-precision floating-point value from src(reg/mem) to dst(reg) and store the result in dst(reg) -- load
    'shrd': {ALU},
    # Shift dst(reg/mem) to right src2(imm/reg CL) places while shifting bits from src1(reg) in from the left -- store
    'vcvtss2sd': {FP},
    # Convert one single-precision floating-point value in src2(reg/mem) to one double-precision floating-point value and merge with high bits of src1(reg) and store in dst(reg) -- load
    'vdivss': {FP},
    # Divide low single-precision floating-point value in src1(reg) by low single-precision floating-point value in src2(reg/mem) and store in dst(reg) -- load
    'vmovss': {LOAD},
    # VMOVSS dst, src1, src2 == Merge scalar single-precision floating-point value from src1 and src2 to dst register OR VMOVSS dst, src1 == dst = src1 -- dst(reg/mem), src1(reg/mem) -- load/store
    'ldmxcsr': {LOAD},  # Load MXCSR register from mem
    'popcnt': {ALU},
    # This instruction calculates the number of bits set to 1 in the second operand (source-reg/mem) and returns the count in the first operand (a destination register) -- load
    'stmxcsr': {STORE},
    # Stores the contents of the MXCSR control and status register to the destination operand. The destination operand is a 32-bit memory location. The reserved bits in the MXCSR register are stored as 0s.
    'fldcw': {LOAD},  # Load FPU control word from memory
    'vxorps': {FP},
    # Return the bitwise logical XOR of packed single-precision floating-point values in src1(reg) and src2(reg/mem) into dst(reg) -- load
    'setnp': {ALU},
    # Sets the destination operand to 0 or 1 depending on the settings of the status flags-- dst is reg or mem -- store
    'movss': {LOAD},  # can be load/store
    'vfmadd132sd': {FP},
    # Multiply scalar double-precision floating-point value from dst(reg) and src2(reg/mem), add to src1(reg) and put result in dst -- load
    'addss': {FP},
    # Add the low single-precision floating-point value from src(reg/mem) to dst(reg) and store the result in dst(reg) -- load
    'vfmadd213sd': {FP},
    # Return the bitwise logical AND of packed double-precision floating-point values in src1(reg) and src2(reg/mem) to dst(reg) -- load
    'vpinsrd': {ALU},
    # Insert a dword integer value from src2(reg/mem) and rest from src1(reg) into dst(reg) at the dword offset in src3(imm) -- load
    'vmulss': {FP},
    # Multiply the low single-precision floating-point value in src2(reg/mem) by the low single-precision floating-point value in src1(reg) to dst(reg) -- load
    'vucomiss': {FP},
    # Compare low single-precision floating-point values in dst(reg) and src1(reg/mem) and set the EFLAGS flags accordingly -- load
    'vpinsrw': {ALU},
    # Insert a word integer value from src2(reg/mem) and rest from src1(reg) into dst(reg) at the word offset in src3(imm) -- load
    'vfmadd231sd': {FP},
    # Multiply scalar double-precision floating-point value from src1(reg) and src2(reg/mem), add to dst(reg) and put result in dst(reg) -- load
    'vaddss': {FP},
    # Add the low single-precision floating-point value from src2(reg/mem) to src1(reg) and store the result in dst(reg) -- load
    'vandpd': {FP},
    # Return the bitwise logical AND of packed double-precision floating-point values in src1(reg) and src2(reg/mem) and stores in dst(reg) -- load
    'vcvttss2si': {FP},
    # Convert one single-precision floating-point value from src(reg/mem) to one signed doubleword integer in dst(reg) using truncation -- load
    'vcvtsd2ss': {FP},
    # Convert one double-precision floating-point value in src2(reg/mem) to one single-precision floating-point value and merge with high bits in src1(reg) and store in dst(reg) -- load
    'vcvttsd2si': {FP},
    # Convert one double-precision floating-point value from src(reg/mem) to one signed doubleword integer in dst(reg) using truncation -- load
    'vpunpckldq': {ALU},
    # Unpacks and interleaves the low-order data elements of src1 and src2 into dst - src1 (reg) src2(mem/register) dst(register) -- load
    'vpmovzxwd': {ALU},
    # Zero extend 4 packed 16-bit integers in the low 8 bytes of src(reg/mem) to 4 packed 32-bit integers in dst(reg) -- load
    'vpcmpeqd': {ALU},
    # Compare packed doublewords in src2(reg/mem) and src1(reg) for equality. and result is stored in dst(reg) -- load
    'mulsd': {FP},
    # Multiply the low double-precision floating-point value in src(reg/mem) by low double-precision floating-point value in dst(reg) -- load
    'divsd': {FP},
    # Divide low double-precision floating-point value in dst(reg) by low double-precision floating-point value in src(reg/mem) and store result in dst -- load
    'subsd': {FP},
    # Subtract the low double-precision floating-point value in src(reg/mem) from dst(reg) and store the result in dst(reg) -- load
    'vsqrtsd': {FP},
    # Computes square root of the low double-precision floating-point value in src2(reg/mem) and stores the results in dst(reg). Also, upper double-precision floating-point value (bits[127:64]) from src1(reg) is copied to dst127:64] -- load
    'xrstor': {LOAD},  # Restore state components specified by EDX:EAX from mem.
    'stosq': {ALU},
    'stosb': {ALU},
    'xadd': {ALU},
    'movsq': {LOAD}

}

InstructionsWithoutDestination = {"test", "cmp", "ucomisd"}
exceptional = {"pcmpistri", "cmpxchg", "vpcmpistri", "xchg", "set", "stosq", "stosb", "syscall", }

maxCommandsNumber = 0

Registers = {
    "rax": {0},
    "eax": {0},
    "ax": {0},
    "al": {0},
    "ah": {0},
    "rcx": {1},
    "ecx": {1},
    "cx": {1},
    "cl": {1},
    "ch": {1},
    "rdx": {2},
    "edx": {2},
    "dx": {2},
    "dl": {2},
    "dh": {2},
    "rbx": {3},
    "ebx": {3},
    "bx": {3},
    "bl": {3},
    "bh": {3},
    "rsi": {4},
    "esi": {4},
    "si": {4},
    "sil": {4},
    "rdi": {5},
    "edi": {5},
    "di": {5},
    "dil": {5},
    "rsp": {6},
    "esp": {6},
    "sp": {6},
    "spl": {6},
    "rbp": {7},
    "ebp": {7},
    "bp": {7},
    "bpl": {7},
    "r8": {8},
    "r8d": {8},
    "r8w": {8},
    "r8b": {8},
    "r9": {9},
    "r9d": {9},
    "r9w": {9},
    "r9b": {9},
    "r10": {10},
    "r10d": {10},
    "r10w": {10},
    "r10b": {10},
    "r11": {11},
    "r11d": {11},
    "r11w": {11},
    "r11b": {11},
    "r12": {12},
    "r12d": {12},
    "r12w": {12},
    "r12b": {12},
    "r13": {13},
    "r13d": {13},
    "r13w": {13},
    "r13b": {13},
    "r14": {14},
    "r14d": {14},
    "r14w": {14},
    "r14b": {14},
    "r15": {15},
    "r15d": {15},
    "r15w": {15},
    "r15b": {15},
    "xmm0": {16},
    "xmm1": {17},
    "xmm2": {18},
    "xmm3": {19},
    "xmm4": {20},
    "xmm5": {21},
    "xmm6": {22},
    "xmm7": {23},
    "ymm0": {24},
    "ymm1": {25},
    "ymm2": {26},
    "ymm3": {27},
    "ymm4": {28},
    "ymm5": {29},
    "ymm6": {30},
    "ymm7": {31},
    "fs": {32},
    "rip": {33},
    "fpu": {34},
    "xmm8": {35},
    "MXCSR": {36}

}
path = input('Please enter your path to the trace you wish to analyze!\n')
pathForAnalyed = input('Please enter your path to the file you wish to write to!\n')
file2 = open(pathForAnalyed, "w+")
sameInstructions = set()
newIns1 = None
newIns2 = None
ins = None


def ConatinsSubstring(address, substring="0x"):
    if substring in address:
        return True
    return False


def removeSubString(line,
                    disallowed_characters=[",", " ptr", " dword", " qword", " word", " byte", " xmmword", " ymmword",
                                           "lock "]):
    for character in disallowed_characters:
        line = line.replace(character, "")
    line1 = line
    return line1


def getValueOfRegister(address):
    # [rip+0x227853] =
    for key in Registers:
        if ConatinsSubstring(address, key):
            return Registers.get(key)
    return -1


def findAllRegistersInInstruction(instruction):
    allRegisters = []
    ins = removeSubString(instruction)
    ins1 = ins.split(" ")
    indexs = []
    for i, word in enumerate(ins1):
        if i == 0:
            continue
        for key in Registers.keys():
            if ConatinsSubstring(word, key):
                value = Registers.get(key)
                value1 = value.pop()
                regTemp = [x for x, _ in allRegisters]
                if value1 not in regTemp or i not in indexs:
                    allRegisters.append((value1, instruction.index(key)))
                    indexs.append(i)
                value.add(value1)

    return allRegisters


def findInstructionUnit(instruction):
    ins = removeSubString(instruction)
    ins1 = ins.split(" ")
    if ConatinsSubstring(ins1[0], "j"):
        return BRANCH
    value1 = Instructions.get(ins1[0]).pop()
    if value1 == LOAD:
        Instructions.get(ins1[0]).add(LOAD)
        if ConatinsSubstring(ins1[1], "]"):
            return STORE
        else:
            return LOAD
    else:
        Instructions.get(ins1[0]).add(value1)
        value = Instructions.get(ins1[0])
        value1 = value.pop()
        value.add(value1)
        return value1


def xadd_ins(instruction):
    global file2
    global maxCommandsNumber
    registers = findAllRegistersInInstruction(instruction)
    #file2.write(instruction)
    maxCommandsNumber -= 1
    file2.write("-2 0" + " " + "-1" + " " + str(registers[0][0]) + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")  # load
    maxCommandsNumber -= 1
    file2.write("-2 2" + " " + "-1" + " " + str(registers[1][0]) + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")  # add
    maxCommandsNumber -= 1
    file2.write("-2 0" + " " + str(registers[1][0]) + " " + "-1" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")  # load to source
    file2.write("-2 1" + " " + "-1" + " " + "-1" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")  # store to desination
    return None


def imul_ins(instruction):
    global file2
    global maxCommandsNumber
    ins = removeSubString(instruction)
    ins1 = ins.split(" ")
    operandsNum = len(ins1) - 1
    #file2.write(instruction)
    registers = findAllRegistersInInstruction(instruction)
    sources = []
    needForLoad = False
    for reg in registers:
        sources.append(reg[0])

    if ConatinsSubstring(instruction, "["):
        needForLoad = True
    if operandsNum == 1:
        if not needForLoad:
            maxCommandsNumber -= 1
            file2.write("2" + " " + "0" + " " + "0" + " " + str(sources[0]) + " " + "-1" + " " + str(
            maxCommandsNumber) + "\n")
            file2.write("2" + " " + "2" + " " + "0" + " " + str(sources[0]) + " " + "-1" + " " + str(
            maxCommandsNumber) + "\n")
        else:
            if len(sources) == 1:
                maxCommandsNumber -= 1
                file2.write("0" + " " + "-1" + " " + str(sources[0]) + " " + "-1" + " " + "-1" + " " + str(
            maxCommandsNumber) + "\n")
                maxCommandsNumber -= 1
                file2.write("2" + " " + "0" + " " + "0" + " " + "-1" + " " + "-1" + " " + str(
            maxCommandsNumber) + "\n")
                file2.write("2" + " " + "2" + " " + "0" + " " + "-1" + " " + "-1" + " " + str(
            maxCommandsNumber) + "\n")
            else:  # len == 2
                maxCommandsNumber -= 1
                file2.write("0" + " " + "-1" + " " + str(sources[0]) + " " + str(sources[1]) + " " + "-1" + " " + str(
            maxCommandsNumber) + "\n")
                maxCommandsNumber -= 1
                file2.write("2" + " " + "0" + " " + "0" + " " + "-1" + " " + "-1" + " " + str(
            maxCommandsNumber) + "\n")
                file2.write("2" + " " + "2" + " " + "0" + " " + "-1" + " " + "-1" + " " + str(
            maxCommandsNumber) + "\n")

    if operandsNum == 2:
        if not needForLoad:
            maxCommandsNumber -= 1
            if len(sources) == 1:
                file2.write("2" + " " + str(sources[0]) + " " + str(sources[0]) + " " + "-1" + " " + "-1" + " " + str(
            maxCommandsNumber) + "\n")
            if len(sources) == 2:
                file2.write(
                    "2" + " " + str(sources[0]) + " " + str(sources[0]) + " " + str(sources[1]) + " " + "-1" + " " + str(
            maxCommandsNumber) + "\n")
        else:
            if len(sources) == 2:
                maxCommandsNumber -= 1
                file2.write("0" + " " + "-1" + " " + str(sources[1]) + " " + "-1" + " " + "-1" + " " + str(
            maxCommandsNumber) + "\n")
                maxCommandsNumber -= 1
                file2.write("2" + " " + str(sources[0]) + " " + str(sources[0]) + " " + "-1" + " " + "-1" + " " + str(
            maxCommandsNumber) + "\n")
            if len(sources) == 3:
                maxCommandsNumber -= 1
                file2.write("0" + " " + "-1" + " " + str(sources[1]) + " " + str(sources[2]) + " " + "-1" + " " + str(
            maxCommandsNumber) + "\n")
                maxCommandsNumber -= 1
                file2.write("2" + " " + str(sources[0]) + " " + str(sources[0]) + " " + "-1" + " " + "-1" + " " + str(
            maxCommandsNumber) + "\n")

    if operandsNum == 3:
        if not needForLoad:
            if len(sources) == 2:
                maxCommandsNumber -= 1
                # the last operand is imm !
                file2.write("2" + " " + str(sources[0]) + " " + str(sources[1]) + " " + "-1" + " " + "-1" + " " + str(
            maxCommandsNumber) + "\n")
        else:
            if len(sources) == 2:
                maxCommandsNumber -= 1
                file2.write("0" + " " + "-1" + " " + str(sources[1]) + " " + "-1" + " " + "-1" + " " + str(
            maxCommandsNumber) + "\n")
                maxCommandsNumber -= 1
                file2.write("2" + " " + str(sources[0]) + " " + "-1" + " " + "-1" + " " + "-1" + " " + str(
            maxCommandsNumber) + "\n")
            if len(sources) == 3:
                maxCommandsNumber -= 1
                file2.write("0" + " " + "-1" + " " + str(sources[1]) + " " + str(sources[2]) + " " + "-1" + " " + str(
            maxCommandsNumber) + "\n")
                maxCommandsNumber -= 1
                file2.write("2" + " " + str(sources[0]) + " " + "-1" + " " + "-1" + " " + "-1"  " " + str(
            maxCommandsNumber) ++ "\n")
    return None


def cqo_ins(instruction):
    global file2
    global maxCommandsNumber
    maxCommandsNumber -= 1
    #file2.write(instruction)
    file2.write("2" + " " + "2" + " " + "0" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
    return None


def cdqe_ins(instruction):
    global file2
    global maxCommandsNumber
    maxCommandsNumber -= 1
    #file2.write(instruction)
    file2.write("2" + " " + "0" + " " + "0" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
    return None


def pshufd_ins(instruction):
    global file2
    global maxCommandsNumber
    maxCommandsNumber -= 1
    #file2.write(instruction)
    registers = findAllRegistersInInstruction(instruction)
    file2.write("2" + " " + str(registers[0][0]) + " " + str(registers[1][0]) + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
    return None


def syscall_ins(instruction):
    global file2
    #file2.write(instruction)
    global maxCommandsNumber
    maxCommandsNumber -= 1
    file2.write("0" + " " + "1" + " " + "-1" + " " + "-1" + " " + "33" + " " + str(
        maxCommandsNumber) + "\n")
    file2.write("0" + " " + "33" + " " + "-1" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
    return None


def div_ins(instruction):
    global file2
    #file2.write(instruction)
    registers = findAllRegistersInInstruction(instruction)
    global maxCommandsNumber
    maxCommandsNumber -= 1
    if not ConatinsSubstring(instruction, "["):
        file2.write("2" + " " + "0" + " " + "2" + " " + "0" + " " + str(registers[0][0]) + " " + str(
            maxCommandsNumber) + "\n")
        file2.write("2" + " " + "2" + " " + "2" + " " + "0" + " " + str(registers[0][0]) + " " + str(
            maxCommandsNumber) + "\n")
    return None


def stos_ins(instruction):
    global file2
    global maxCommandsNumber
    maxCommandsNumber -= 1
    #file2.write(instruction)
    file2.write("1" + " " + "-1" + " " + "0" + " " + "5" + " " + "-1" + " " + str(
            maxCommandsNumber) + "\n")
    file2.write("2" + " " + "5" + " " + "5" + " " + "-1" + " " + "-1" + " " + str(
            maxCommandsNumber) + "\n")
    return None


def xchg_ins(instruction):
    global file2
    #file2.write(instruction)
    global maxCommandsNumber
    registers = findAllRegistersInInstruction(instruction)
    if not ConatinsSubstring(instruction, "["):
        maxCommandsNumber -= 1
        file2.write("2" + " " + str(registers[0][0]) + " " + str(registers[1][0]) + " " + "-1" + " " + "-1" + " " + str(
            maxCommandsNumber) + "\n")
        file2.write("2" + " " + str(registers[1][0]) + " " + str(registers[0][0]) + " " + "-1" + " " + "-1" + " " + str(
            maxCommandsNumber) + "\n")
    else:
        maxCommandsNumber -= 1
        if len(registers) == 2:
                file2.write("-2 0" + " " + "-1" + " " + str(registers[0][0]) + " " + "-1" + " " + "-1" + " " + str(
            maxCommandsNumber) + "\n")
                file2.write("-2 2" + " " + str(registers[1][0]) + " " + "-1" + " " + "-1" + " " + "-1" + " " + str(
            maxCommandsNumber) + "\n")
                file2.write("-2 1" + " " + "-1" + " " + str(registers[1][0]) + " " + "-1" + " " + "-1" + " " + str(
            maxCommandsNumber) + "\n")

    return None


def rep_cmpsb_ins(instruction):
        global file2
        global maxCommandsNumber
        #file2.write(instruction)
        maxCommandsNumber -= 1
        file2.write("0" + " " + "-1" + " " + "4" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
        maxCommandsNumber -= 1
        file2.write("-2 0" + " " + "1" + " " + "5" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
        maxCommandsNumber -= 1
        file2.write("2" + " " + "-1" + " " + "-1" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
        file2.write("2" + " " + "4" + " " + "4" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
        file2.write("2" + " " + "5" + " " + "5" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
        return None


def threeOcc(instruction, str_block=""):
    global file2
    global maxCommandsNumber
    registers = findAllRegistersInInstruction(instruction)
    value = findInstructionUnit(instruction)
    dst = "-1"
    sr1 = "-1"
    sr2 = "-1"
    sr3 = "-1"
    value1 = LOAD
    #file2.write(instruction)
    if value == ALU:  # load, add, store ----- the destination is the MEMORY :)
        value2 = ALU
        value3 = STORE
        if len(registers) == 1:
            sr1 = registers[0][0]
            maxCommandsNumber -= 1
            file2.write(str_block + str(value1) + " " + str(dst) + " " + str(sr1) + " " + str(sr2) + " " + str(sr3) +
                        " " + str(maxCommandsNumber) + "\n")
            maxCommandsNumber -= 1
            file2.write(str_block + str(value2) + " " + str(dst) + " " + "-1" + " " + "-1" + " " + "-1" + " " +
                        str(maxCommandsNumber) + "\n")
        if len(registers) == 2:
            sr1 = registers[0][0]
            sr2 = registers[1][0]
            if registers[1][1] > instruction.index("]"):
                sr2 = "-1"
            maxCommandsNumber -= 1
            file2.write(
                str_block + str(value1) + " " + str(dst) + " " + str(sr1) + " " + str(sr2) + " " + str(sr3) + " " + str(
                maxCommandsNumber) + "\n")
            if sr2 == "-1":
                sr2 = registers[1][0]
            else:
                sr2 = "-1"
            maxCommandsNumber -= 1
            file2.write(str_block + str(value2) + " " + str(dst) + " " + "-1" + " " + str(sr2) + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
        if len(registers) == 3:
            sr1 = registers[0][0]
            sr2 = registers[1][0]
            sr3 = registers[2][0]
            maxCommandsNumber -= 1
            file2.write(str_block + str(value1) + " " + str(dst) + " " + str(sr1) + " " + str(sr2) + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
            maxCommandsNumber -= 1
            file2.write(str_block + str(value2) + " " + str(dst) + " " + "-1" + " " + str(sr3) + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
        maxCommandsNumber -= 1
        file2.write(str_block + str(value3) + " " + str(dst) + " " + "-1" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) +"\n")

    if value == BRANCH:  # load , push( store), jmp (BRANCH) to rip
        if len(registers) == 1:
            sr1 = registers[0][0]
        if len(registers) == 2:
            sr1 = registers[0][0]
            sr2 = registers[1][0]
        if len(registers) == 3:  # this should never happens !
            sr1 = registers[0][0]
            sr2 = registers[1][0]
            sr3 = registers[2][0]
        value2 = STORE
        value3 = BRANCH
        maxCommandsNumber -= 1
        file2.write(str(value1) + " " + str(dst) + " " + str(sr1) + " " + str(sr2) + " " + str(sr3) + " " + str(
        maxCommandsNumber) +  "\n")
        maxCommandsNumber -= 1
        file2.write(str(value2) + " " + "-1" + " " + "25" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
        maxCommandsNumber -= 1
        file2.write(str(value3) + " " + "-1" + " " + "-1" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")


def two_occ(instruction):  # we have only load and we are not writing to the memory
    global file2
    registers = findAllRegistersInInstruction(instruction)
    global maxCommandsNumber
    value2 = findInstructionUnit(instruction)
    dst = "-1"
    sr1 = "-1"
    sr2 = "-1"
    sr3 = "-1"
    value1 = LOAD
    #file2.write(instruction)
    if instruction.split(" ")[0] != "call" and instruction.split(" ")[
        0] != "ret" and value2 != BRANCH:
        if instruction.split(" ")[0] in InstructionsWithoutDestination:
            if instruction.split(" ")[0] == "ucomisd":
                if len(registers) == 2:
                    sr1 = -1
                    sr2 = registers[1][0]
                    maxCommandsNumber -= 1
                    file2.write(str(value1) + " " + str(dst) + " " + str(sr1) + " " + str(sr2) + " " + str(sr3) + " " + str(
        maxCommandsNumber) + "\n")
                    sr1 = registers[0][0]
                    sr2 = -1
                    maxCommandsNumber -= 1
                    file2.write(str(value2) + " " + "-1" + " " + "-1" + " " + str(sr1) + " " + "-1" +" " + str(
        maxCommandsNumber) +  "\n")
                if len(registers) == 3:
                    sr1 = registers[0][0]
                    sr2 = registers[1][0]
                    sr3 = registers[2][0]
                    maxCommandsNumber -= 1
                    file2.write(str(value1) + " " + "-1" + " " + str(sr2) + " " + str(sr3) + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
                    maxCommandsNumber -= 1
                    file2.write(str(value2) + " " + "-1" + " " + "-1" + " " + "-1" + " " + str(sr1) + " " + str(
        maxCommandsNumber) + "\n")
                return None
            if len(registers) == 1:
                sr1 = registers[0][0]
                maxCommandsNumber -= 1
                file2.write(str(value1) + " " + "-1" + " " + str(sr1) + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
                maxCommandsNumber -= 1
                file2.write(str(value2) + " " + "-1" + " " + "-1" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
            if len(registers) == 2:
                sr1 = registers[0][0]
                sr2 = registers[1][0]
                if registers[1][1] > instruction.index("]"):
                    sr2 = "-1"
                maxCommandsNumber -= 1
                file2.write(str(value1) + " " + str(dst) + " " + str(sr1) + " " + str(sr2) + " " + str(sr3) + " " + str(
        maxCommandsNumber) + "\n")
                if sr2 == "-1":
                    sr2 = registers[1][0]
                else:
                    sr2 = "-1"
                maxCommandsNumber -= 1
                file2.write(str(value2) + " " + "-1" + " " + "-1" + " " + str(sr2) + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
            if len(registers) == 3:
                sr1 = registers[0][0]
                sr2 = registers[1][0]
                sr3 = registers[2][0]
                maxCommandsNumber -= 1
                file2.write(str(value1) + " " + "-1" + " " + str(sr1) + " " + str(sr2) + " " + "-1" + " " + str(
        maxCommandsNumber) +  "\n")
                maxCommandsNumber -= 1
                file2.write(str(value2) + " " + "-1" + " " + "-1" + " " + "-1" + " " + str(sr3) + " " + str(
        maxCommandsNumber) + "\n")
            return None
        if len(registers) == 1:
            if instruction.split(" ")[0] == 'stmxcsr' or instruction.split(" ")[0] == 'ldmxcsr':
                sr1 = registers[0][0]
                maxCommandsNumber -= 1
                file2.write("0" + " " + "-1" + " " + "-1" + " " + str(sr1) + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
                maxCommandsNumber -= 1
                file2.write("0" + " " + "36" + " " + "-1" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
            if instruction.split(" ")[0] == 'fldcw':
                sr1 = registers[0][0]
                maxCommandsNumber -= 1
                file2.write(str(value1) + " " + "-1" + " " + str(sr1) + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
                maxCommandsNumber -= 1
                file2.write(str(value1) + " " + "34" + " " + "-1" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
            if ConatinsSubstring(instruction.split(" ")[0], "set"):
                sr1 = registers[0][0]
                maxCommandsNumber -= 1
                file2.write(str(value2) + " " + "-1" + " " + "-1" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
                maxCommandsNumber -= 1
                file2.write("1" + " " + "-1" + " " + "-1" + " " + str(sr1) + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
            if instruction.split(" ")[0] == 'fnstcw':
                sr1 = registers[0][0]
                maxCommandsNumber -= 1
                file2.write("0" + " " + "-1" + " " + "34" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
                maxCommandsNumber -= 1
                file2.write("1" + " " + "-1" + " " + "-1" + " " + str(sr1) + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
            if instruction.split(" ")[0] == 'xsavec':
                sr1 = registers[0][0]
                maxCommandsNumber -= 1
                file2.write("1" + " " + "-1" + " " + "2" + " " + str(sr1) + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
                maxCommandsNumber -= 1
                file2.write("1" + " " + "-1" + " " + "0" + " " + str(sr1) + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
            if instruction.split(" ")[0] == 'xrstor':
                sr1 = registers[0][0]
                maxCommandsNumber -= 1
                file2.write("0" + " " + "0" + " " + "-1" + " " + str(sr1) + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
                maxCommandsNumber -= 1
                file2.write("0" + " " + "2" + " " + "-1" + " " + str(sr1) + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")

        if len(registers) == 2:
            if instruction.split(" ")[0] == 'stmxcsr' or instruction.split(" ")[0] == 'ldmxcsr':
                sr1 = registers[0][0]
                sr2 = registers[1][0]
                maxCommandsNumber -= 1
                file2.write("0" + " " + "-1" + " " + str(sr2) + " " + str(sr1) + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
                maxCommandsNumber -= 1
                file2.write("0" + " " + "36" + " " + "-1" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
                return None
            if instruction.split(" ")[0] == 'fldcw':
                sr1 = registers[0][0]
                sr2 = registers[1][0]
                maxCommandsNumber -= 1
                file2.write(str(value1) + " " + "-1" + " " + str(sr1) + " " + str(sr2) + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
                maxCommandsNumber -= 1
                file2.write(str(value1) + " " + "34" + " " + "-1" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
                return None
            if instruction.split(" ")[0] == 'fnstcw':
                sr1 = registers[0][0]
                maxCommandsNumber -= 1
                file2.write("0" + " " + "-1" + " " + "34" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
                maxCommandsNumber -= 1
                file2.write("1" + " " + "-1" + " " + str(sr2) + " " + str(sr1) + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
                return None
            if instruction.split(" ")[0] == 'xsavec':
                sr1 = registers[0][0]
                sr2 = registers[1][0]
                maxCommandsNumber -= 1
                file2.write("1" + " " + "-1" + " " + "2" + " " + str(sr1) + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
                maxCommandsNumber -= 1
                file2.write("1" + " " + "-1" + " " + "0" + " " + str(sr2) + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
                return None
            if instruction.split(" ")[0] == 'xrstor':
                sr1 = registers[0][0]
                sr2 = registers[1][0]
                maxCommandsNumber -= 1
                file2.write("0" + " " + "0" + " " + str(sr1) + " " + str(sr2) + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
                maxCommandsNumber -= 1
                file2.write("0" + " " + "2" + " " + str(sr1) + " " + str(sr2) + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
                return None
            dst = registers[0][0]
            sr1 = registers[0][0]
            sr2 = registers[1][0]
            if instruction.split(" ")[0] == "bsr":
                sr1 = -1
            maxCommandsNumber -= 1
            file2.write(str(value1) + " " + "-1" + " " + "-1" + " " + str(sr2) + " " + str(sr3) + " " + str(
        maxCommandsNumber) + "\n")
            maxCommandsNumber -= 1
            file2.write(str(value2) + " " + str(dst) + " " + "-1" + " " + str(sr1) + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")

        if len(registers) == 3:
            dst = registers[0][0]
            sr1 = registers[0][0]
            sr2 = registers[1][0]
            sr3 = registers[2][0]
            if instruction.split(" ")[0] == "bsr":
                sr1 = -1
            maxCommandsNumber -= 1
            file2.write(str(value1) + " " + "-1" + " " + "-1" + " " + str(sr2) + " " + str(sr3) + " " + str(
        maxCommandsNumber) + "\n")
            maxCommandsNumber -= 1
            file2.write(str(value2) + " " + str(dst) + " " + "-1" + " " + str(sr1) + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")

    if value2 == BRANCH and instruction.split(" ")[0] != "call" and instruction.split(" ")[
        0] != "ret":  # load and BRANCH
        if len(registers) == 1:
            sr1 = registers[0][0]
        if len(registers) == 2:
            sr1 = registers[0][0]
            sr2 = registers[1][0]
        if len(registers) == 3:  # this should never happens !
            sr1 = registers[0][0]
            sr2 = registers[1][0]
            sr3 = registers[2][0]
        maxCommandsNumber -= 1
        file2.write(str(value1) + " " + str(dst) + " " + str(sr1) + " " + str(sr2) + " " + str(sr3) + " " + str(
        maxCommandsNumber) + "\n")
        maxCommandsNumber -= 1
        file2.write(str(value2) + " " + str(dst) + " " + "-1" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) +  "\n")

    if instruction.split(" ")[0] == "call":  # push and jmp
        maxCommandsNumber -= 1
        file2.write("1" + " " + "-1" + " " + "25" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
        maxCommandsNumber -= 1
        file2.write(str(value2) + " " + "-1" + " " + "-1" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
    if instruction.split(" ")[0] == "ret":  # pop and jmp
        maxCommandsNumber -= 1
        file2.write("0" + " " + "25" + " " + "-1" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
        maxCommandsNumber -= 1
        file2.write(str(value2) + " " + "-1" + " " + "-1" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
    if instruction.split(" ")[0] == 'xgetbv':
        maxCommandsNumber -= 1
        file2.write("0" + " " + "0" + " " + "-1" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
        maxCommandsNumber -= 1
        file2.write("0" + " " + "2" + " " + "-1" + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
    if instruction.split(" ")[0] == "vpalignr":
        if len(registers) == 4:
            sr3 = registers[3][0]
        sr2 = registers[2][0]
        maxCommandsNumber -= 1
        file2.write(str(value1) + " " + "-1" + " " + str(sr2) + " " + str(sr3) + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")
        maxCommandsNumber -= 1
        file2.write("2" + " " + str(registers[0][0]) + " " + str(registers[1][0]) + " " + "-1" + " " + "-1" + " " + str(
        maxCommandsNumber) + "\n")


def one_occ(
        instruction):  # one occcurnce means that we are going to excute this one instruction and no need of extra memory
    global file2
    global maxCommandsNumber
    maxCommandsNumber -= 1
    registers = findAllRegistersInInstruction(instruction)
    value = findInstructionUnit(instruction)
    dst = "-1"
    sr1 = "-1"
    sr2 = "-1"
    sr3 = "-1"
    #file2.write(instruction)

    if len(registers) == 1:  # mov ptr [rdx], 0x1 --> rdx not a dst  , add rdx,01
        if instruction.split(" ")[0] not in InstructionsWithoutDestination:
            dst = str(registers[0][0])
        if (value == ALU or value == FP):
            sr1 = str(registers[0][0])
        if (value == STORE):
            dst = "-1"
            sr1 = str(registers[0][0])
        if value == BRANCH:
            dst = -1
            sr1 = str(registers[0][0])
    if len(registers) == 2:  # mov  xmmword ptr [rdi*1-0x10], 0x7
        if instruction.split(" ")[0] not in InstructionsWithoutDestination:
            dst = str(registers[0][0])
        sr2 = str(registers[1][0])
        if (value == ALU or value == FP) and instruction.split(" ")[0] != "bsf" and instruction.split(" ")[
            0] != "bsr" and instruction.split(" ")[0] != 'popcnt' and instruction.split(" ")[0] != 'vpslldq' and \
                instruction.split(" ")[0] != 'vpbroadcastb':
            sr1 = str(registers[0][0])
        if value == STORE:
            dst = -1
            sr1 = str(registers[0][0])
    if len(registers) == 3:
        dst = str(registers[0][0])
        sr1 = str(registers[1][0])
        sr2 = str(registers[2][0])
        if (value == STORE):
            dst = -1
            sr1 = str(registers[0][0])
            sr2 = str(registers[1][0])
            sr3 = str(registers[2][0])
        if instruction.split(" ")[0] == "shld" or instruction.split(" ")[0] == "shrd":
            sr3 = dst
    if instruction.split(" ")[0] == "rdtsc":

        file2.write("0 0 -1 -1 -1" + " " + str(maxCommandsNumber) + "\n")  # eax
        file2.write("0 2 -1 -1 -1"+ " " + str(maxCommandsNumber) + "\n")  # edx
        return None
    if ConatinsSubstring(instruction.split(" ")[0], "set"):
        sr1 = "-1"

    file2.write(str(value) + " " + str(dst) + " " + str(sr1) + " " + str(sr2) + " " + str(sr3) + " " + str(
        maxCommandsNumber) + "\n")



file = open(path, "r")
Lines = file.readlines()
newLines = []
for line in Lines:
    if line[0] != "[" and ConatinsSubstring(line, ":"):
        newLines.append(line)
        maxCommandsNumber += 1
file.close()
instructionss = set()
flag2 = False
flag3 = False
for i, line in enumerate(newLines):
    lineSplit1 = newLines[i].split(": ")
    ins = lineSplit1[1].split(" ")
    str_block = ""
    if ins[0] not in Instructions.keys() and not ConatinsSubstring(ins[0], "j"):
        print(ins[0])
        continue
    if ins[0] == 'lfence\n':
        maxCommandsNumber -= 1
        file2.write("-2 5" + " " + "-1" + " " + "-1" + " " + "-1" + " " + "-1" + " " + str(
            maxCommandsNumber) + "\n")
        continue
    if ins[0] == "lock":
        if ins[1] == "xadd":
            xadd_ins(lineSplit1[1])
            continue
        else:
            str_block = "-2 "
    if ins[0] == "imul" or ins[0] == "mul":
        imul_ins(lineSplit1[1])
        continue
    if ins[0] == 'cqo':
        cqo_ins(lineSplit1[1])
        continue
    if ins[0] == "cdqe":
        cdqe_ins(lineSplit1[1])
        continue
    if ins[0] == "pshufd":
        pshufd_ins(lineSplit1[1])
        continue
    if ins[0] == "syscall":
        syscall_ins(lineSplit1[1])
        continue
    if ins[0] == "rep":
        if ins[1] == "cmpsb":
            if not flag3:
                rep_cmpsb_ins(lineSplit1[1])
                flag3 = True
                continue
            else:
                if not flag2:
                    flag2 = True
                    continue
                else:
                    flag2 = False
                    flag3 = False
                    continue

    if ins[0] == "rep":
        if ConatinsSubstring(ins[1], "stos"):
            stos_ins(lineSplit1[1])
            continue
    if ins[0] == "nop" or ins[0] == "nop\n":
        maxCommandsNumber -= 1
        file2.write("5" + " " + "-1" + " " + "-1" + " " + "-1" + " " + "-1" + " " + str(
            maxCommandsNumber) + "\n")
        continue
    if ins[0] == "xchg":
        xchg_ins(lineSplit1[1])
        continue
    if ins[0] == "div" or ins[0] == "idiv":
        div_ins(lineSplit1[1])
        continue
    lineSplit2 = None
    lineSplit3 = None
    if i + 1 <= len(newLines) - 1:
        lineSplit2 = newLines[i + 1].split(": ")
    if i + 2 <= len(newLines) - 1:
        lineSplit3 = newLines[i + 2].split(": ")
    if lineSplit2 is None:
        if not flag2:
            instructionss.add(lineSplit1[0])
            one_occ(lineSplit1[1])
            break

    if lineSplit3 is None:
        if not flag3:
            if lineSplit1[0] == lineSplit2[0]:
                two_occ(lineSplit1[1])
            else:
                one_occ(lineSplit1[1])
                one_occ(lineSplit2[1])
        break

    if lineSplit1[0] == lineSplit2[0] and lineSplit1[0] == lineSplit3[0]:
        flag3 = True
        flag2 = False
        threeOcc(lineSplit1[1], str_block)

    else:
        if lineSplit1[0] == lineSplit2[0] and lineSplit1[0] != lineSplit3[0]:
            if not flag3:
                two_occ(lineSplit1[1])
                flag2 = True
                flag3 = False
            else:
                flag2 = True
                flag3 = False
        else:
            if not flag2:
                one_occ(lineSplit1[1])
                flag3 = False
            else:
                flag2 = False
                flag3 = False

print("finish")
file2.close()
exit()
