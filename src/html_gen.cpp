#include "html_gen.h"
#include "lx4c.h"
#include "lx_to_mathml.h"

/* ----------- */
/* CONSTRUCTOR */
/* ----------- */
HTMLGen::HTMLGen(const std::string &content, int flags, uint32_t abi_version)
  : content_(content) {
  parser_.abi_version = abi_version;
  parser_.flags       = flags;
}

/* ---------------- */
/* Public Functions */
/* ---------------- */
int HTMLGen::parse_markdown() {
  parser_.enter_block = HTMLGen::enter_block;
  parser_.leave_block = HTMLGen::leave_block;
  parser_.enter_span  = HTMLGen::enter_span;
  parser_.leave_span  = HTMLGen::leave_span;
  parser_.text        = HTMLGen::text;
  int res = md_parse(content_.c_str(), content_.size(), &parser_, this);
  return res;
}

/* ------------------ */
/* Callback Functions */
/* ------------------ */
int HTMLGen::enter_block(MD_BLOCKTYPE type, void *detail_ptr, void *gen_data) {
  auto *self = static_cast<HTMLGen *>(gen_data);
  return self->dispatch_enter_block(type, detail_ptr);
}
int HTMLGen::leave_block(MD_BLOCKTYPE type, void *detail_ptr, void *gen_data) {
  auto *self = static_cast<HTMLGen *>(gen_data);
  return self->dispatch_leave_block(type, detail_ptr);
}
int HTMLGen::enter_span(MD_SPANTYPE type, void *detail_ptr, void *gen_data) {
  auto *self = static_cast<HTMLGen *>(gen_data);
  return self->dispatch_enter_span(type, detail_ptr);
}
int HTMLGen::leave_span(MD_SPANTYPE type, void *detail_ptr, void *gen_data) {
  auto *self = static_cast<HTMLGen *>(gen_data);
  return self->dispatch_leave_span(type, detail_ptr);
}
int HTMLGen::text(MD_TEXTTYPE type, const MD_CHAR *text, MD_SIZE size,
                  void *gen_data) {
  auto *self = static_cast<HTMLGen *>(gen_data);
  return self->dispatch_text(type, text, size);
}

