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
  vector<int> dimension;
  
  Variable() {}
  
  int size() {
    int ret = 1;
    for (int i = 0, _i = (int) dimension.size(); i < _i; i++)
      ret *= dimension[i];
    return ret;
  }
};

set<string> reserved;
map<string, int> priority;
map<string, int> parameterCount;
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
  
  priority['='] = 10;
  priority['+'] = priority['-'] = 20;
  priority['*'] = priority['/'] = 30;
}

void parseStatement() {
  if (curToken == tok_opt && tokString == ":") {
    nextToken();
    while (! (curToken != tok_str && tokString == "end")) parseExpression();
    nextToken();
  }
}

int main(int argc, char* argv[]) {
  argument(argc, argv);
  init();
  out(2, "JMP function_main\n");
  int function_main = false;
  
  while (curToken != tok_eof) {
    if (curToken != tok_str) reportCompileError("unknown identifier.");
    if (tokString == "def") {
      lvarMap.clear();
      nextToken(tok_str);
      string name = tokString;
      if (reserved.count(name)) reportCompileError("%s reserved.", name.c_str());
      out(2, "@function_%s ENT function_%s_variables\n", name.c_str(), name.c_str());
      consume("(", tok_opt || tok_str);
      
      int parameter = 0;
      if (! (curToken == tok_opt && tokString == ")")) {
        if (curToken != tok_str) reportCompileError("string expected.");
        while (true) {
          if (reserved.count(tokString)) 
            reportCompileError("%s reserved.", tokString.c_str());
          lvarMap[tokString] = parameter++;
          nextToken(tok_opt);
          if (tokString == ")") break;
          if (tokString != ",") reportCompileError("comma expected.");
          nextToken(tok_str);
        }
      }
      parameterCount[name] = parameter;
      if (name == "main") {
        if (parameter != 0)
          reportCompileError("main() must be without any parameters");
        function_main = true;
      }
      nextToken(tok_opt);
      if (tokString != ":") reportCompileError("colon expected.");
      for (map<string, Variable>::iterator iter = lvarMap.begin();
          iter != lvarMap.end(); iter++) {
        iter->second = parameter - iter->second + 1;
      }
      parseStatement();
      out(0, "@function_%s_variables = %d\n", lvarMap.size());
    }
    else {
      if (reserved.count(tokString)) 
        reportCompileError("%s reserved.", tokString.c_str());
      gvarMap[tokString] = 0;
    }
  }
  if (! function_main) report("where's the main()?\n");
  
  return 0;
}
