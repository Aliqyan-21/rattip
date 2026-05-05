#include "md4c.h"
#include <fstream>
#include <iostream>
#include <iterator>

int enter(MD_BLOCKTYPE, void *, void *) {
  std::cout << "entered some kind of md syntax" << std::endl;
  return 0;
}

int leave(MD_BLOCKTYPE, void *, void *) {
  std::cout << "left it" << std::endl;
  return 0;
}

int text(MD_TEXTTYPE, const MD_CHAR *text, MD_SIZE size, void *) {
  std::cout << "now reading some text: " << std::string(text, size)
            << std::endl;
  return 0;
}

int enter_s(MD_SPANTYPE, void *, void *) { return 0; }
int leave_s(MD_SPANTYPE, void *, void *) { return 0; }

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
