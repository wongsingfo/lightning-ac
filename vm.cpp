#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <vector>
#include <string>
#define MemoryPool 0x100000 * 1
#include "vm.h"

std::vector<long long> text;

enum {submit = 1, interpre, debug};
int Todo;
char* code;
char* p_code;

void printHelp() {
  printf("exit code:\n"
        "  0  successfully\n"
        "  -1 file error\n"
        "  -2 compile error\n"
        "\n"
        "usage:\n"
        "  S    generate a code for submission online\n"
        "  I    interpre\n"
        "  D    debug\n"
        "\n"
        "example:\n"
        "  ./vm S code.vm\n");
}

#define report(...) fprintf(stderr, __VA_ARGS__), exit(-1)

void argument(int argc, char* argv[]) {
  if (argc != 3) {printHelp(); report("no input.\n");}

  if (argv[1][0] == 'S') Todo = submit;
  else if (argv[1][0] == 'I') Todo = interpre;
  else if (argv[1][0] == 'D') Todo = debug;

  if (! Todo) report("nothing to be done.\n");
  char* fileName = argv[2];
#define report_close(...) fclose(input), report(__VA_ARGS__)
  if (FILE* input = fopen(fileName, "r")) {
    if (fseek(input, 0, SEEK_END)) report_close("seek_end error.\n");
    int lenCode = ftell(input);
    code = new char[lenCode + 1];
    if (fseek(input, 0, SEEK_SET)) report_close("seek_set error.\n");
    if (lenCode != (int) fread(input, 1, lenCode, input)) report_close("couldn't read the code!\n");
    code[lenCode] = 0; // append '\0'
    if (fclose(input)) report("couldn't close '%s'", fileName);
  }
  else report("couldn't open '%s'", fileName);
#undef report_close
}

int line, column;
char* line_front;

void report_compile_error(const char* const msg) {
  while (*line_front != '\n' && *line_front) 
    fprintf(stderr, "%c", *line_front);
  fprintf(stderr, "\n");
  for (int i = 2; i < column; i++) fprintf(stderr, " ");
  fprintf(stderr, "^\nerror: %s\n", msg);
  exit(-2);
}

int nextChar() {
  if (*p_code == '\0') return EOF;
  if (p_code == code || *(p_code - 1) == '\n') {
    line++; 
    column = 1;
    line_front = p_code;
  }
  else column++;
  return *p_code++;
}

enum token {
  tok_num = -100,
  tok_label,
  tok_eof,
};

int CurToken;
long long Number;
std::string StrToken;

int counter;
std::map<std::string, long long> label;

void nextToken(int take_place = 1) {
#define MAXBUFFER 256
  static char s[MAXBUFFER];
  static int c;
  if (p_code == code) c = ' ';
  while (1) {
    while (isspace(c) && c != EOF) c = nextChar();
    if (c == ';') while (c != '\n' || c != EOF) c = nextChar();
    else break;
  }
  if (c == EOF) {
    CurToken = tok_eof;
    return;
  }
  if (! isalpha(c) && ! isdigit(c)) {
    CurToken = c;
    return;
  }

  int len = 0;
  while (isalpha(c) || isdigit(c)) {
    s[len++] = c;
    if (len + 2 >= MAXBUFFER) report_compile_error("identifier too long.");
  }
  s[len] = 0;
#undef MAXBUFFER

  if (take_place) counter++; // !

  int isNum = 1;
  for (int i = 0; i < len; i++) 
    if (! isdigit(s[i])) {isNum = 0; break;}
  if (isNum) {
    if (1 != sscanf(s, "%lld", &Number)) report_compile_error("unknown number.");
    return;
  }

  CurToken = tok_eof;
  if (! strcmp(s, "LOD")) Number = LOD;
  if (! strcmp(s, "IMM")) Number = IMM;
  if (! strcmp(s, "LEA")) Number = LEA;
  if (! strcmp(s, "JMP")) Number = JMP;
  if (! strcmp(s, "JZ" )) Number = JZ;
  
  if (! strcmp(s, "JNZ")) Number = JNZ;
  if (! strcmp(s, "STO")) Number = STO;
  if (! strcmp(s, "PSH")) Number = PSH;
  if (! strcmp(s, "ENT")) Number = ENT;
  if (! strcmp(s, "LEV")) Number = LEV;
  
  if (! strcmp(s, "CALL")) Number = CALL;
  if (! strcmp(s, "EXIT")) Number = EXIT;

  /* ------------------------------ */
  if (! strcmp(s, "ADD")) Number = ADD;
  if (! strcmp(s, "SUB")) Number = SUB;
  if (! strcmp(s, "MUL")) Number = MUL;
  if (! strcmp(s, "DIV")) Number = DIV;
  if (! strcmp(s, "MOD")) Number = MOD;

  if (! strcmp(s, "EQ")) Number = EQ;
  if (! strcmp(s, "NE")) Number = NE;
  if (! strcmp(s, "LT")) Number = LT;
  if (! strcmp(s, "GT")) Number = GT;
  if (! strcmp(s, "LE")) Number = LE;

  if (! strcmp(s, "GE")) Number = GE;
  if (! strcmp(s, "SHL")) Number = SHL;
  if (! strcmp(s, "SHR")) Number = SHR;

  /* ------------------------------ */
  if (! strcmp(s, "RINT")) Number = RINT;
  if (! strcmp(s, "WINT")) Number = WINT;
  if (! strcmp(s, "GETC")) Number = GETC;
  if (! strcmp(s, "PUTC")) Number = PUTC;
  if (CurToken != tok_eof) return;

  StrToken.assign(s);
  if (label.count(StrToken)) {
    Number = label[StrToken];
    CurToken = tok_num;
  }
  else CurToken = tok_label;
}

void init() {
  p_code = code;
  line = column = 0;
  nextToken();
}

void cat(const char* const s) {
  if (FILE* f = fopen(s, "r")) {
    int t;
    while (EOF != (t = fgetc(f))) putchar(t);
    if (fclose(f)) report("couldn't close file '%s'", s);
    putchar(10);
  }
  else report("couldn't find '%s'", s);
}

int main(int argc, char* argv[]) {
  argument(argc, argv);
  for (int i = 0; i < 2; i++) {
    init();
    if      (CurToken == tok_num) {
      if (i) text.push_back(Number);
      nextToken();
    }
    else if (CurToken == '@') {
      nextToken(0);
      if (CurToken != tok_label && i == 0) report_compile_error("expected label");
      std::string s = StrToken;
      nextToken(0);
      if (CurToken == '=') {
        nextToken(0);
        if (i == 0) {
          if (CurToken != tok_num) report_compile_error("expected number");
          label[s] = Number;
        }
        nextToken(0);
      }
      else if (i == 0) {
        label[s] = counter;
      }
    }
    else report_compile_error("unknown token");
  }
  delete []code;
  
  if (Todo == submit) {
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
