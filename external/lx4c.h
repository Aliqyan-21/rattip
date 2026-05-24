#ifndef LX4C_H
#define LX4C_H

#include <stddef.h>
#include <stdlib.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef enum lx4c_node_type {
  /* leaf nodes */
  LX4C_NODE_IDENT,  /*  x,y,E */
  LX4C_NODE_NUMBER, /*  42, 3.14 */
  LX4C_NODE_OP,     /*  +, =, \leq, \times */
  LX4C_NODE_TEXT,   /* \text{hello} */

  /* strcture nodes */
  LX4C_NODE_ROW,    /* seq of nodes */
  LX4C_NODE_FRAC,   /* \frac{a}{b} */
  LX4C_NODE_SQRT,   /* \sqrt{x} */
  LX4C_NODE_ROOT,   /* \sqrt[n]{x} */
  LX4C_NODE_SUP,    /* x^{n} */
  LX4C_NODE_SUB,    /* x_{i} */
  LX4C_NODE_SUBSUP, /* x_{i}^{n} */
  LX4C_NODE_OVER,   /* \overline, \vec */
  LX4C_NODE_UNDER,  /* \underline */

  LX4C_NODE_UNKNOWN, /* unrecognized token */
} lx4c_node_type;

typedef struct lx4c_node lx4c_node;
typedef struct lx4c_node {
  lx4c_node_type type;

  /* leaf value is valid for IDENT, NUMBER, OP, TEXT, UNKNOWN */
  const char *value; /* original input string pointer */
  size_t      value_len;

  /* for OP/OVER/UNDER */
  const char *symbol; /* e.g. "\leq" -> "<=", NULL if unused */

  /* children -> for valid structural nodes */
  lx4c_node **children;
  int         child_count;
} lx4c_node;

typedef enum {
  CMD_IDENT,
  CMD_OP,
  CMD_FRAC,
  CMD_SQRT,
  CMD_OVER,
  CMD_UNDER,
  CMD_VEC,
  CMD_TEXT,
  CMD_UNKNOWN,
} cmd_kind;

typedef struct {
  const char *name;
  cmd_kind    kind;
  const char *symbol;
} cmd_entry;

/* the cmd table is cmd -> symbol table */
extern cmd_entry CMD_TABLE[];

/* Parse a latex math string, args:
 * - latex : input string, does not neet to be null-terminated
 * - len : length of input
 * - returns - root node, or NULL on allocation failure and others(?)
 */
lx4c_node *lx4c_parse(const char *latex, size_t len);

/* free the full AST returned by the above function */
void lx4c_free(lx4c_node *root);

/* print ast: for just utility */
void print_ast(lx4c_node *root);

#ifdef __cplusplus
}
#endif

#endif  //! LX4C_H
