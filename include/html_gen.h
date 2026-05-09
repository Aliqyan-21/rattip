#ifndef HTML_GEN_H
#define HTML_GEN_H

#include <cstdint>
#include <string>
#include "md4c.h"

class HTMLGen {
public:
  HTMLGen(const std::string &content, uint32_t abi_version = 0);

  int parse_markdown();

  static int enter_block(MD_BLOCKTYPE type, void *detail_ptr, void *gen_data);
  static int leave_block(MD_BLOCKTYPE type, void *detail_ptr, void *gen_data);
  static int enter_span(MD_SPANTYPE type, void *detail_ptr, void *gen_data);
  static int leave_span(MD_SPANTYPE type, void *detail_ptr, void *gen_data);
  static int text(MD_TEXTTYPE type, const MD_CHAR *text, MD_SIZE size,
                  void *gen_data);

  /* getter */
  std::string get_html() { return html_buf_; }

private:
  std::string content_;
  MD_PARSER   parser_ = {};
  std::string html_buf_;  // generated html

  bool is_tight_ = true;

  /* dispatchers */
  int dispatch_enter_block(MD_BLOCKTYPE type, void *detail_ptr);
  int dispatch_leave_block(MD_BLOCKTYPE type, void *detail_ptr);
  int dispatch_enter_span(MD_SPANTYPE type, void *detail_ptr);
  int dispatch_leave_span(MD_SPANTYPE type, void *detail_ptr);
  int dispatch_text(MD_TEXTTYPE type, const MD_CHAR *text, MD_SIZE size);

  /* handlers for enter_block */
  void handle_doc_enter();
  void handle_h_enter(MD_BLOCK_H_DETAIL *d);
  void handle_p_enter();
  void handle_html_enter();
  void handle_ul_enter(MD_BLOCK_UL_DETAIL *d);
  void handle_ol_enter(MD_BLOCK_OL_DETAIL *d);
  void handle_li_enter(MD_BLOCK_LI_DETAIL *d);
  void handle_hr_enter();
  void handle_code_enter(MD_BLOCK_CODE_DETAIL *d);
  //...

  /* handlers for leave_block */
  void handle_doc_leave();
  void handle_h_leave(MD_BLOCK_H_DETAIL *d);
  void handle_p_leave();
  void handle_html_leave();
  void handle_ul_leave();
  void handle_ol_leave();
  void handle_li_leave();
  void handle_hr_leave();
  void handle_code_leave();
  //...

  /* handlers for enter_span */
  void handle_em_enter();
  void handle_strong_enter();
  void handle_backtick_enter();
  void handle_link_enter(MD_SPAN_A_DETAIL *d);
  void handle_img_enter(MD_SPAN_IMG_DETAIL *d);
  //...

  /* handlers for leave_span */
  void handle_em_leave();
  void handle_strong_leave();
  void handle_backtick_leave();
  void handle_link_leave();
  void handle_img_leave();
  //...

  /* handlers for text */
  void handle_normal_text(const MD_CHAR *text, MD_SIZE size);
  void handle_br_text();
  void handle_html_text(const MD_CHAR *text, MD_SIZE size);
  void handle_code_text(const MD_CHAR *text, MD_SIZE size);
};

#endif  //! HTML_GEN_H
