#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vm.h"

long long text[1000];

char* inputFile;
int submit, interpre;

void printHelp() {
  printf("usage:\n"
      "  -s    submit\n"
      "  -i    interpre\n"
      "\n"
      "example:\n"
      "  ./vm code.vm -s\n");
}

void report(const char* const msg) {
  fprintf(stderr, msg);
  exit(-1);
}

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
    else if (inputFile) report("too more files.\n");
    else inputFile = argv[i];
  }
  if (submit + interpre == 0) report("nothing to be done.\n");
}

int main(int argc, char* argv[]) {
  if (argc == 1) {
    printHelp();
    return 0;
  }
  argument(argc, argv);

  return 0;
}
