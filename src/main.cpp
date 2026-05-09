#include <fstream>
#include <iostream>
#include <iterator>
#include "html_gen.h"
#include "utils.hpp"

int main(void) {
  verbose             = true;
  std::string content = load_file("test.md");

  HTMLGen generator(content);
  int     res = generator.parse_markdown();

  if (res == 0) {
    std::cout << "Successfully made tent" << std::endl;
  } else {
    std::cout << "Some problem occured" << std::endl;
  }

  std::ifstream tfile("templates/base.html");
  std::string   base((std::istreambuf_iterator<char>(tfile)),
                     std::istreambuf_iterator<char>());

  size_t bc = base.find("{blog_content}");
  tfile.close();

  if (bc != std::string::npos) {
    base.replace(bc, 14, generator.get_html());
  } else {
    std::cout << "nahi mila lawdiya" << std::endl;
  }

  std::ofstream outf("index.html");
  outf.write(base.c_str(), base.size());
  outf.close();

  return 0;
}
