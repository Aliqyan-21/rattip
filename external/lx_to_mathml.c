#include "lx_to_mathml.h"
#include <stdio.h>

void lx4c_accept(lx4c_node *node, const lx4c_visitor *v, void *ctx) {
  if (!node || !v) { return; }

  switch (node->type) {
    case LX4C_NODE_IDENT: v->visit_ident(node, ctx, v); break;
    case LX4C_NODE_NUMBER: v->visit_number(node, ctx, v); break;
    case LX4C_NODE_OP: v->visit_op(node, ctx, v); break;
    case LX4C_NODE_TEXT: v->visit_text(node, ctx, v); break;
    case LX4C_NODE_ROW: v->visit_row(node, ctx, v); break;
    case LX4C_NODE_FRAC: v->visit_frac(node, ctx, v); break;
    case LX4C_NODE_SQRT: v->visit_sqrt(node, ctx, v); break;
    case LX4C_NODE_ROOT: v->visit_root(node, ctx, v); break;
    case LX4C_NODE_SUP: v->visit_sup(node, ctx, v); break;
    case LX4C_NODE_SUB: v->visit_sub(node, ctx, v); break;
    case LX4C_NODE_SUBSUP: v->visit_subsup(node, ctx, v); break;
    case LX4C_NODE_OVER: v->visit_over(node, ctx, v); break;
    case LX4C_NODE_UNDER: v->visit_under(node, ctx, v); break;
    default: v->visit_unknown(node, ctx, v); break;
  }
}

void append_buf(lx4c_buffer *b, const char *s) {
  size_t slen = strlen(s);
  if (b->len + slen + 1 > b->cap) {
    b->cap = (b->len + slen + 1) * 2;
    b->buf = realloc(b->buf, b->cap);
  }
  memcpy(b->buf + b->len, s, slen);
  b->len += slen;
  b->buf[b->len] = '\0';
}

/* ---------- */
/* LEAF NODES */
/* ---------- */

void visit_ident(lx4c_node *n, void *ctx, const lx4c_visitor *v) {
  lx4c_buffer *b = ctx;
  if (n->symbol) {
    append_buf(b, "<mi>");
    append_buf(b, n->symbol);
    append_buf(b, "</mi>");
  } else {
    char tmp[64];
    snprintf(tmp, sizeof(tmp), "<mi>%.*s</mi>", (int)n->value_len, n->value);
    append_buf(b, tmp);
  }
}

void visit_number(lx4c_node *n, void *ctx, const lx4c_visitor *v) {
  lx4c_buffer *b = ctx;
  char         tmp[64];
  snprintf(tmp, sizeof(tmp), "<mn>%.*s</mn>", (int)n->value_len, n->value);
  append_buf(b, tmp);
}

static void visit_op(lx4c_node *n, void *ctx, const lx4c_visitor *v) {
  lx4c_buffer *b = ctx;
  append_buf(b, "<mo>");
  if (n->symbol) {
    append_buf(b, n->symbol);
  } else {
    char tmp[64];
    snprintf(tmp, sizeof(tmp), "%.*s", (int)n->value_len, n->value);
    append_buf(b, tmp);
  }
  append_buf(b, "</mo>");
}

static void visit_text(lx4c_node *n, void *ctx, const lx4c_visitor *v) {
  lx4c_buffer *b = ctx;
  append_buf(b, "<mtext>");
  lx4c_accept(n->children[0], v, ctx);
  append_buf(b, "</mtext>");
}

/* ---------------- */
/* STRUCTURAL NODES */
/* ---------------- */
static void visit_row(lx4c_node *n, void *ctx, const lx4c_visitor *v) {
  lx4c_buffer *b = ctx;
  append_buf(b, "<mrow>");
  for (int i = 0; i < n->child_count; i++) {
    lx4c_accept(n->children[i], v, ctx);
  }
  append_buf(b, "</mrow>");
}

