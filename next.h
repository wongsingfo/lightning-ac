#ifndef NEXT_H
#define NEXT_H

#include "report.h"
#include "constants.h"

#include <bits/stdc++.h>
using namespace std;

int curToken;
int tokNumber;
string tokString;

enum token {
  tok_eof = 1 << 0,
  tok_num = 1 << 1,
  tok_opt = 1 << 2,
  tok_str = 1 << 3,
};

void nextToken(int permit = -1);
void loadCode(const char* const file);
void printCodeLine(int line);
void reportCompileError(const char* msg, ...);
void consume(const char* s, int permit = -1);

static char *code;
static char *p_code;

static int line, column;
static vector<char*> line_front;

void loadCode(const char* const file) {
#define report_close(...) fclose(input), report(__VA_ARGS__)
  if (FILE* input = fopen(file, "r")) {
    if (fseek(input, 0, SEEK_END)) report_close("seek_end error.\n");
    int lenCode = ftell(input);
    code = new char[lenCode + 1];
    if (fseek(input, 0, SEEK_SET)) report_close("seek_set error.\n");
    if (lenCode != (int) fread(code, 1, lenCode, input)) 
      report_close("couldn't read the code!\n");
    code[lenCode] = 0; // append '\0'
    if (fclose(input)) report("couldn't close '%s'\n", file);
    line = column = 0;
  }
  else report("couldn't open '%s'\n", file);
#undef report_close

  p_code = code;
  nextToken();
}

void reportCompileError(const char* msg, ...) {
  printCodeLine(line);
  for (int i = 1; i < column + 5; i++) fprintf(stderr, " ");
  fprintf(stderr, "^\nerror: ");
  va_list ap;
  va_start(ap, msg);
  fprintf(stderr, msg, ap);
  va_end(ap);
  fprintf(stderr, "\n");
  exit(-2);
}

void printCodeLine(int line) {
  fprintf(stderr, "%4d: ", line);
  char* front = line_front[line];
  while (*front!= '\n' && *front) 
    fprintf(stderr, "%c", *front++);
  fprintf(stderr, "\n");
}

static int nextChar() {
  if (*p_code == '#') while (*p_code != '\n' && *p_code != '\0') p_code++;
  if (*p_code == '\0') return EOF;
  if (p_code == code || *(p_code - 1) == '\n') {
    line++;
    column = 0;
    line_front.push_back(p_code);
  }
  column++;
  return *p_code++;
}

void nextToken(int permit) {
  static int c;
  if (p_code == code) c = ' ';
  while (isspace(c) && c != EOF) c = nextChar();
  if (c == EOF) {
    curToken = tok_eof;
    return;
  }
  
  // name: (\w|_)[\w\d_]*
  if (isalpha(c) || c == '_') {
    curToken = tok_str;
    string &s = tokString;
    s = "";
    do {
      s += c;
      c = nextChar();
    }while (isalpha(c) || isdigit(c) || c == '_');
  }
  // number(not float) : \d*
  else if (isdigit(c)) {
    curToken = tok_num;
    int &x = tokNumber;
    x = 0;
    do {
      x = x * 10 + (c & 31);
      c = nextChar();
    }while (isdigit(c));
  }
  // operator 
  else {
    curToken = tok_opt;
    string &s = tokString;
    s = "";
    s += c;
    c = nextChar();
    
    if (c == '=') {
      if (s == "=" || s == "!" || s == "<" || s == ">") {
        s += c;
        c = nextChar();
      }
    }
    else if ((c == '<' && s == "<") || (c == '>' && s == ">")) {
      s += c;
      c = nextChar();
    }
    else if ((c == '|' && s == "|") || (c == '&' && s == "&")) {
      s += c;
      c = nextChar();
    }
  }
  
  if ((curToken & permit) == 0) {
    if (permit == tok_num) reportCompileError("number expected.");
    else if (permit == tok_str) reportCompileError("string expected.");
    else if (permit == (tok_str | tok_num))
      reportCompileError("string/number expected.");
    else reportCompileError("%d expected.", permit);
  }
}

void consume(const char* s, int permit) {
  nextToken();
  if (curToken != tok_str || tokString != s) 
    reportCompileError("%s expected.", s);
  nextToken(permit);
}

#endif // NEXT_H
