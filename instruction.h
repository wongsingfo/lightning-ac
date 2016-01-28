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
