#include "lx4c.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum lx4c_token_type {
  TOK_IDENT,  /* single letter only: x,y, E */
  TOK_NUMBER, /* digits */
  TOK_CMD,    /* backlash command: \frac, \sqrt, \beta, etc. */
  TOK_LBRACE,
  TOK_RBRACE,
  TOK_LBRACKET,
  TOK_RBRACKET,
  TOK_LPAREN,
  TOK_RPAREN,
  TOK_SUP,   /* ^ */
  TOK_SUB,   /* _ */
  TOK_OTHER, /* any unrecognized token */
  TOK_EOF,
} lx4c_token_type;

typedef struct {
  lx4c_token_type type;
  const char     *start;
  size_t          len;
} lx4c_token;

/* --------------- */
/* lexer/tokenizer */
/* --------------- */

typedef struct {
  const char *src; /* the input */
  size_t      len; /* total length of input */
  size_t      pos; /* current position */
} lx4c_lexer;

/* utility functions */
static inline bool is_end(lx4c_lexer *lex) { return lex->pos >= lex->len; }
static inline void skip_whitespace(lx4c_lexer *lex) {
  while (!is_end(lex) && isspace(lex->src[lex->pos])) { lex->pos++; }
}

static void lexer_init(lx4c_lexer *lex, const char *src, size_t len) {
  lex->src = src;
  lex->len = len;
  lex->pos = 0;
}

/* readers */
inline static void read_cmd(lx4c_lexer *lex) {
  while (lex->pos < lex->len && isalpha((unsigned char)lex->src[lex->pos])) {
    lex->pos++;
  }
}

inline static void read_number(lx4c_lexer *lex) {
  while (lex->pos < lex->len && (isdigit((unsigned char)lex->src[lex->pos]) ||
                                 lex->src[lex->pos] == '.')) {
    lex->pos++;
  }
}

static lx4c_token tokenize_next(lx4c_lexer *lex) {
  lx4c_token tok;
  skip_whitespace(lex);

  /* EOF */
  if (is_end(lex)) {
    tok.type  = TOK_EOF;
    tok.start = lex->src + lex->pos;
    tok.len   = 0;
    return tok;
  }

  char c    = lex->src[lex->pos];
  tok.start = lex->src + lex->pos;

  /* backlash command: \frac, \beta, ... */
  if (c == '\\') {
    lex->pos++;
    size_t cmd_start = lex->pos;
    read_cmd(lex);
    tok.type  = TOK_CMD;
    tok.start = lex->src + cmd_start;
    tok.len   = lex->pos - cmd_start;
    return tok;
  }

  /* number: 42, 3.14 */
  if (isdigit((unsigned char)c)) {
    read_number(lex);
    tok.type = TOK_NUMBER;
    tok.len  = lex->pos - (tok.start - lex->src);
    return tok;
  }

  /* ident */
  if (isalpha((unsigned char)c)) {
    lex->pos++;
    tok.type = TOK_IDENT;
    tok.len  = 1;
    return tok;
  }

  /* single char tokens */
  lex->pos++;
  tok.len = 1;
  switch (c) {
    case '{': tok.type = TOK_LBRACE; break;
    case '}': tok.type = TOK_RBRACE; break;
    case '[': tok.type = TOK_LBRACKET; break;
    case ']': tok.type = TOK_RBRACKET; break;
    case '(': tok.type = TOK_LPAREN; break;
    case ')': tok.type = TOK_RPAREN; break;
    case '^': tok.type = TOK_SUP; break;
    case '_': tok.type = TOK_SUB; break;
    default: tok.type = TOK_OTHER; break;
  }

  return tok;
}

/* ----------- */
/* lx4c Parser */
/* ----------- */

