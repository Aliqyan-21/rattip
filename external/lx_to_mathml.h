#ifndef LX_TO_MATHML_H
#define LX_TO_MATHML_H

#include <stdbool.h>
#include <string.h>
#include "lx4c.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct lx4c_visitor lx4c_visitor;

struct lx4c_visitor {
  void (*visit_ident)(lx4c_node *node, void *ctx, const lx4c_visitor *v);
  void (*visit_number)(lx4c_node *node, void *ctx, const lx4c_visitor *v);
  void (*visit_op)(lx4c_node *node, void *ctx, const lx4c_visitor *v);
  void (*visit_text)(lx4c_node *node, void *ctx, const lx4c_visitor *v);
  void (*visit_row)(lx4c_node *node, void *ctx, const lx4c_visitor *v);
  void (*visit_frac)(lx4c_node *node, void *ctx, const lx4c_visitor *v);
  void (*visit_sqrt)(lx4c_node *node, void *ctx, const lx4c_visitor *v);
  void (*visit_root)(lx4c_node *node, void *ctx, const lx4c_visitor *v);
  void (*visit_sup)(lx4c_node *node, void *ctx, const lx4c_visitor *v);
  void (*visit_sub)(lx4c_node *node, void *ctx, const lx4c_visitor *v);
  void (*visit_subsup)(lx4c_node *node, void *ctx, const lx4c_visitor *v);
  void (*visit_over)(lx4c_node *node, void *ctx, const lx4c_visitor *v);
  void (*visit_under)(lx4c_node *node, void *ctx, const lx4c_visitor *v);
  void (*visit_unknown)(lx4c_node *node, void *ctx, const lx4c_visitor *v);
};

typedef struct {
  char  *buf;
  size_t len;
  size_t cap;
} lx4c_buffer; /* buffer for visitors: they must too dump somehwere afterall */

void append_buf(lx4c_buffer *b, const char *s);

void lx4c_accept(lx4c_node *node, const lx4c_visitor *v, void *ctx);

char *lx4c_to_mathml(lx4c_node *root, bool display);

#ifdef __cplusplus
}
#endif
#endif  //! LX_TO_MATHML_H
