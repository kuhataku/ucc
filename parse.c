#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "ucc.h"

Vector *tokens;

static int pos = 0;

enum {
  TK_NUM = 256,
  TK_IDENT,
  TK_EQUAL,
  TK_NEQUAL,
  TK_EOF,
};

void error(char *message, int i) {
  fprintf(stderr, "%s :%s \n", message, ((Token *)tokens->data[i])->input);
  exit(1);
}

Node *term();
Node *mul();
Node *expr();
Node *assign();

Token *add_token(Vector *v, int ty, char *input) {
  Token *t = malloc(sizeof(Token));
  t->ty = ty;
  t->input = input;
  vec_push(v, t);
  return t;
}

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
  Token *token = tokens->data[pos];
  if (token->ty == TK_NUM) {
    pos++;
    return new_node_num(token->val);
  }
  if (token->ty == TK_IDENT) {
    pos++;
    return new_node_ident(token->input);
  }
  if (token->ty == '(') {
    pos++;
    Node *node = expr();
    token = tokens->data[pos];
    if (token->ty != ')') {
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
  Token *token = tokens->data[pos];
  if (token->ty == '*') {
    pos++;
    return new_node('*', lhs, mul());
  }
  if (token->ty == '/') {
    pos++;
    return new_node('/', lhs, mul());
  }
  return lhs;
}

Node *expr() {
  Node *lhs = mul();
  Token *token = tokens->data[pos];
  if (token->ty == '+') {
    pos++;
    return new_node('+', lhs, expr());
  }
  if (token->ty == '-') {
    pos++;
    return new_node('-', lhs, expr());
  }
  if (token->ty == TK_EQUAL) {
    pos++;
    return new_node(ND_EQUAL, lhs, expr());
  }
  if (token->ty == TK_NEQUAL) {
    pos++;
    return new_node(ND_NEQUAL, lhs, expr());
  }
  return lhs;
}

Node *assign() {
  Node *lhs = expr();
  Token *token = tokens->data[pos];
  if (token->ty == '=') {
    pos++;
    return new_node('=', lhs, expr());
  }
  return lhs;
}

Vector *program() {
  Vector *code = new_vector();
  while (1) {
    Token *token = tokens->data[pos];
    if (token->ty == TK_EOF) {
      break;
    } else {
      Node *lhs = assign();
      Token *token = tokens->data[pos];
      if (token->ty == ';') {
        pos++;
        vec_push(code, lhs);
      }
    }
  }
  vec_push(code, NULL);
  return code;
}

Vector *tokenize(char *p) {
  tokens = new_vector();
  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (!strncmp(p, "==", 2)) {
      add_token(tokens, TK_EQUAL, p);
      p+=2;
      continue;
    }

    if (!strncmp(p, "!=", 2)) {
      add_token(tokens, TK_NEQUAL, p);
      p+=2;
      continue;
    }

    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' ||
        *p == ')' || *p == ';' || *p == '=') {
      add_token(tokens, *p, p);
      p++;
      continue;
    }

    if ('a' <= *p && *p <= 'z') {
      add_token(tokens, TK_IDENT, p);
      p++;
      continue;
    }

    if (isdigit(*p)) {
      Token *t = add_token(tokens, TK_NUM, p);
      t->val = strtol(p, &p, 10);
      continue;
    }

    fprintf(stderr, "トークナイズできません %s", p);
    exit(1);
  }

  add_token(tokens, TK_EOF, p);
  return tokens;
}