cmd_entry CMD_TABLE[] = {
  /* the greek letters */
  {"alpha",      CMD_IDENT, "α"      },
  {"beta",       CMD_IDENT, "β"      },
  {"gamma",      CMD_IDENT, "γ"      },
  {"delta",      CMD_IDENT, "δ"      },
  {"epsilon",    CMD_IDENT, "ε"      },
  {"theta",      CMD_IDENT, "θ"      },
  {"lambda",     CMD_IDENT, "λ"      },
  {"mu",         CMD_IDENT, "μ"      },
  {"pi",         CMD_IDENT, "π"      },
  {"sigma",      CMD_IDENT, "σ"      },
  {"phi",        CMD_IDENT, "φ"      },
  {"omega",      CMD_IDENT, "ω"      },
  {"Gamma",      CMD_IDENT, "Γ"      },
  {"Delta",      CMD_IDENT, "Δ"      },
  {"Sigma",      CMD_IDENT, "Σ"      },
  {"Omega",      CMD_IDENT, "Ω"      },
  /* the named functions */
  {"sin",        CMD_IDENT, "sin"    },
  {"cos",        CMD_IDENT, "cos"    },
  {"tan",        CMD_IDENT, "tan"    },
  {"log",        CMD_IDENT, "log"    },
  {"ln",         CMD_IDENT, "ln"     },
  {"exp",        CMD_IDENT, "exp"    },
  {"max",        CMD_IDENT, "max"    },
  {"min",        CMD_IDENT, "min"    },
  {"lim",        CMD_IDENT, "lim"    },
  /* the operators */
  {"leq",        CMD_OP,    "≤"      },
  {"geq",        CMD_OP,    "≥"      },
  {"neq",        CMD_OP,    "≠"      },
  {"times",      CMD_OP,    "×"      },
  {"cdot",       CMD_OP,    "·"      },
  {"div",        CMD_OP,    "÷"      },
  {"pm",         CMD_OP,    "±"      },
  {"infty",      CMD_OP,    "∞"      },
  {"partial",    CMD_OP,    "∂"      },
  {"nabla",      CMD_OP,    "∇"      },
  {"sum",        CMD_OP,    "∑"      },
  {"prod",       CMD_OP,    "∏"      },
  {"int",        CMD_OP,    "∫"      },
  {"approx",     CMD_OP,    "≈"      },
  {"in",         CMD_OP,    "∈"      },
  {"subset",     CMD_OP,    "⊂"      },
  {"cup",        CMD_OP,    "∪"      },
  {"cap",        CMD_OP,    "∩"      },
  {"to",         CMD_OP,    "→"      },
  {"rightarrow", CMD_OP,    "→"      },
  {"leftarrow",  CMD_OP,    "←"      },
  {"forall",     CMD_OP,    "∀"      },
  {"exists",     CMD_OP,    "∃"      },
  {"neg",        CMD_OP,    "¬"      },
  {"land",       CMD_OP,    "∧"      },
  {"lor",        CMD_OP,    "∨"      },
  {"equiv",      CMD_OP,    "≡"      },
  {"sim",        CMD_OP,    "∼"      },
  /* the structural */
  {"frac",       CMD_FRAC,  NULL     },
  {"sqrt",       CMD_SQRT,  NULL     },
  {"overline",   CMD_OVER,  "‾"      },
  {"vec",        CMD_VEC,   "→"      },
  {"underline",  CMD_UNDER, "&#x332;"},
  {"text",       CMD_TEXT,  NULL     },
  /* the accents */
  {"hat",        CMD_OVER,  "^"      },
  {"bar",        CMD_OVER,  "‾"      },
  {"tilde",      CMD_OVER,  "~"      },
  {"dot",        CMD_OVER,  "˙"      },
  {"ddot",       CMD_OVER,  "¨"      },
  /* the ellipsis */
  {"cdots",      CMD_OP,    "⋯"      },
  {"ldots",      CMD_OP,    "…"      },
  {"vdots",      CMD_OP,    "⋮"      },
  /* the spacing */
  {"quad",       CMD_OP,    " "      },
  /* the binomial */
  {"binom",      CMD_FRAC,  NULL     },
};

typedef struct {
  lx4c_lexer lex;
  lx4c_token curr;
  lx4c_token peek;
} lx4c_parser;

static void parser_init(lx4c_parser *p, const char *src, size_t len) {
  lexer_init(&p->lex, src, len);
  p->curr = tokenize_next(&p->lex); /* first */
  p->peek = tokenize_next(&p->lex); /* second */
}

static lx4c_token parser_advance(lx4c_parser *p) {
  lx4c_token tok = p->curr;
  p->curr        = p->peek;
  p->peek        = tokenize_next(&p->lex); /* second */
  return tok;
}

static const int CMD_T_SIZE = (sizeof(CMD_TABLE) / sizeof(CMD_TABLE[0]));

static const cmd_entry *cmd_lookup(const char *name, size_t len) {
  for (size_t i = 0; i < CMD_T_SIZE; ++i) {
    if (strlen(CMD_TABLE[i].name) == len &&
        strncmp(CMD_TABLE[i].name, name, len) == 0) {
      return &CMD_TABLE[i];
    }
  }
  return NULL;
}

static lx4c_node *parse_row(lx4c_parser *p, lx4c_token_type stop);
static lx4c_node *parse_atom(lx4c_parser *p);

