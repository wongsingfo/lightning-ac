#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <vector>
#include <string>
#define MemoryPool 0x100000 * 1

#ifndef as_header
#define as_header
#include "vm.h"
#include "instruction.h"
#undef as_header
#endif 

std::vector<long long> text;
std::vector<long long> text_line;
std::vector<char*> line_front;

enum Tool {submit = 1, interpret, debug} Todo;
char* code;
char* p_code;
FILE* inputFile;

void printHelp() {
  printf("exit code:\n"
        "  0  successfully\n"
        "  -1 file error\n"
        "  -2 compile error\n"
        "\n"
        "usage:\n"
        "  s    generate a code for submission online\n"
        "  i    interpret\n"
        "  d    debug\n"
        "\n"
        "example:\n"
        "  ./vm s code.vm input.txt\n");
}

#define report(...) fprintf(stderr, __VA_ARGS__), exit(-1)

void argument(int argc, char* argv[]) {
  if (argc != 3 && argc != 4) {printHelp(); report("no input.\n");}

  if (argv[1][0] == 's') Todo = submit;
  else if (argv[1][0] == 'i') Todo = interpret;
  else if (argv[1][0] == 'd') {
    Todo = debug;
    if (argc != 4) report("debug mode need an input file.\n");
  }

  if (! Todo) report("nothing to be done.\n");
  char* fileName = argv[2];
#define report_close(...) fclose(input), report(__VA_ARGS__)
  if (FILE* input = fopen(fileName, "r")) {
    if (fseek(input, 0, SEEK_END)) report_close("seek_end error.\n");
    int lenCode = ftell(input);
    code = new char[lenCode + 1];
    if (fseek(input, 0, SEEK_SET)) report_close("seek_set error.\n");
    if (lenCode != (int) fread(code, 1, lenCode, input)) report_close("couldn't read the code!\n");
    code[lenCode] = 0; // append '\0'
    if (fclose(input)) report("couldn't close '%s'\n", fileName);
  }
  else report("couldn't open '%s'\n", fileName);
#undef report_close

  if (argc == 4) {
   if (! (inputFile = fopen(argv[3], "r"))) report("couldn't open '%s'\n", argv[3]);
  }
}

int line, column;

void print_code_line(int line) {
  fprintf(stderr, "%4d: ", line);
  char* front = line_front[line];
  while (*front!= '\n' && *front) 
    fprintf(stderr, "%c", *front++);
  fprintf(stderr, "\n");
}

void report_compile_error(const char* const msg) {
  print_code_line(line);
  for (int i = 1; i < column + 5; i++) fprintf(stderr, " ");
  fprintf(stderr, "^\nerror: %s\n", msg);
  exit(-2);
}

