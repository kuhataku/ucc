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

typedef struct {
  void **data;
  int capacity;
  int len;
} Vector;

typedef struct {
  Vector *keys;
  Vector *vals;
} Map;

extern Token tokens[100];
extern Node *code[100];

void program();
void tokenize(char *p);
void gen(Node *node);
void error(char *message, int i);
Vector *new_vector();
void vec_push(Vector *vec, void *elem);
Map *new_map();
void map_put(Map *map, char *key, void *val);
void *map_get(Map *map, char *key);
void runtest();