/* -------------------- */
/* Dispatcher Functions */
/* -------------------- */
int HTMLGen::dispatch_enter_block(MD_BLOCKTYPE type, void *detail_ptr) {
  switch (type) {
    case MD_BLOCK_DOC: handle_doc_enter(); break;
    case MD_BLOCK_H:
      handle_h_enter(static_cast<MD_BLOCK_H_DETAIL *>(detail_ptr));
      break;
    case MD_BLOCK_P: handle_p_enter(); break;
    case MD_BLOCK_HTML: handle_html_enter(); break;
    case MD_BLOCK_UL:
      handle_ul_enter(static_cast<MD_BLOCK_UL_DETAIL *>(detail_ptr));
      break;
    case MD_BLOCK_OL:
      handle_ol_enter(static_cast<MD_BLOCK_OL_DETAIL *>(detail_ptr));
      break;
    case MD_BLOCK_LI:
      handle_li_enter(static_cast<MD_BLOCK_LI_DETAIL *>(detail_ptr));
      break;
    case MD_BLOCK_HR: handle_hr_enter(); break;
    case MD_BLOCK_CODE:
      handle_code_enter(static_cast<MD_BLOCK_CODE_DETAIL *>(detail_ptr));
      break;
    case MD_BLOCK_QUOTE: handle_quote_enter(); break;
    case MD_BLOCK_TABLE:
      handle_table_enter(static_cast<MD_BLOCK_TABLE_DETAIL *>(detail_ptr));
      break;
    case MD_BLOCK_THEAD: handle_thead_enter(); break;
    case MD_BLOCK_TBODY: handle_tbody_enter(); break;
    case MD_BLOCK_TR: handle_tr_enter(); break;
    case MD_BLOCK_TH:
      handle_th_enter(static_cast<MD_BLOCK_TD_DETAIL *>(detail_ptr));
      break;
    case MD_BLOCK_TD:
      handle_td_enter(static_cast<MD_BLOCK_TD_DETAIL *>(detail_ptr));
      break;
    default: break;
  }
  return 0;
}
int HTMLGen::dispatch_leave_block(MD_BLOCKTYPE type, void *detail_ptr) {
  switch (type) {
    case MD_BLOCK_DOC: handle_doc_leave(); break;
    case MD_BLOCK_H:
      handle_h_leave(static_cast<MD_BLOCK_H_DETAIL *>(detail_ptr));
      break;
    case MD_BLOCK_P: handle_p_leave(); break;
    case MD_BLOCK_HTML: handle_html_leave(); break;
    case MD_BLOCK_UL: handle_ul_leave(); break;
    case MD_BLOCK_OL: handle_ol_leave(); break;
    case MD_BLOCK_LI: handle_li_leave(); break;
    case MD_BLOCK_HR: handle_hr_leave(); break;
    case MD_BLOCK_CODE: handle_code_leave(); break;
    case MD_BLOCK_QUOTE: handle_quote_leave(); break;
    case MD_BLOCK_TABLE: handle_table_leave(); break;
    case MD_BLOCK_THEAD: handle_thead_leave(); break;
    case MD_BLOCK_TBODY: handle_tbody_leave(); break;
    case MD_BLOCK_TR: handle_tr_leave(); break;
    case MD_BLOCK_TH: handle_th_leave(); break;
    case MD_BLOCK_TD: handle_td_leave(); break;
    default: break;
  }
  return 0;
}
int HTMLGen::dispatch_enter_span(MD_SPANTYPE type, void *detail_ptr) {
  switch (type) {
    case MD_SPAN_EM: handle_em_enter(); break;
    case MD_SPAN_STRONG: handle_strong_enter(); break;
    case MD_SPAN_CODE: handle_backtick_enter(); break;
    case MD_SPAN_A:
      handle_link_enter(static_cast<MD_SPAN_A_DETAIL *>(detail_ptr));
      break;
    case MD_SPAN_IMG:
      handle_img_enter(static_cast<MD_SPAN_IMG_DETAIL *>(detail_ptr));
      break;
    case MD_SPAN_DEL: handle_del_enter(); break;
    case MD_SPAN_U: handle_u_enter(); break;
    case MD_SPAN_SUPERSCRIPT: handle_superscript_enter(); break;
    case MD_SPAN_SUBSCRIPT: handle_subscript_enter(); break;
    case MD_SPAN_SPOILER: handle_spoiler_enter(); break;
    case MD_SPAN_LATEXMATH: handle_latex_enter(); break;
    case MD_SPAN_LATEXMATH_DISPLAY: handle_latex_display_enter(); break;
    default: break;
  }
  return 0;
}
int HTMLGen::dispatch_leave_span(MD_SPANTYPE type, void *detail_ptr) {
  switch (type) {
    case MD_SPAN_EM: handle_em_leave(); break;
    case MD_SPAN_STRONG: handle_strong_leave(); break;
    case MD_SPAN_CODE: handle_backtick_leave(); break;
    case MD_SPAN_A: handle_link_leave(); break;
    case MD_SPAN_IMG: handle_img_leave(); break;
    case MD_SPAN_DEL: handle_del_leave(); break;
    case MD_SPAN_U: handle_u_leave(); break;
    case MD_SPAN_SUPERSCRIPT: handle_superscript_leave(); break;
    case MD_SPAN_SUBSCRIPT: handle_subscript_leave(); break;
    case MD_SPAN_SPOILER: handle_spoiler_leave(); break;
    case MD_SPAN_LATEXMATH: handle_latex_leave(); break;
    case MD_SPAN_LATEXMATH_DISPLAY: handle_latex_display_leave(); break;
    default: break;
  }
  return 0;
}
int HTMLGen::dispatch_text(MD_TEXTTYPE type, const MD_CHAR *text,
                           MD_SIZE size) {
  switch (type) {
    case MD_TEXT_NORMAL: handle_normal_text(text, size); break;
    case MD_TEXT_BR: handle_br_text(); break;
    case MD_TEXT_HTML: handle_html_text(text, size); break;
    case MD_TEXT_CODE: handle_code_text(text, size); break;
    case MD_TEXT_SOFTBR: handle_softbr_text(); break;
    case MD_TEXT_LATEXMATH: handle_latex_text(text, size); break;
    default: break;
  }
  return 0;
}

