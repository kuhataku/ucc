#include <stdio.h>
#include "ucc.h"

static Map *vars;
static int bpoff = 8;

void gen_lval(Node *node) {
  if (node->ty == ND_IDENT) {
    int off = (intptr_t)map_get(vars, node->name);
    if ( off == NULL) {
      off = bpoff;
      map_put(vars, node->name, (void *)(intptr_t)bpoff);
      bpoff += 8;
      printf("  sub rsp, 8\n");
    }
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", off);
    printf("  push rax\n");
    return;
  }
  fprintf(stderr, "代入の左辺値が変数ではありません\n");
}

void gen(Node *node) {
  if (node->ty == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  if (node->ty == ND_IDENT) {
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  }

  if (node->ty == '=') {
    gen_lval(node->lhs);
    gen(node->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->ty) {
  case '+':
    printf("  add rax, rdi\n");
    break;
  case '-':
    printf("  sub rax, rdi\n");
    break;
  case '*':
    printf("  mul rdi\n");
    break;
  case '/':
    printf("  mov rdx, 0\n");
    printf("  div rdi\n");
    break;
  case ND_EQUAL:
    printf("  cmp rdi, rax\n");
    printf("  sete al\n");
    printf("  movzx rax, al\n");
    break;
  case ND_NEQUAL:
    printf("  cmp rdi, rax\n");
    printf("  setne al\n");
    printf("  movzx rax, al\n");
    break;
  }

  printf("  push rax\n");
}

void gen_ir(Vector *code) {

  vars = new_map();

  printf(".intel_syntax noprefix\n");
  printf(".global _main\n");
  printf("_main:\n");
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");

  for (int i = 0; code->data[i]; i++) {
    gen(code->data[i]);
    printf("  pop rax\n");
  }

  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
}

