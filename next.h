#ifndef NEXT_H
#define NEXT_H

#include <vector>
#include <string>
using namespace std;
#include "report.h"
#include "constants.h"

int curToken;
int tokNumber;
string tokString;

enum token {
  tok_num,
  tok_opt,
  tok_str,
};

/* 
 * void loadCode(const char* const file)
 * void printCodeLine(int line)
 * void reportCompileError(const char* const msg)
 */

static char *code;

static int line, column;
static vector<char*> line_front;

void loadCode(const char* const file) {
  FILE* input = fopen(file, "r");
  if (FILE* input = fopen(fileName, "r")) {
    if (fseek(input, 0, SEEK_END)) report_close("seek_end error.\n");
    int lenCode = ftell(input);
    code = new char[lenCode + 1];
    if (fseek(input, 0, SEEK_SET)) report_close("seek_set error.\n");
    if (lenCode != (int) fread(code, 1, lenCode, input)) report_close("couldn't read the code!\n");
    code[lenCode] = 0; // append '\0'
    if (fclose(input)) report("couldn't close '%s'\n", file);
    line = column = 0;
  }
  else report("couldn't open '%s'\n", file);
}

void reportCompileError(const char* const msg) {
  print_code_line(line);
  for (int i = 1; i < column + 5; i++) fprintf(stderr, " ");
  fprintf(stderr, "^\nerror: %s\n", msg);
  exit(-2);
}

void printCodeLine(int line) {
  fprintf(stderr, "%4d: ", line);
  char* front = line_front[line];
  while (*front!= '\n' && *front) 
    fprintf(stderr, "%c", *front++);
  fprintf(stderr, "\n");
}

static void nextChar() {
  if (*p_code == '\0') return EOF;
  if (p_code == code || *(p_code - 1) == '\n') {
    line++;
    column = 1;
    line_front.push_back(p_code);
  }
}

void nextToken() {
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
  }
}

#endif // NEXT_H
