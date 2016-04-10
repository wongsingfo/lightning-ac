#ifdef as_header
void debug_handle(int cur, long long reg, int sp);
void debug_scanf(long long &reg);
void debug_getchar(long long &reg);
#endif

#ifndef ONLINE_JUDGE
#define errorp(...) fprintf(stderr, __VA_ARGS__)
//#define vmdebug
#include "instruction.h"
#else
#define errorp(...) ;
#endif

long long m[MemoryPool];

#ifdef vmdebug
void print_instruction(FILE* stream, int line, long long i, long long j) {
  fprintf(stream, "%4d: ", line);
  if      (i == LOD) fprintf(stream, "LOD");
  else if (i == IMM) fprintf(stream, "IMM");
  else if (i == LEA) fprintf(stream, "LEA");
  else if (i == JMP) fprintf(stream, "JMP");
  else if (i == JZ ) fprintf(stream, "JZ");

  else if (i == JNZ) fprintf(stream, "JNZ");
  else if (i == STO) fprintf(stream, "STO");
  else if (i == PSH) fprintf(stream, "PSH");
  else if (i == ENT) fprintf(stream, "ENT");
  else if (i == LEV) fprintf(stream, "LEV");

  else if (i == CALL) fprintf(stream, "CALL");
  else if (i == EXIT) fprintf(stream, "EXIT");
  else if (i == ADJ) fprintf(stream, "ADJ");

  else if (i == ADD) fprintf(stream, "ADD");
  else if (i == SUB) fprintf(stream, "SUB");
  else if (i == MUL) fprintf(stream, "MUL");
  else if (i == DIV) fprintf(stream, "DIV");
  else if (i == MOD) fprintf(stream, "MOD");

  else if (i == EQ ) fprintf(stream, "EQ");
  else if (i == NE ) fprintf(stream, "NE");
  else if (i == XOR) fprintf(stream, "XOR");
  else if (i == LT ) fprintf(stream, "LT");
  else if (i == GT ) fprintf(stream, "RT");

  else if (i == LE ) fprintf(stream, "LE");
  else if (i == GE ) fprintf(stream, "GE");
  else if (i == SHL) fprintf(stream, "SHL");
  else if (i == SHR) fprintf(stream, "SHR");
  else if (i == INC) fprintf(stream, "INC");

  else if (i == DEC) fprintf(stream, "DEC");

  else if (i == RINT) fprintf(stream, "RINT");
  else if (i == WINT) fprintf(stream, "WINT");
  else if (i == GETC) fprintf(stream, "GETC");
  else if (i == PUTC) fprintf(stream, "PUTC");
  else {fprintf(stream, "unknown instruction=%lld\n", i); exit(-1);}

  if (i == IMM ||
      i == LEA ||
      i == JMP ||
      i == JZ  ||
      i == JNZ ||
      i == ENT ||
      i == CALL||
      i == ADJ
      ) fprintf(stream, " %lld", j);
  fprintf(stream, "\n");
}
#endif

int VMRun() {
  long long i;
  long long cur = 0;
  long long bp = MemoryPool; // bottom pointer
  long long sp = MemoryPool; // stack pointer
  long long reg; // only one register

  m[--sp] = EXIT; // call exit if main exit
  m[--sp] = PSH; 
  sp--; m[sp] = sp + 1;
  
  while (1) {
#ifdef as_header
    debug_handle(cur, reg, sp);
#endif

    i = m[cur++]; // instruction
#ifdef vmdebug
#ifdef debugregister
    fprintf(stderr, "-------------------------------------register: %lld\n", reg);
#endif
#ifdef debugstack
    int j;
    fprintf(stderr, "-----------------stack:");
    for (j = MemoryPool - 1; j >= sp; j--) fprintf(stderr, "%lld ", m[j]);
    fprintf(stderr, "\n");
#endif
    print_instruction(stderr, cur - 1, i, m[cur]);
#endif
    // load 
    if      (i == LOD) reg = m[reg];
    // load immediately or global address
    else if (i == IMM) reg = m[cur++];
    // load local address
    else if (i == LEA) reg = bp + m[cur++];
    // jump
    else if (i == JMP) cur = m[cur];
    // jump equal zero
    else if (i == JZ) if (!reg) cur = m[cur]; else cur++;
    // jump not equal zero
    else if (i == JNZ) if (reg) cur = m[cur]; else cur++;
    // store 
    else if (i == STO) m[m[sp++]] = reg;
    // push
    else if (i == PSH) m[--sp] = reg;
    // enter subroutine
    else if (i == ENT) {m[--sp] = bp; bp = sp; sp -= m[cur++];}
    // leave subroutine
    else if (i == LEV) {sp = bp, bp = m[sp++]; cur = m[sp++];}
    // jump to subroutine, call function
    else if (i == CALL) {m[--sp] = cur + 1; cur = m[cur];}
    // exit
    else if (i == EXIT) {errorp("exit(%lld)\n", m[sp]); break;}
    // adjust stack
    else if (i == ADJ) sp += m[cur++];

    // ==================== operation   ======================
    else if (i == ADD) reg = m[sp++] + reg;
    else if (i == SUB) reg = m[sp++] - reg;
    else if (i == MUL) reg = m[sp++] * reg;
    else if (i == DIV) reg = m[sp++] / reg;
    else if (i == MOD) reg = m[sp++] % reg;
    else if (i == EQ) reg = m[sp++] == reg;
    else if (i == NE) reg = m[sp++] != reg;
    else if (i == XOR)reg = m[sp++] ^ reg;
    else if (i == LT) reg = m[sp++] < reg;
    else if (i == GT) reg = m[sp++] > reg;
    else if (i == LE) reg = m[sp++] <= reg;
    else if (i == GE) reg = m[sp++] >= reg;
    else if (i == SHL) reg = m[sp++] << reg;
    else if (i == SHR) reg = m[sp++] >> reg;
    else if (i == INC) reg++;
    else if (i == DEC) reg--;

    // ==================== system call ======================
    else if (i == RINT) 
#ifdef as_header
      debug_scanf(reg);
#else
      scanf("%lld", &reg);
#endif
    else if (i == WINT) printf("%lld", reg);
    else if (i == GETC)
#ifdef as_header
      debug_getchar(reg);
#else
      reg = getchar();
#endif
    else if (i == PUTC) putchar(reg);
    else {errorp("unknown instruction = %lld\n", i); return -1;}
  }

  return 0;
}
