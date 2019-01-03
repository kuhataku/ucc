enum {
  ND_NUM = 256,
  ND_IDENT,
  ND_EQUAL,
  ND_NEQUAL,
};

typedef struct Node {
  int ty;
  struct Node *lhs;
  struct Node *rhs;
  int val;
  char name;
} Node;

typedef struct {
  int ty;
  int val;
  char *input;
} Token;

extern Token tokens[100];
extern Node *code[100];

void program();
void tokenize(char *p);
void gen(Node *node);
void error(char *message, int i);
