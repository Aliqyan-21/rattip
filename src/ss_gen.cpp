#include "ss_gen.h"
#include <filesystem>
#include <fstream>
#include "html_gen.h"
#include "utils.hpp"

SSGen::SSGen(const std::string &main_folder_path,
             const std::string &public_folder_path)
  : main_folder_(main_folder_path), public_folder_(public_folder_path) {
  base_ = load_file("templates/base.html");
}

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
  std::filesystem::create_directories(public_folder_);

  std::string page = base_;

  size_t tt = page.find("{blog_title}");
  page.replace(tt, std::string("{blog_title}").size(), front_matter.title);
  size_t bc = page.find("{blog_content}");
  page.replace(bc, std::string("{blog_content}").size(), html_content);

  std::ofstream of(public_folder_ + "/" + get_filename_from_path(md_file) +
                   ".html");
  of.write(page.c_str(), page.size());
  of.close();
}
