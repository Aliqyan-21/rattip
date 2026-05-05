#include "md4c.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <unordered_map>

std::unordered_map<MD_BLOCKTYPE, std::string> blockt_to_str = {
    {MD_BLOCK_DOC, "MD_BLOCK_DOC"},     {MD_BLOCK_QUOTE, "MD_BLOCK_QUOTE"},
    {MD_BLOCK_UL, "MD_BLOCK_UL"},       {MD_BLOCK_OL, "MD_BLOCK_OL"},
    {MD_BLOCK_LI, "MD_BLOCK_LI"},       {MD_BLOCK_HR, "MD_BLOCK_HR"},
    {MD_BLOCK_H, "MD_BLOCK_H"},         {MD_BLOCK_CODE, "MD_BLOCK_CODE"},
    {MD_BLOCK_HTML, "MD_BLOCK_HTML"},   {MD_BLOCK_P, "MD_BLOCK_P"},
    {MD_BLOCK_TABLE, "MD_BLOCK_TABLE"}, {MD_BLOCK_THEAD, "MD_BLOCK_THEAD"},
    {MD_BLOCK_TBODY, "MD_BLOCK_TBODY"}, {MD_BLOCK_TR, "MD_BLOCK_TR"},
    {MD_BLOCK_TH, "MD_BLOCK_TH"},       {MD_BLOCK_TD, "MD_BLOCK_TD"},
};

std::unordered_map<MD_TEXTTYPE, std::string> textt_to_str = {
    {MD_TEXT_NORMAL, "MD_TEXT_NORMAL"},
    {MD_TEXT_NULLCHAR, "MD_TEXT_NULLCHAR"},
    {MD_TEXT_BR, "MD_TEXT_BR"},
    {MD_TEXT_SOFTBR, "MD_TEXT_SOFTBR"},
    {MD_TEXT_ENTITY, "MD_TEXT_ENTITY"},
    {MD_TEXT_CODE, "MD_TEXT_CODE"},
    {MD_TEXT_HTML, "MD_TEXT_HTML"},
    {MD_TEXT_LATEXMATH, "MD_TEXT_LATEXMATH"},
};

std::unordered_map<MD_SPANTYPE, std::string> spant_to_str = {
    {MD_SPAN_EM, "MD_SPAN_EM"},
    {MD_SPAN_STRONG, "MD_SPAN_STRONG"},
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

int enter(MD_BLOCKTYPE type, void *, void *) {
  std::cout << "[Entered] " << blockt_to_str.at(type) << std::endl;
  return 0;
}

int leave(MD_BLOCKTYPE type, void *, void *) {
  std::cout << "[Left] " << blockt_to_str.at(type) << std::endl;
  return 0;
}

int text(MD_TEXTTYPE type, const MD_CHAR *text, MD_SIZE size, void *) {
  std::cout << "[Text] " << textt_to_str.at(type) << "\n"
            << std::string(text, size) << std::endl;
  return 0;
}

int enter_s(MD_SPANTYPE type, void *, void *) {
  std::cout << "[Entered] " << spant_to_str.at(type) << std::endl;
  return 0;
}
int leave_s(MD_SPANTYPE type, void *, void *) {
  std::cout << "[Left] " << spant_to_str.at(type) << std::endl;
  return 0;
}

int main(void) {
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

  int res = md_parse(content.c_str(), content.size(), &parser, nullptr);

  if (res == 0) {
    std::cout << "Successfully made tent" << std::endl;
  } else {
    std::cout << "Some problem occured" << std::endl;
  }

  return 0;
}
