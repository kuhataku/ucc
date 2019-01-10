#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ucc.h"

void error(char *message, int i) {
  fprintf(stderr, "%s :%s \n", message, tokens[i].input);
  exit(1);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }
  if ( strcmp(argv[1], "-test") == 0 ) {
    runtest();
    return 0;
  }

  tokenize(argv[1]);
  program();

  printf(".intel_syntax noprefix\n");
  printf(".global _main\n");
  printf("_main:\n");
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n");

  for (int i = 0; code[i]; i++) {
    gen(code[i]);
    printf("  pop rax\n");
  }

  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  return 0;
}