/* ------------------------ */
/* handlers for enter_block */
/* ------------------------ */
void HTMLGen::handle_doc_enter() { /* html_buf_ += "<body>\n"; */ }
void HTMLGen::handle_h_enter(MD_BLOCK_H_DETAIL *d) {
  std::string l = std::to_string(d->level);
  html_buf_ += "<h" + l + " class=\"rattip-h" + l + "\"> ";
}
void HTMLGen::handle_p_enter() { html_buf_ += "<p class=\"rattip-p\"> "; }
void HTMLGen::handle_html_enter() { html_buf_ += ""; }
void HTMLGen::handle_ul_enter(MD_BLOCK_UL_DETAIL *d) {
  is_tight_ = d->is_tight;
  html_buf_ += "<ul class=\"rattip-ul\">\n";
}
void HTMLGen::handle_ol_enter(MD_BLOCK_OL_DETAIL *d) {
  this->is_tight_ = d->is_tight;
  html_buf_ += "<ol class=\"rattip-ol\">\n";
}
void HTMLGen::handle_li_enter(MD_BLOCK_LI_DETAIL *d) {
  html_buf_ += "<li class=\"rattip-li\"> ";
  if (d->is_task) {
    html_buf_ += "<input type=\"checkbox\" disabled";
    if (d->task_mark != ' ') {
      html_buf_ += " checked> ";
    } else {
      html_buf_ += "> ";
    }
  }
}
void HTMLGen::handle_hr_enter() { html_buf_ += "<hr class=\"rattip-hr\">"; }
void HTMLGen::handle_code_enter(MD_BLOCK_CODE_DETAIL *d) {
  html_buf_ += "<pre class=\"rattip-pre\"><code class=\"rattip-code lang-" +
               std::string(d->lang.text, d->lang.size) + "\">";
}
void HTMLGen::handle_quote_enter() {
  html_buf_ += "<blockquote class=\"rattip-blockquote\"> ";
}
void HTMLGen::handle_table_enter(MD_BLOCK_TABLE_DETAIL *d) {
  html_buf_ += "<table class=\"rattip-table\">\n";
}
void HTMLGen::handle_thead_enter() {
  html_buf_ += "<thead class=\"rattip-thead\">";
}
void HTMLGen::handle_tbody_enter() {
  html_buf_ += "<tbody class=\"rattip-tbody\">";
}
void HTMLGen::handle_tr_enter() { html_buf_ += "<tr class=\"rattip-tr\">"; }
void HTMLGen::handle_th_enter(MD_BLOCK_TD_DETAIL *d) {
  html_buf_ += "<th class=\"rattip-th\">";
}
void HTMLGen::handle_td_enter(MD_BLOCK_TD_DETAIL *d) {
  html_buf_ += "<td class=\"rattip-td\">";
}

/* ------------------------ */
/* handlers for leave_block */
/* ------------------------ */
void HTMLGen::handle_doc_leave() { /* html_buf_ += "</body>"; */ }
void HTMLGen::handle_h_leave(MD_BLOCK_H_DETAIL *d) {
  html_buf_ += " </h" + std::to_string(d->level) + ">\n";
}
void HTMLGen::handle_p_leave() {
  if (!is_tight_) {
    html_buf_ += " </p>";
  } else {
    html_buf_ += " </p>\n";
  }
}
void HTMLGen::handle_html_leave() { html_buf_ += ""; }
void HTMLGen::handle_ul_leave() { html_buf_ += "</ul>\n"; }
void HTMLGen::handle_ol_leave() { html_buf_ += "</ol>\n"; }
void HTMLGen::handle_li_leave() { html_buf_ += " </li>\n"; }
void HTMLGen::handle_hr_leave() { html_buf_ += "\n"; }
void HTMLGen::handle_code_leave() { html_buf_ += "</code></pre>\n"; }
void HTMLGen::handle_quote_leave() { html_buf_ += "</blockquote>\n"; }
void HTMLGen::handle_table_leave() { html_buf_ += "</table>\n"; }
void HTMLGen::handle_thead_leave() { html_buf_ += "</thead>\n"; }
void HTMLGen::handle_tbody_leave() { html_buf_ += "</tbody>\n"; }
void HTMLGen::handle_tr_leave() { html_buf_ += "</tr>\n"; }
void HTMLGen::handle_th_leave() { html_buf_ += "</th>\n"; }
void HTMLGen::handle_td_leave() { html_buf_ += "</td>\n"; }

