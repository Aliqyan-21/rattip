#include "ss_gen.h"
#include <filesystem>
#include <fstream>
#include "html_gen.h"
#include "utils.hpp"

SSGen::SSGen(const std::string &main_folder_path,
             const std::string &public_folder_path)
  : main_folder_(main_folder_path), public_folder_(public_folder_path) {
  load_templates();
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

/* intialize theme struct object */
void SSGen::init_theme(const std::string &name, const std::string &dir) {
  theme_.name      = name;
  theme_.theme_dir = dir;
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
  std::filesystem::path md_path(md_file);
  std::filesystem::path rel = std::filesystem::relative(md_path, main_folder_);
  std::filesystem::path out_path = std::filesystem::path(public_folder_) / rel;
  out_path.replace_extension(".html");
  std::filesystem::create_directories(out_path.parent_path());
  auto        it = templates_.find(front_matter.tmpl);
  std::string tmpl;
  if (it != templates_.end()) {
    tmpl = it->second;
  } else {
    std::cerr << "Could not find the template for: " << front_matter.tmpl
              << "\n"
              << "Defaulting to using: " << templates_.begin()->first
              << std::endl;
    tmpl = templates_[templates_.begin()->first];
  }
  std::ofstream of(out_path);

  size_t tt = tmpl.find("{title}");
  tmpl.replace(tt, std::string("{title}").size(), front_matter.title);
  size_t bc = tmpl.find("{md_content}");
  tmpl.replace(bc, std::string("{md_content}").size(), html_content);
  if ("blog" == front_matter.tmpl) {
    size_t bd = tmpl.find("{blog_date}");
    tmpl.replace(bd, std::string("{blog_date}").size(), front_matter.date);
  }

  of.write(tmpl.c_str(), tmpl.size());
  of.close();
}

/* something like this:
---
title: first blog
date: 1998-01-23
template: [page/blog/...]
---
*/
FMatter SSGen::parse_front_matter(std::string &content) {
  FMatter fm;

  std::stringstream ss(content);
  std::string       line;
  bool              start{false};

  while (std::getline(ss, line)) {
    if (!start) {
      if ("---" == trim(line)) {
        start = true;
        continue;
      }
    }
    if ("---" == trim(line)) { break; }

    uint64_t pos = line.find(":");
    if (pos == std::string::npos) { continue; }

    std::string key = trim(line.substr(0, pos));
    std::string val = trim(line.substr(pos + 1));

    key == "title"      ? fm.title     = val
    : key == "date"     ? fm.date     = val
    : key == "template" ? fm.tmpl = val
                        : "";
    if (fm.tmpl.empty()) { fm.tmpl = "page"; }
  }
  if (!start) { return fm; }
  content = std::string(std::istreambuf_iterator<char>(ss), {});
  V66V("Front Matter parsed successfully!\n");
  return fm;
}

void SSGen::load_templates() {
  if (!std::filesystem::exists(std::filesystem::path("templates")) ||
      std::filesystem::is_empty(std::filesystem::path("templates"))) {
    std::cerr << "templates folder is empty, html files formed will be without "
                 "templates"
              << std::endl;
    return;
  }
  for (auto const &en :
       std::filesystem::recursive_directory_iterator("templates")) {
    std::string fn = en.path().filename().stem();
    templates_[fn] = load_file(en.path());
  }
}
