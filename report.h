#ifndef REPORT_H
#define REPORT_H

#define report(...) fprintf(stderr, __VA_ARGS__), exit(-1)

/*
 * void printHelp()
 */

void printHelp() {
  printf("\n");
}

#endif // REPORT_H