#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "ucc.h"

Vector *tokens;
Map *keywords;

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

int consume(int ty) {
  Token *token = tokens->data[pos];
  if (token->ty != ty)
    return 0;
  pos++;
  return 1;
}

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
  node->name = val;
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
    return new_node_ident(token->name);
  }
  
  if ( consume('(') ) {
    Node *node = expr();
    if ( ! consume(')') ) {
      error("開きカッコに対応する閉じカッコがありません", pos);
    }
    return node;
  }
  error("数値でも開きカッコでもないトークンです", pos);
  return NULL;
}

Node *mul() {
  Node *lhs = term();
  if ( consume('*')) {
    return new_node('*', lhs, mul());
  }
  if ( consume('/')) {
    return new_node('/', lhs, mul());
  }
  return lhs;
}

Node *expr() {
  Node *lhs = mul();
  if (consume('+')) {
    return new_node('+', lhs, expr());
  }
  if (consume('-')) {
    return new_node('-', lhs, expr());
  }
  if (consume(TK_EQUAL)) {
    return new_node(ND_EQUAL, lhs, expr());
  }
  if (consume(TK_NEQUAL)) {
    return new_node(ND_NEQUAL, lhs, expr());
  }
  return lhs;
}

Node *assign() {
  Node *lhs = expr();
  if (consume('=')) {
    return new_node('=', lhs, expr());
  }
  return lhs;
}

Node *stmt() {
  Node *lhs = assign();
  if ( !consume(';')) {
    error("';'ではないトークンです", pos);
  }
  return lhs;
}

Vector *program() {
  Vector *code = new_vector();
  while (!consume(TK_EOF)) {
    vec_push(code, stmt());
  }
  vec_push(code, NULL);
  return code;
}

Vector *tokenize(char *p) {
  tokens = new_vector();
  keywords = new_map();
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

    if (isdigit(*p)) {
      Token *t = add_token(tokens, TK_NUM, p);
      t->val = strtol(p, &p, 10);
      continue;
    }

    if (isalpha(*p) || *p == '_') {
      int len = 1;
      while (isalpha(p[len]) || isdigit(p[len] || p[len] == '_') ) {
        len++;
      }
      char *name = strndup(p, len);
      int ty = (intptr_t)map_get(keywords, name);
      if ( !ty ) {
        ty = TK_IDENT;
      }
      Token *t = add_token(tokens, TK_IDENT, p);
      t->name = name;
      p+=len;
      continue;
    }

    fprintf(stderr, "トークナイズできません %s", p);
    exit(1);
  }

  add_token(tokens, TK_EOF, p);
  return tokens;
}

