#ifndef INSTRUCTION_H
#define INSTRUCTION_H
enum {
  LOD = 100, IMM, LEA, JMP, JZ, 
  JNZ, STO, PSH, ENT, LEV,
  CALL,EXIT, ADJ
};

enum operation {
  ADD = 200, SUB, MUL, DIV, MOD, 
  EQ, NE, XOR, LT, GT, 
  LE, GE, SHL, SHR, INC, 
  DEC, 
};

enum systemFunction {
  RINT = 300, WINT, GETC, PUTC
};
#endif // INSTRUCTION_H