int nextChar() {
  if (*p_code == '\0') return EOF;
  if (p_code == code || *(p_code - 1) == '\n') {
    line++; 
    column = 1;
    line_front.push_back(p_code);
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
std::map<std::string, long long> label; // used for 'jump' instruction

bool isidentifier(int c) {
  return isalpha(c) || isdigit(c) || c == '_' || c == '-';
}

void nextToken(int take_place = 1) {
#define MAXBUFFER 256
  static char s[MAXBUFFER];
  static int c;
  if (p_code == code) c = ' ';
  while (1) {
    while (isspace(c) && c != EOF) c = nextChar();
    if (c == ';') while (c != '\n' && c != EOF) c = nextChar();
    else break;
  }
  if (c == EOF) {
    CurToken = tok_eof;
    return;
  }
  if (! isidentifier(c)) {
    CurToken = c;
    c = nextChar();
    return;
  }

  int len = 0;
  while (isidentifier(c)) {
    s[len++] = c;
    if (len + 2 >= MAXBUFFER) report_compile_error("identifier too long.");
    c = nextChar();
  }
  s[len] = 0;
#undef MAXBUFFER

  //fprintf(stderr, "%s at %d\n", s, counter);
  if (take_place) counter++; // !

  int isNum = 1;
  for (int i = 0; i < len; i++) 
    if (! isdigit(s[i]) && s[i] != '-') {isNum = 0; break;}
  if (isNum) {
    if (1 != sscanf(s, "%lld", &Number)) report_compile_error("unknown number.");
    CurToken = tok_num;
    return;
  }

  CurToken = tok_eof;
  if      (! strcmp(s, "LOD")) Number = LOD, CurToken = tok_num;
  else if (! strcmp(s, "IMM")) Number = IMM, CurToken = tok_num;
  else if (! strcmp(s, "LEA")) Number = LEA, CurToken = tok_num;
  else if (! strcmp(s, "JMP")) Number = JMP, CurToken = tok_num;
  else if (! strcmp(s, "JZ" )) Number = JZ , CurToken = tok_num;
  
  else if (! strcmp(s, "JNZ")) Number = JNZ, CurToken = tok_num;
  else if (! strcmp(s, "STO")) Number = STO, CurToken = tok_num;
  else if (! strcmp(s, "PSH")) Number = PSH, CurToken = tok_num;
  else if (! strcmp(s, "ENT")) Number = ENT, CurToken = tok_num;
  else if (! strcmp(s, "LEV")) Number = LEV, CurToken = tok_num;
  
  else if (! strcmp(s, "CALL")) Number = CALL, CurToken = tok_num;
  else if (! strcmp(s, "EXIT")) Number = EXIT, CurToken = tok_num;
  else if (! strcmp(s, "ADJ"))  Number = ADJ,  CurToken = tok_num;

  /* ------------------------------ */
  else if (! strcmp(s, "ADD")) Number = ADD, CurToken = tok_num;
  else if (! strcmp(s, "SUB")) Number = SUB, CurToken = tok_num;
  else if (! strcmp(s, "MUL")) Number = MUL, CurToken = tok_num;
  else if (! strcmp(s, "DIV")) Number = DIV, CurToken = tok_num;
  else if (! strcmp(s, "MOD")) Number = MOD, CurToken = tok_num;

  else if (! strcmp(s, "EQ")) Number = EQ, CurToken = tok_num;
  else if (! strcmp(s, "NE")) Number = NE, CurToken = tok_num;
  else if (! strcmp(s, "XOR"))Number = XOR,CurToken = tok_num;
  else if (! strcmp(s, "LT")) Number = LT, CurToken = tok_num;
  else if (! strcmp(s, "GT")) Number = GT, CurToken = tok_num;

  else if (! strcmp(s, "LE")) Number = LE, CurToken = tok_num;
  else if (! strcmp(s, "GE" )) Number = GE , CurToken = tok_num;
  else if (! strcmp(s, "SHL")) Number = SHL, CurToken = tok_num;
  else if (! strcmp(s, "SHR")) Number = SHR, CurToken = tok_num;
  else if (! strcmp(s, "INC")) Number = INC, CurToken = tok_num;

  else if (! strcmp(s, "DEC")) Number = DEC, CurToken = tok_num;

  /* ------------------------------ */
  else if (! strcmp(s, "RINT")) Number = RINT, CurToken = tok_num;
  else if (! strcmp(s, "WINT")) Number = WINT, CurToken = tok_num;
  else if (! strcmp(s, "GETC")) Number = GETC, CurToken = tok_num;
  else if (! strcmp(s, "PUTC")) Number = PUTC, CurToken = tok_num;
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
  line_front.clear();
  line_front.push_back(0); // start from 1
  nextToken();
}

void clean() {
  delete []code;
  if (inputFile) fclose(inputFile);
}

void cat(const char* const s) {
  if (FILE* f = fopen(s, "rb")) {
    int t;
    while (EOF != (t = fgetc(f))) putchar(t);
    if (fclose(f)) report("couldn't close file '%s'", s);
    putchar(10);
  }
  else report("couldn't find '%s'", s);
}

void debug_scanf(long long &x) {
  if (inputFile) fscanf(inputFile, "%lld", &x);
  else scanf("%lld", &x);
}

void debug_getchar(long long &x) {
  if (inputFile) x = fgetc(inputFile);
  else x = getchar();
}

void debug_handle(int cur, long long reg, int sp) {
  if (Todo != debug) return;
  if (cur < 0 || cur >= (int) text.size()) {
    // the bottom of the stack is PSH EXIT
    if (cur != MemoryPool - 1 &&
        cur != MemoryPool - 2)
      fprintf(stderr, "debug error: the %%cur beyond the text.\n");
    return;
  }
  int curLine = text_line[cur];
  static int lastLine = -1;
  if (curLine == lastLine) return;
  static int hook = -1; // use for setting a breakpoint
  if (hook != -1) {
    if (curLine != hook) return;
    else hook = -1;
  }

  print_code_line(curLine);

  int command, adr;
  while (1) {
    fprintf(stderr, "debug> ");
    command = getchar();
    std::string s("");

    if (command != '\n') {
      adr = 0;
      do {
        if (adr != -1) {
          if (! isdigit(command)) adr = -1;
          else adr = adr * 10 + (15 & command);
        }
        s += command;
        command = getchar();
      }while (command != '\n');
    } 
    else {
      adr = -1;
      s = "n";
    }

    // next 
    if      (s == "n") {
      break;
    }
    // quit
    else if (s == "q") {
      fprintf(stderr, "quit now.\n");
      exit(0);
    }
    // show reg ans stk
    else if (s == "p") {
      fprintf(stderr, "      reg: %6lld stk:", reg);
      for (int i = MemoryPool - 1; i >= sp; i--) 
        fprintf(stderr, " %lld", m[i]);
      fprintf(stderr, "\n");
    }
    // show code
    else if (s == "l") {
      for (int i = std::max(1, curLine - 10), 
          _i = std::min(curLine + 10, (int) line_front.size() - 1);
          i < _i; i++) 
        print_code_line(i);
    }
    // show m[address] or set a breakpoint
    else if (adr != -1) {
      if (adr < 0 || adr >= MemoryPool) 
        fprintf(stderr, "debug error: address beyond the memory.\n");
      else if (adr >= (int) text.size())
        fprintf(stderr, "     memory[%d] = %lld\n", adr, m[adr]);
      else if (adr != curLine) {
        hook = adr;
        break;
      }
      else 
        fprintf(stderr, "nothing to do.\n");
    }
    // show varible or label
    else if (label.count(s)) {
      fprintf(stderr, "     %s = %lld\n", s.c_str(), label[s]);
    }
    else {
      //fprintf(stderr, "nothing to do.\n");
    }
  }
  while (command != '\n') command = getchar();

  lastLine = curLine;
}

int main(int argc, char* argv[]) {
  argument(argc, argv);
  for (int i = 0; i < 2; i++) {
    init();
    while (1) {
      if      (CurToken == tok_num) {
        if (i) {
          text.push_back(Number);
          if (Todo == debug) text_line.push_back(line);
        }
        nextToken();
      }
      else if (CurToken == '@') {
        nextToken(0);
        int location = counter; // jump to location
        if (CurToken != tok_label && i == 0) report_compile_error("expected label");
        std::string s = StrToken;
        nextToken();
        if (CurToken == '=') {
          nextToken(0);
          if (i == 0) {
            if (CurToken != tok_num) report_compile_error("expected number");
            label[s] = Number;
          }
          nextToken();
        }
        else if (i == 0) {
          //fprintf(stderr, "%s -> %d\n", s.c_str(), counter);
          label[s] = location;
        }
      }
      else if (CurToken == tok_eof) break;
      else if (i == 0) nextToken();
      else report_compile_error("unknown token");
    }
  }
  
  if (Todo == submit) {
    printf("#include <stdio.h>\n"
           "#include <stdlib.h>\n");
    printf("#define MemoryPool %d\n", (int) 1e6);
    cat("vm.h");
    printf("const long long text[] = {");
    for (int i = 0, _i = text.size(); i < _i; i++) {
      printf("%lld", text[i]);
      if (text[i] >= (1LL << 31) || text[i] < -(1LL << 31)) printf("ll");
      if (i + 1 < _i) printf(",");
    }
    printf("};\n");
    printf("int main() {\n"
        "int i; for (i = 0; i < %d; i++) m[i] = text[i];\n", (int) text.size());
    printf("VMRun();\n" 
        "return 0;}\n");
  }
  else if (Todo == interpret || Todo == debug) {
    for (int i = 0, _i = (int) text.size(); i < _i; i++) m[i] = text[i];
    VMRun();
  }
  else report("nothing to be done.\n");

  clean();
  return 0;
}
