#include "md4c.h"
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <unordered_map>

std::unordered_map<MD_BLOCKTYPE, std::string> blockt_to_str = {
    {MD_BLOCK_DOC, "body"},
    {MD_BLOCK_QUOTE, "MD_BLOCK_QUOTE"},
    {MD_BLOCK_UL, "ul"},
    {MD_BLOCK_OL, "ol"},
    {MD_BLOCK_LI, "li"},
    {MD_BLOCK_HR, "MD_BLOCK_HR"},
    {MD_BLOCK_H, "h"},
    {MD_BLOCK_CODE, "MD_BLOCK_CODE"},
    {MD_BLOCK_HTML, "MD_BLOCK_HTML"},
    {MD_BLOCK_P, "p"},
    {MD_BLOCK_TABLE, "MD_BLOCK_TABLE"},
    {MD_BLOCK_THEAD, "MD_BLOCK_THEAD"},
    {MD_BLOCK_TBODY, "MD_BLOCK_TBODY"},
    {MD_BLOCK_TR, "MD_BLOCK_TR"},
    {MD_BLOCK_TH, "MD_BLOCK_TH"},
    {MD_BLOCK_TD, "MD_BLOCK_TD"},
};

std::unordered_map<MD_TEXTTYPE, std::string> textt_to_str = {
    {MD_TEXT_NORMAL, " "},
    {MD_TEXT_NULLCHAR, "MD_TEXT_NULLCHAR"},
    {MD_TEXT_BR, "<br/>"},
    {MD_TEXT_SOFTBR, ""},
    {MD_TEXT_ENTITY, "MD_TEXT_ENTITY"},
    {MD_TEXT_CODE, "MD_TEXT_CODE"},
    {MD_TEXT_HTML, ""},
    {MD_TEXT_LATEXMATH, "MD_TEXT_LATEXMATH"},
};

std::unordered_map<MD_SPANTYPE, std::string> spant_to_str = {
    {MD_SPAN_EM, "i"},
    {MD_SPAN_STRONG, "b"},
    {MD_SPAN_A, "MD_SPAN_A"},
    {MD_SPAN_IMG, "MD_SPAN_IMG"},
    {MD_SPAN_CODE, "MD_SPAN_CODE"},
    {MD_SPAN_DEL, "MD_SPAN_DEL"},
    {MD_SPAN_LATEXMATH, "MD_SPAN_LATEXMATH"},
    {MD_SPAN_LATEXMATH_DISPLAY, "MD_SPAN_LATEXMATH_DISPLAY"},
    {MD_SPAN_WIKILINK, "MD_SPAN_WIKILINK"},
    {MD_SPAN_U, "MD_SPAN_U"},
    {MD_SPAN_SPOILER, "MD_SPAN_SPOILER"},
    {MD_SPAN_SUPERSCRIPT, "MD_SPAN_SUPERSCRIPT"},
    {MD_SPAN_SUBSCRIPT, "MD_SPAN_SUBSCRIPT"},
};

int enter(MD_BLOCKTYPE type, void *detail_ptr, void *html_buf) {
  std::string *this_html_buf = static_cast<std::string *>(html_buf);
  if (type == MD_BLOCK_H) {
    auto *detail = static_cast<MD_BLOCK_H_DETAIL *>(detail_ptr);
    this_html_buf->append("<" + blockt_to_str.at(type) +
                          std::to_string(detail->level) + ">" + "\n");
  } else {
    this_html_buf->append("<" + blockt_to_str.at(type) + ">" + "\n");
  }
  return 0;
}

int leave(MD_BLOCKTYPE type, void *detail_ptr, void *html_buf) {
  std::string *this_html_buf = static_cast<std::string *>(html_buf);
  if (type == MD_BLOCK_H) {
    auto *detail = static_cast<MD_BLOCK_H_DETAIL *>(detail_ptr);
    this_html_buf->append("</" + blockt_to_str.at(type) +
                          std::to_string(detail->level) + ">" + "\n");
  } else {
    this_html_buf->append("</" + blockt_to_str.at(type) + ">" + "\n");
  }
  return 0;
}

int text(MD_TEXTTYPE type, const MD_CHAR *text, MD_SIZE size, void *html_buf) {
  std::string *this_html_buf = static_cast<std::string *>(html_buf);
  this_html_buf->append(textt_to_str.at(type) + std::string(text, size) + "\n");
  return 0;
}

int enter_s(MD_SPANTYPE type, void *, void *html_buf) {
  std::string *this_html_buf = static_cast<std::string *>(html_buf);
  this_html_buf->append("<" + spant_to_str.at(type) + ">" + "\n");
  return 0;
}
int leave_s(MD_SPANTYPE type, void *, void *html_buf) {
  std::string *this_html_buf = static_cast<std::string *>(html_buf);
  this_html_buf->append("</" + spant_to_str.at(type) + ">" + "\n");
  return 0;
}

int main(void) {
  std::string html_buf;
  std::ifstream inf("test.md");
  if (!inf) {
    std::cout << "wtf" << std::endl;
    return 1;
  }
  std::string content((std::istreambuf_iterator<char>(inf)),
                      std::istreambuf_iterator<char>());
  MD_PARSER parser = {};
  parser.abi_version = 0;

  parser.enter_block = enter;
  parser.leave_block = leave;
  parser.enter_span = enter_s;
  parser.leave_span = leave_s;
  parser.text = text;

  int res = md_parse(content.c_str(), content.size(), &parser, &html_buf);
  inf.close();

  // if (res == 0) {
  //   std::cout << "Successfully made tent\n" << std::endl;
  // } else {
  //   std::cout << "Some problem occured" << std::endl;
  // }

  std::ifstream tfile("templates/base.html");
  std::string base((std::istreambuf_iterator<char>(tfile)),
                   std::istreambuf_iterator<char>());

  size_t bc = base.find("{blog_content}");
  tfile.close();

  if (bc != std::string::npos) {
    base.replace(bc, 14, html_buf);
  } else {
    std::cout << "nahi mila lawdiya" << std::endl;
  }

  std::ofstream outf("index.html");
  outf.write(base.c_str(), base.size());
  outf.close();

  return 0;
}