static lx4c_node *parse_group(lx4c_parser *p) {
  if (p->curr.type == TOK_LBRACE) {
    /* expect { -> consume */
    parser_advance(p);
    lx4c_node *row = parse_row(p, TOK_RBRACE);
    if (p->curr.type == TOK_RBRACE) {
      /* parse after '{' till '}' */
      parser_advance(p);  // consume '}'
    }
    return row;
  }
  return parse_atom(p);
}

static lx4c_node *node_alloc(lx4c_node_type type) {
  lx4c_node *n = calloc(1, sizeof(lx4c_node));
  if (n) { n->type = type; }
  return n;
}

static lx4c_node *parse_atom(lx4c_parser *p) {
  lx4c_token tok = parser_advance(p);
  lx4c_node *n   = NULL;

  switch (tok.type) {
    case TOK_IDENT:
      n = node_alloc(LX4C_NODE_IDENT);
      if (!n) { return NULL; }
      n->value     = tok.start;
      n->value_len = tok.len;
      n->symbol    = NULL;
      return n;
    case TOK_NUMBER:
      n = node_alloc(LX4C_NODE_NUMBER);
      if (!n) { return NULL; }
      n->value     = tok.start;
      n->value_len = tok.len;
      return n;
    case TOK_CMD: {
      const cmd_entry *e = cmd_lookup(tok.start, tok.len);
      if (!e) {
        n = node_alloc(LX4C_NODE_UNKNOWN);
        if (!n) { return NULL; }
        n->value     = tok.start;
        n->value_len = tok.len;
        return n;
      }
      switch (e->kind) {
        case CMD_IDENT:
          n = node_alloc(LX4C_NODE_IDENT);
          if (!n) { return NULL; }
          n->value     = tok.start;
          n->value_len = tok.len;
          n->symbol    = e->symbol;
          return n;
        case CMD_OP:
          n = node_alloc(LX4C_NODE_OP);
          if (!n) { return NULL; }
          n->value     = tok.start;
          n->value_len = tok.len;
          n->symbol    = e->symbol;
          return n;
        case CMD_FRAC: {
          /* \frac{numr}{denr} */
          lx4c_node *numr = parse_group(p);
          lx4c_node *denr = parse_group(p);
          n               = node_alloc(LX4C_NODE_FRAC);
          if (!n) { return NULL; }
          n->children = malloc(2 * sizeof(lx4c_node *));
          if (!n->children) { return NULL; }
          n->children[0] = numr;
          n->children[1] = denr;
          n->child_count = 2;
          return n;
        }
        case CMD_SQRT: {
          /* \sqrt[n]{x} or \sqrt{x} */
          if (p->curr.type == TOK_LBRACKET) {
            parser_advance(p);
            lx4c_node *root_n = parse_row(p, TOK_RBRACKET);
            if (p->curr.type == TOK_RBRACKET) { parser_advance(p); }
            lx4c_node *body = parse_group(p);
            n               = node_alloc(LX4C_NODE_ROOT);
            if (!n) { return NULL; }
            n->children = malloc(2 * sizeof(lx4c_node *));
            if (!n->children) { return NULL; }
            n->children[0] = body;
            n->children[1] = root_n;
            n->child_count = 2;
          } else {
            lx4c_node *body = parse_group(p);
            n               = node_alloc(LX4C_NODE_SQRT);
            if (!n) { return NULL; }
            n->children = malloc(1 * sizeof(lx4c_node *));
            if (!n->children) { return NULL; }
            n->children[0] = body;
            n->child_count = 1;
          }
          return n;
        }
        case CMD_OVER:
        case CMD_VEC: {
          lx4c_node *body = parse_group(p);
          n               = node_alloc(LX4C_NODE_OVER);
          if (!n) { return NULL; }
          n->symbol   = e->symbol;
          n->children = malloc(1 * sizeof(lx4c_node *));
          if (!n->children) { return NULL; }
          n->children[0] = body;
          n->child_count = 1;
          return n;
        }
        case CMD_UNDER: {
          lx4c_node *body = parse_group(p);
          n               = node_alloc(LX4C_NODE_UNDER);
          if (!n) { return NULL; }
          n->symbol   = e->symbol;
          n->children = malloc(1 * sizeof(lx4c_node *));
          if (!n->children) { return NULL; }
          n->children[0] = body;
          n->child_count = 1;
          return n;
        }
        case CMD_TEXT: {
          /* \text{hello} - parse raw text inside braces */
          lx4c_node *body = parse_group(p);
          n               = node_alloc(LX4C_NODE_TEXT);
          if (!n) { return NULL; }
          n->children = malloc(1 * sizeof(lx4c_node *));
          if (!n->children) { return NULL; }
          n->children[0] = body;
          n->child_count = 1;
          return n;
        }
        default: return NULL;
      }
    }
    case TOK_OTHER:
      n = node_alloc(LX4C_NODE_OP);
      if (!n) { return NULL; }
      n->value     = tok.start;
      n->value_len = tok.len;
      n->symbol    = NULL;
      return n;

    case TOK_LBRACE: return parse_group(p);
    case TOK_LPAREN:
    case TOK_RPAREN:
    case TOK_LBRACKET:
    case TOK_RBRACKET:
      n            = node_alloc(LX4C_NODE_OP);
      n->value     = tok.start;
      n->value_len = tok.len;
      return n;
    default: return NULL;
  }
}