void visit_frac(lx4c_node *n, void *ctx, const lx4c_visitor *v) {
  lx4c_buffer *b = ctx;
  append_buf(b, "<mfrac>");
  for (int i = 0; i < n->child_count; ++i) {
    lx4c_accept(n->children[i], v, ctx);
  }
  append_buf(b, "</mfrac>");
}

void visit_sqrt(lx4c_node *n, void *ctx, const lx4c_visitor *v) {
  lx4c_buffer *b = ctx;
  append_buf(b, "<msqrt>");
  for (int i = 0; i < n->child_count; ++i) {
    lx4c_accept(n->children[i], v, ctx);
  }
  append_buf(b, "</msqrt>");
}

void visit_root(lx4c_node *n, void *ctx, const lx4c_visitor *v) {
  lx4c_buffer *b = ctx;
  append_buf(b, "<mroot>");
  for (int i = 0; i < n->child_count; ++i) {
    lx4c_accept(n->children[i], v, ctx);
  }
  append_buf(b, "</mroot>");
}

void visit_sup(lx4c_node *n, void *ctx, const lx4c_visitor *v) {
  lx4c_buffer *b = ctx;
  append_buf(b, "<msup>");
  for (int i = 0; i < n->child_count; ++i) {
    lx4c_accept(n->children[i], v, ctx);
  }
  append_buf(b, "</msup>");
}

void visit_sub(lx4c_node *n, void *ctx, const lx4c_visitor *v) {
  lx4c_buffer *b = ctx;
  append_buf(b, "<msub>");
  for (int i = 0; i < n->child_count; ++i) {
    lx4c_accept(n->children[i], v, ctx);
  }
  append_buf(b, "</msub>");
}

void visit_subsup(lx4c_node *n, void *ctx, const lx4c_visitor *v) {
  lx4c_buffer *b = ctx;
  append_buf(b, "<msubsup>");
  for (int i = 0; i < n->child_count; ++i) {
    lx4c_accept(n->children[i], v, ctx);
  }
  append_buf(b, "</msubsup>");
}

void visit_over(lx4c_node *n, void *ctx, const lx4c_visitor *v) {
  lx4c_buffer *b = ctx;
  append_buf(b, "<mover>");
  for (int i = 0; i < n->child_count; ++i) {
    lx4c_accept(n->children[i], v, ctx);
  }
  append_buf(b, "<mo>");
  append_buf(b, n->symbol ? n->symbol : "‾");
  append_buf(b, "</mo>");
  append_buf(b, "</mover>");
}

void visit_under(lx4c_node *n, void *ctx, const lx4c_visitor *v) {
  lx4c_buffer *b = ctx;
  append_buf(b, "<munder>");
  for (int i = 0; i < n->child_count; ++i) {
    lx4c_accept(n->children[i], v, ctx);
  }
  append_buf(b, "<mo>&#x332;</mo>");
  append_buf(b, "</munder>");
}

void visit_unknown(lx4c_node *n, void *ctx, const lx4c_visitor *v) {
  lx4c_buffer *b = ctx;
  char         tmp[64];
  snprintf(tmp, sizeof(tmp), "<mi>%.*s</mi>", (int)n->value_len, n->value);
  append_buf(b, tmp);
}

char *lx4c_to_mathml(lx4c_node *root, bool display) {
  static const lx4c_visitor mathml_visitor = {
    .visit_ident   = visit_ident,
    .visit_number  = visit_number,
    .visit_op      = visit_op,
    .visit_text    = visit_text,
    .visit_row     = visit_row,
    .visit_frac    = visit_frac,
    .visit_sqrt    = visit_sqrt,
    .visit_root    = visit_root,
    .visit_sup     = visit_sup,
    .visit_sub     = visit_sub,
    .visit_subsup  = visit_subsup,
    .visit_over    = visit_over,
    .visit_under   = visit_under,
    .visit_unknown = visit_unknown,
  };

  lx4c_buffer b = {0};
  // append_buf(&b, display ? "<math display=\"block\">" : "<math>");
  lx4c_accept(root, &mathml_visitor, &b);
  // append_buf(&b, "</math>");
  return b.buf;
}
