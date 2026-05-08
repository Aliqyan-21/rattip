#include "html_gen.h"

HTMLGen::HTMLGen(const std::string &content, uint32_t abi_version)
    : content_(content) {
  parser_.abi_version = abi_version;
}

int HTMLGen::parse_markdown() {
  parser_.enter_block = HTMLGen::enter_block;
  parser_.leave_block = HTMLGen::leave_block;
  parser_.enter_span = HTMLGen::enter_span;
  parser_.leave_span = HTMLGen::leave_span;
  parser_.text = HTMLGen::text;
  int res = md_parse(content_.c_str(), content_.size(), &parser_, this);
  return res;
}

int HTMLGen::enter_block(MD_BLOCKTYPE type, void *detail_ptr, void *gen_data) {
  auto *self = static_cast<HTMLGen *>(gen_data);
  return self->dispatch_enter_block(type, detail_ptr);
}
int HTMLGen::leave_block(MD_BLOCKTYPE type, void *detail_ptr, void *gen_data) {
  auto *self = static_cast<HTMLGen *>(gen_data);
  return self->dispatch_leave_block(type, detail_ptr);
}
int HTMLGen::enter_span(MD_SPANTYPE type, void *detail_ptr, void *gen_data) {
  // TODO: implement
  return 0;
}
int HTMLGen::leave_span(MD_SPANTYPE type, void *detail_ptr, void *gen_data) {
  // TODO: implement
  return 0;
}
int HTMLGen::text(MD_TEXTTYPE type, const MD_CHAR *text, MD_SIZE size,
                  void *gen_data) {
  auto *self = static_cast<HTMLGen *>(gen_data);
  return self->dispatch_text(type, text, size);
}

int HTMLGen::dispatch_enter_block(MD_BLOCKTYPE type, void *detail_ptr) {
  switch (type) {
  case MD_BLOCK_H:
    handle_h_enter(static_cast<MD_BLOCK_H_DETAIL *>(detail_ptr));
    break;
  default:
    break;
  }
  return 0;
}
int HTMLGen::dispatch_leave_block(MD_BLOCKTYPE type, void *detail_ptr) {
  switch (type) {
  case MD_BLOCK_H:
    handle_h_leave(static_cast<MD_BLOCK_H_DETAIL *>(detail_ptr));
    break;
  default:
    break;
  }
  return 0;
}
int HTMLGen::dispatch_enter_span(MD_BLOCKTYPE type, void *detail_ptr) {
  // TODO: implement
  return 0;
}
int HTMLGen::dispatch_leave_span(MD_BLOCKTYPE type, void *detail_ptr) {
  // TODO: implement
  return 0;
}
int HTMLGen::dispatch_text(MD_TEXTTYPE type, const MD_CHAR *text,
                           MD_SIZE size) {
  switch (type) {
  case MD_TEXT_NORMAL:
    handle_normal_text(text, size);
    break;
  default:
    break;
  }
  return 0;
}

void HTMLGen::handle_h_enter(MD_BLOCK_H_DETAIL *d) {
  html_buf_ += "<h" + std::to_string(d->level) + "> ";
}

void HTMLGen::handle_h_leave(MD_BLOCK_H_DETAIL *d) {
  html_buf_ += "</h" + std::to_string(d->level) + ">";
}

void HTMLGen::handle_normal_text(const MD_CHAR *text, MD_SIZE size) {
  html_buf_ += std::string(text, size);
}
