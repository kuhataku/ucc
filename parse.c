#include <stdlib.h>
#include <ctype.h>
#include "ucc.h"

Token tokens[100];
Node *code[100];

static int pos = 0;
static int c = 0;

enum {
  TK_NUM = 256,
  TK_IDENT,
  TK_EOF,
};

Node *term();
Node *mul();
Node *expr();
Node *assign();

Node *new_node(int ty, Node *lhs, Node *rhs) {
  Node *node = malloc(sizeof(Node));
  node->ty = ty;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = malloc(sizeof(Node));
  node->ty = ND_NUM;
  node->val = val;
  return node;
}

Node *new_node_ident(char *val) {
  Node *node = malloc(sizeof(Node));
  node->ty = ND_IDENT;
  node->name = *val;
  return node;
}

Node *term() {
  if (tokens[pos].ty == TK_NUM) {
    return new_node_num(tokens[pos++].val);
  }
  if (tokens[pos].ty == TK_IDENT) {
    return new_node_ident(tokens[pos++].input);
  }
  if (tokens[pos].ty == '(') {
    pos++;
    Node *node = expr();
    if (tokens[pos].ty != ')') {
      error("開きカッコに対応する閉じカッコがありません", pos);
    }
    pos++;
    return node;
  }
  error("数値でも開きカッコでもないトークンです", pos);
  return NULL;
}

Node *mul() {
  Node *lhs = term();
  if (tokens[pos].ty == '*') {
    pos++;
    return new_node('*', lhs, mul());
  }
  if (tokens[pos].ty == '/') {
    pos++;
    return new_node('/', lhs, mul());
  }
  return lhs;
}

Node *expr() {
  Node *lhs = mul();
  if (tokens[pos].ty == '+') {
    pos++;
    return new_node('+', lhs, expr());
  }
  if (tokens[pos].ty == '-') {
    pos++;
    return new_node('-', lhs, expr());
  }
  return lhs;
}

Node *assign() {
  Node *lhs = expr();
  if (tokens[pos].ty == '=') {
    pos++;
    return new_node('=', lhs, expr());
  }
  return lhs;
}

void program() {
  Node *lhs = assign();
  if (tokens[pos].ty == ';') {
    pos++;
    code[c++] = lhs;
  }
  if (tokens[pos].ty == TK_EOF) {
    code[c] = NULL;
    return;
  } else {
    program();
  }
}

void tokenize(char *p) {
  int i = 0;
  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' ||
        *p == ')' || *p == ';' || *p == '=') {
      tokens[i].ty = *p;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }

    if ('a' <= *p && *p <= 'z') {
      tokens[i].ty = TK_IDENT;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }

    if (isdigit(*p)) {
      tokens[i].ty = TK_NUM;
      tokens[i].input = p;
      tokens[i].val = strtol(p, &p, 10);
      i++;
      continue;
    }

    error("トークナイズできません", i);
    exit(1);
  }

  tokens[i].ty = TK_EOF;
  tokens[i].input = p;
}

