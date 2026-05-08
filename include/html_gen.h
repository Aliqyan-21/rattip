#ifndef HTML_GEN_H
#define HTML_GEN_H

#include "md4c.h"
#include <cstdint>
#include <string>

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
  MD_PARSER parser_ = {};
  std::string html_buf_; // generated html

  /* dispatchers */
  int dispatch_enter_block(MD_BLOCKTYPE type, void *detail_ptr);
  int dispatch_leave_block(MD_BLOCKTYPE type, void *detail_ptr);
  int dispatch_enter_span(MD_BLOCKTYPE type, void *detail_ptr);
  int dispatch_leave_span(MD_BLOCKTYPE type, void *detail_ptr);
  int dispatch_text(MD_TEXTTYPE type, const MD_CHAR *text, MD_SIZE size);

  /* handlers for enter_block */
  void handle_h_enter(MD_BLOCK_H_DETAIL *d);
  //...

  /* handlers for leave_block */
  void handle_h_leave(MD_BLOCK_H_DETAIL *d);
  //...

  /* handlers for enter_span */
  //...

  /* handlers for leave_span */
  //...

  /* handlers for text */
  void handle_normal_text(const MD_CHAR *text, MD_SIZE size);
};

#endif //! HTML_GEN_H