static const int CHILDREN_INIT = 8;

static lx4c_node *parse_row(lx4c_parser *p, lx4c_token_type stop) {
  lx4c_node *row = node_alloc(LX4C_NODE_ROW);
  if (!row) { return NULL; }

  size_t capacity = CHILDREN_INIT;
  row->children   = malloc(capacity * sizeof(lx4c_node *));
  if (!row->children) { return NULL; }
  row->child_count = 0;

  while (p->curr.type != stop && p->curr.type != TOK_EOF) {
    lx4c_node *atom = parse_atom(p);
    if (!atom) { break; }

    int has_sup = (p->curr.type == TOK_SUP);
    int has_sub = (p->curr.type == TOK_SUB);

    lx4c_node *sub_node = NULL;
    lx4c_node *sup_node = NULL;

    if (!has_sup && !has_sub) {
      /* plain atom so just append */
      goto append;
    }

    if (has_sub) {
      parser_advance(p);
      sub_node = parse_group(p);
      has_sup  = (p->curr.type == TOK_SUP);
    }
    if (has_sup) {
      parser_advance(p);
      sup_node = parse_group(p);
      if (!has_sub) { has_sub = (p->curr.type == TOK_SUB); }
      if (has_sub && !sub_node) {
        parser_advance(p);
        sub_node = parse_group(p);
      }
    }
    if (sub_node && sup_node) {
      lx4c_node *ss   = node_alloc(LX4C_NODE_SUBSUP);
      ss->children    = malloc(3 * sizeof(lx4c_node *));
      ss->children[0] = atom;
      ss->children[1] = sub_node;
      ss->children[2] = sup_node;
      ss->child_count = 3;
      atom            = ss;
    } else if (sup_node) {
      lx4c_node *s   = node_alloc(LX4C_NODE_SUP);
      s->children    = malloc(2 * sizeof(lx4c_node *));
      s->children[0] = atom;
      s->children[1] = sup_node;
      s->child_count = 2;
      atom           = s;
    } else if (sub_node) {
      lx4c_node *s   = node_alloc(LX4C_NODE_SUB);
      s->children    = malloc(2 * sizeof(lx4c_node *));
      s->children[0] = atom;
      s->children[1] = sub_node;
      s->child_count = 2;
      atom           = s;
    }

append:
    if (row->child_count == (int)capacity) {
      capacity += 2;
      row->children = realloc(row->children, capacity * sizeof(lx4c_node *));
      if (!row->children) { return NULL; }
    }
    row->children[row->child_count++] = atom;
  }
  return row;
}

lx4c_node *lx4c_parse(const char *latex, size_t len) {
  lx4c_parser p;
  parser_init(&p, latex, len);
  return parse_row(&p, TOK_EOF);
}

void lx4c_free(lx4c_node *root) {
  if (!root) { return; }
  for (int i = 0; i < root->child_count; ++i) { lx4c_free(root->children[i]); }
  free(root->children);
  free(root);
}

const char *node_type_to_str[] = {
  "IDENT", "NUMBER", "OP",  "TEXT",   "ROW",  "FRAC",  "SQRT",
  "ROOT",  "SUP",    "SUB", "SUBSUP", "OVER", "UNDER", "UNKNOWN",
};

void print_node(lx4c_node *n, int depth) {
  if (!n) { return; }
  for (int i = 0; i < depth; ++i) { printf(" "); }

  printf("[%s] ", node_type_to_str[n->type]);
  if (n->value) { printf("%.*s", (int)n->value_len, n->value); }
  if (n->symbol) { printf("(%s)", n->symbol); }
  printf("\n");

  for (int i = 0; i < n->child_count; ++i) {
    print_node(n->children[i], depth + 1);
  }
}

void print_ast(lx4c_node *root) { print_node(root, 0); }
