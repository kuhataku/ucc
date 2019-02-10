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
  char *name;
} Node;

typedef struct {
  int ty;
  int val;
  char *name;
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

Vector *program();
Vector *tokenize(char *p);
void gen_ir(Vector *code);
void error(char *message, int i);
Vector *new_vector();
void vec_push(Vector *vec, void *elem);
Map *new_map();
void map_put(Map *map, char *key, void *val);
void *map_get(Map *map, char *key);
void runtest();
