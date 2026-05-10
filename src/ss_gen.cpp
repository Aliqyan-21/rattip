#include "ss_gen.h"
#include <filesystem>
#include <fstream>
#include "html_gen.h"
#include "utils.hpp"

SSGen::SSGen(const std::string &main_folder_path,
             const std::string &public_folder_path)
  : main_folder_(main_folder_path), public_folder_(public_folder_path) {}

/*
 * a recursive functin that will walk through the
 * main folder and find all the 'md' files and generate
 * html out of them and store in public folder (?)
 */
void SSGen::generate_site() {
  content_walker();
  generate_html();
}

/* collect all the md files from the main_folder */
void SSGen::content_walker() {
  for (const std::filesystem::directory_entry &en :
       std::filesystem::recursive_directory_iterator(main_folder_)) {
    if (en.is_regular_file() && en.path().extension() == ".md") {
      md_files_.push_back(en.path());
    }
  }
}

void SSGen::generate_html() {
  int flags = MD_FLAG_STRIKETHROUGH | MD_FLAG_UNDERLINE | MD_FLAG_SUPERSCRIPTS |
              MD_FLAG_SUBSCRIPTS | MD_FLAG_TABLES;

  for (const std::string &mf : md_files_) {
    std::string content = load_file(mf);
    FMatter     fm      = parse_front_matter(content);
    HTMLGen     generator(content, flags);
    generator.parse_markdown();
    save_html_file(generator.get_html(), fm, mf);
  }
}

void SSGen::save_html_file(const std::string &html_content,
                           const FMatter     &front_matter,
                           const std::string &md_file) {
  std::filesystem::create_directory(public_folder_);

  std::string base = load_file("templates/base.html");
  size_t      tt   = base.find("{blog_title}");
  size_t      bc   = base.find("{blog_content}");

  base.replace(bc, 14, html_content);
  base.replace(tt, 12, front_matter.title);

  std::ofstream of(public_folder_ + "/" + get_filename_from_path(md_file) +
                   ".html");
  of.write(base.c_str(), base.size());
  of.close();
}
