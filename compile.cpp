#include "report.h"
#include "next.h"
#include <bits/stdc++.h>
using namespace std;

FILE* outputFile;

void argument(int argc, char* argv[]) {
  if (argc != 3) {
    printHelp();
    report("no input.\n");
  }
  loadCode(argv[1]);
  if (! (outputFile = fopen(argv[2], "w"))) 
    report("Can't open %s.\n", argv[2]);
}

int instructionCount;

void out(int count, const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  fprintf(outputFile, fmt, ap);
  va_end(ap);
  instructionCount += count;
}

struct Variable {
  int location;
  vector<int> dimension;
  
  Variable(int loc = 0): location(loc) {
  }
  
  int size() {
    int ret = 1;
    for (int i = 0, _i = (int) dimension.size(); i < _i; i++)
      ret *= dimension[i];
    return ret;
  }
};

set<string> reserved;
map<string, Variable> gvarMap, lvarMap; // global & local

void init() {
  reserved.insert("end");
  reserved.insert("repeat");
  reserved.insert("forward");
  reserved.insert("reduce");
  reserved.insert("def");
  reserved.insert("continue");
  reserved.insert("break");
  reserved.insert("return");
}

void parseStatement() {
  if (curToken == tok_opt && tokString == ":") {
    nextToken();
  }
}

int main(int argc, char* argv[]) {
  argument(argc, argv);
  init();
  out(2, "JMP main_enter\n");
  
  while (curToken != tok_eof) {
    if (curToken != tok_str) reportCompileError("unknown identifier.");
    if (tokString == "def") {
      lvarMap.clear();
      nextToken(tok_str);
      string name = tokString;
      consume("(", tok_opt || tok_str);
      
      int parameter = 0;
      if (! (curToken == tok_opt && tokString == ")")) {
        nextToken(tok_str);
        while (true) {
          lvarMap[tokString] = parameter++;
          nextToken(tok_opt);
          if (tokString == ")") break;
          if (tokString != ",") reportCompileError("comma expected.");
          nextToken(tok_str);
        }
      }
      nextToken(tok_opt);
      if (tokString != ":") reportCompileError("colon expected.");
      parseStatement();
    }
  }
  
  //out("@main_enter %d\n");
  return 0;
}