/* ----------------------- */
/* handlers for enter_span */
/* ----------------------- */
void HTMLGen::handle_em_enter() { html_buf_ += " <i class=\"rattip-i\">"; }
void HTMLGen::handle_strong_enter() { html_buf_ += " <b class=\"rattip-b\">"; }
void HTMLGen::handle_backtick_enter() {
  html_buf_ += " <code class=\"rattip-code\">";
}
void HTMLGen::handle_link_enter(MD_SPAN_A_DETAIL *d) {
  html_buf_ += "<a href=\"" + std::string(d->href.text, d->href.size) +
               "\" class=\"rattip-a\">" +
               std::string(d->title.text, d->title.size);
}
void HTMLGen::handle_img_enter(MD_SPAN_IMG_DETAIL *d) {
  html_buf_ += "<img src=\"" + std::string(d->src.text, d->src.size) +
               "\" class=\"rattip-img\" alt=\"";
}
void HTMLGen::handle_del_enter() { html_buf_ += " <del class=\"rattip-del\">"; }
void HTMLGen::handle_u_enter() { html_buf_ += " <u class=\"rattip-u\">"; }
void HTMLGen::handle_superscript_enter() {
  html_buf_ += "<sup class=\"rattip-sup\">";
}
void HTMLGen::handle_subscript_enter() {
  html_buf_ += "<sub class=\"rattip-sub\">";
}
void HTMLGen::handle_spoiler_enter() {
  html_buf_ +=
    "<span class=\"rattip-spoiler\" "
    "style=\"filter:blur(4px);cursor:pointer;"
    "transition:filter 0.3s ease;user-select:none;\" "
    "onclick=\"this.style.filter=this.style.filter===''?'blur(4px)':''\">";
}
void HTMLGen::handle_latex_enter() {
  html_buf_ += "<math class=\"rattip-lx\">";
}
void HTMLGen::handle_latex_display_enter() {
  html_buf_ += "<math display=\"block\" class=\"rattip-lx-display\">";
}

/* ----------------------- */
/* handlers for leave_span */
/* ----------------------- */
void HTMLGen::handle_em_leave() { html_buf_ += "</i> "; }
void HTMLGen::handle_strong_leave() { html_buf_ += "</b> "; }
void HTMLGen::handle_backtick_leave() { html_buf_ += "</code> "; }
void HTMLGen::handle_link_leave() { html_buf_ += "</a> "; }
void HTMLGen::handle_img_leave() { html_buf_ += "\" /> "; }
void HTMLGen::handle_del_leave() { html_buf_ += "</del> "; }
void HTMLGen::handle_u_leave() { html_buf_ += "</u> "; }
void HTMLGen::handle_superscript_leave() { html_buf_ += "</sup> "; }
void HTMLGen::handle_subscript_leave() { html_buf_ += "</sub> "; }
void HTMLGen::handle_spoiler_leave() { html_buf_ += "</span> "; }
void HTMLGen::handle_latex_leave() { html_buf_ += "</math>"; }
void HTMLGen::handle_latex_display_leave() { html_buf_ += "</math>"; }

/* ----------------- */
/* handlers for text */
/* ----------------- */
void HTMLGen::handle_normal_text(const MD_CHAR *text, MD_SIZE size) {
  html_buf_ += std::string(text, size);
}
void HTMLGen::handle_br_text() { html_buf_ += " <br/>\n"; }
void HTMLGen::handle_html_text(const MD_CHAR *text, MD_SIZE size) {
  html_buf_ += std::string(text, size);
}
void HTMLGen::handle_code_text(const MD_CHAR *text, MD_SIZE size) {
  html_buf_ += std::string(text, size);
}
void HTMLGen::handle_softbr_text() { html_buf_ += " "; }
void HTMLGen::handle_latex_text(const MD_CHAR *text, MD_SIZE size) {
  lx4c_node *root = lx4c_parse(text, size);
  html_buf_ += lx4c_to_mathml(root, 0);
  lx4c_free(root);
}
