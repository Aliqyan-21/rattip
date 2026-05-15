#include "ss_gen.h"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <thread>
#include "html_gen.h"
#include "utils.hpp"

SSGen::SSGen(const std::string &main_dir_path,
             const std::string &public_dir_path, const std::string &assets_dir)
  : main_dir_(main_dir_path),
    public_dir_(public_dir_path),
    assets_dir_(assets_dir) {
  load_templates();
  std::filesystem::create_directory(public_dir_path);
  if (std::filesystem::exists(std::filesystem::path(assets_dir_))) {
    std::filesystem::copy(assets_dir_, public_dir_ + "/" + assets_dir_,
                          std::filesystem::copy_options::recursive |
                            std::filesystem::copy_options::overwrite_existing);
  } else {
    V66V("Skipping loading ", assets_dir_, "\nDoes not exists.");
  }
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

/* watch for any change (file saved) in main_dir_ */
void SSGen::watch_and_regen(std::atomic<bool> &reload_flag) {
  std::unordered_map<std::string, std::filesystem::file_time_type>
    snaps;  // [name : time last changed]

  for (auto const &en :
       std::filesystem::recursive_directory_iterator(main_dir_)) {
    if (en.path().extension() == ".md") {
      snaps[en.path()] = std::filesystem::last_write_time(en.path());
    }
  }

  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    for (auto const &en :
         std::filesystem::recursive_directory_iterator(main_dir_)) {
      if (en.path().extension() != ".md") { continue; }
      auto ct = std::filesystem::last_write_time(en.path());
      if (ct != snaps[en.path()]) {
        snaps[en.path()] = ct;
        V66V("Change detected: ", en.path().string());
        generate_html();
        reload_flag = true;
      }
    }
  }
}

/* collect all the md files from the main_folder */
void SSGen::content_walker() {
  for (const std::filesystem::directory_entry &en :
       std::filesystem::recursive_directory_iterator(main_dir_)) {
    if (en.is_regular_file() && en.path().extension() == ".md") {
      md_files_.push_back(en.path());
    }
  }
}

/* intialize theme struct object */
void SSGen::init_theme(const std::string &name) {
  theme_.name = name;
  std::filesystem::copy("themes/" + name, public_dir_ + "/styles/",
                        std::filesystem::copy_options::recursive |
                          std::filesystem::copy_options::overwrite_existing);
}

void SSGen::generate_html() {
  int flags = MD_FLAG_STRIKETHROUGH | MD_FLAG_UNDERLINE | MD_FLAG_SUPERSCRIPTS |
              MD_FLAG_SUBSCRIPTS | MD_FLAG_TABLES;

  for (const std::string &mf : md_files_) {
    std::filesystem::path md_path(mf);
    std::filesystem::path rel = std::filesystem::relative(md_path, main_dir_);
    std::filesystem::path out_path = std::filesystem::path(public_dir_) / rel;
    out_path.replace_extension(".html");

    if (!force_ && std::filesystem::exists(out_path) &&
        std::filesystem::last_write_time(out_path) >=
          std::filesystem::last_write_time(md_path)) {
      V66V("Skipping: ", mf, " (no changes)");
      continue;
    }

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
  std::filesystem::path rel = std::filesystem::relative(md_path, main_dir_);
  std::filesystem::path out_path = std::filesystem::path(public_dir_) / rel;
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
  }
  if (fm.tmpl.empty()) { fm.tmpl = "page"; }
  if (!start) { return fm; }
  content = std::string(std::istreambuf_iterator<char>(ss), {});
  V66V("Front Matter parsed successfully!\n");
  return fm;
}

void SSGen::load_templates() {
  if (!std::filesystem::exists(std::filesystem::path("templates")) ||
      std::filesystem::is_empty(std::filesystem::path("templates"))) {
    std::cerr << "templates folder is empty or does not exists, html files "
                 "formed will be without "
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

void SSGen::set_force() {
  V66V(
    "Force option set, so html will be generated for all the markdown files\n");
  force_ = true;
}
