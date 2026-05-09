#include <fstream>
#include <iostream>
#include <iterator>
#include "html_gen.h"
#include "utils.hpp"

int main(void) {
  verbose             = true;
  std::string content = load_file("test.md");

  FMatter fm = parse_front_matter(content);
  std::cout << "title: " << fm.title << std::endl;
  std::cout << "date: " << fm.date << std::endl;

  int flags = MD_FLAG_STRIKETHROUGH | MD_FLAG_UNDERLINE | MD_FLAG_SUPERSCRIPTS |
              MD_FLAG_SUBSCRIPTS | MD_FLAG_TABLES;

  HTMLGen generator(content, flags);
  int     res = generator.parse_markdown();

  if (res == 0) {
    std::cout << "Successfully made tent" << std::endl;
  } else {
    std::cout << "Some problem occured" << std::endl;
  }

  std::ifstream tfile("templates/base.html");
  std::string   base((std::istreambuf_iterator<char>(tfile)),
                     std::istreambuf_iterator<char>());

  size_t tt = base.find("{blog_title}");

  size_t bc = base.find("{blog_content}");
  tfile.close();

  base.replace(bc, 14, generator.get_html());
  base.replace(tt, 12, fm.title);

  std::ofstream outf("index.html");
  outf.write(base.c_str(), base.size());
  outf.close();

  return 0;
}
