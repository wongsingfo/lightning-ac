#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <vector>
#define MemoryPool 0x100000 * 1
#include "vm.h"

std::vector<long long> text;

char* fileName;
FILE* inputFile;
int submit, interpre;

void printHelp() {
  printf("usage:\n"
      "  -s    submit\n"
      "  -i    interpre\n"
      "\n"
      "example:\n"
      "  ./vm code.vm -s\n");
}

#define report(...) fprintf(stderr, __VA_ARGS__), exit(-1)

void argument(int argc, char* argv[]) {
  int i;
  for (i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
        case 's':
          submit = 1;
          break;
        case 'i':
          interpre = 1;
          break;
        default:
          report("unknown argument.\n");
      }
    }
    else if (fileName) report("too more files.\n");
    else fileName = argv[i];
  }
  if (submit + interpre == 0) report("nothing to be done.\n");
  if (submit + interpre > 1) report("do one thing at one time.\n");
  if (! (inputFile = fopen(fileName, "r"))) 
    report("couldn't open file '%s'.\n", fileName);
}

long long nextIdentifier(int &end) {
  end = 0;
  static int line = 1;
  static char s[256];
  int c = fgetc(inputFile);
  if (c == EOF) {end = 1; return EOF;}
  while (1) {
    if (c == '\n') line++;
    if (c == ' ' || c == '\n' || c == '\r') c = fgetc(inputFile);
    else break;
  }
  int len = 0;
  s[len++] = c;
  while (1) {
    c = fgetc(inputFile);
    if (c == ';') while (c != '\n' && c != EOF) c = fgetc(inputFile);
    if (c == ' ' || c == '\n' || c == '\r' || c == EOF) break;
    s[len++] = c;
  }
  s[len] = 0;

  int i;
  int isNum = 1;
  for (i = 0; i < len; i++) 
    if (s[i] < '0' || s[i] > '9') {isNum = 0; break;}
  if (isNum) {
    long long num = 0;
    sscanf(s, "%lld", &num);
    return num;
  }

  // to upper
  for (i = 0; i < len; i++) 
    if (s[i] >= 'a' && s[i] <= 'z') s[i] ^= 1 << 5;

  if (! strcmp(s, "LOD")) return 100;
  if (! strcmp(s, "IMM")) return 101;
  if (! strcmp(s, "LEA")) return 102;
  if (! strcmp(s, "JMP")) return 103;
  if (! strcmp(s, "JZ" )) return 104;
  
  if (! strcmp(s, "JNZ")) return 105;
  if (! strcmp(s, "STO")) return 106;
  if (! strcmp(s, "PSH")) return 107;
  if (! strcmp(s, "ENT")) return 108;
  if (! strcmp(s, "LEV")) return 109;
  
  if (! strcmp(s, "CALL")) return 110;
  if (! strcmp(s, "EXIT")) return 111;

  /* ------------------------------ */
  if (! strcmp(s, "ADD")) return 200;
  if (! strcmp(s, "SUB")) return 201;
  if (! strcmp(s, "MUL")) return 202;
  if (! strcmp(s, "DIV")) return 203;
  if (! strcmp(s, "MOD")) return 204;

  if (! strcmp(s, "EQ")) return 205;
  if (! strcmp(s, "NE")) return 206;
  if (! strcmp(s, "LT")) return 207;
  if (! strcmp(s, "GT")) return 208;
  if (! strcmp(s, "LE")) return 209;

  if (! strcmp(s, "GE")) return 210;
  if (! strcmp(s, "SHL")) return 211;
  if (! strcmp(s, "SHR")) return 212;

  /* ------------------------------ */
  if (! strcmp(s, "RINT")) return 300;
  if (! strcmp(s, "WINT")) return 301;
  if (! strcmp(s, "GETC")) return 302;
  if (! strcmp(s, "PUTC")) return 303;

  report("unknown identifier '%s' at line %d", s, line);
}

void cat(const char* const s) {
  if (FILE* f = fopen(s, "r")) {
    int t;
    while (EOF != (t = fgetc(f))) putchar(t);
    if (fclose(f)) report("couldn't close file '%s'", s);
  }
  else report("couldn't find '%s'", s);
}

int main(int argc, char* argv[]) {
  if (argc == 1) {
    printHelp();
    return 0;
  }
  argument(argc, argv);

  while (1) {
    int eof;
    long long t = nextIdentifier(eof);
    if (eof) break;
    text.push_back(t);
  }
  
  if (submit) {
    printf("#include <stdio.h>\n"
           "#include <stdlib.h>\n");
    printf("#define MemoryPool %d\n", 0x100000 * 10);
    cat("vm.h");
    printf("const long long text[] = {");
    for (int i = 0, _i = text.size(); i < _i; i++) {
      printf("%lld", text[i]);
      if (text[i] >= (1LL << 31) || text[i] < -(1LL << 31)) printf("ll");
      if (i + 1 < _i) printf(",");
    }
    printf("};\n");
    printf("int main() {\n"
        "for (int i = 0; i < %d; i++) m[i] = text[i];\n", (int) text.size());
    printf("VMRun(0); return 0;}");
  }

  return 0;
}
