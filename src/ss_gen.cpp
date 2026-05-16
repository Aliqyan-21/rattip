#include "ss_gen.h"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <thread>
#include "html_gen.h"
#include "utils.hpp"

SSGen::SSGen(const std::string &main_dir_path,
             const std::string &public_dir_path, const std::string template_dir,
             const std::string &assets_dir)
  : main_dir_(main_dir_path),
    public_dir_(public_dir_path),
    template_dir_(template_dir),
    assets_dir_(assets_dir) {
  std::filesystem::create_directory(public_dir_);
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
        files_.clear();
        nav_pages_.clear();
        navbar_.clear();
        content_walker();
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
      // md_files_.push_back(en.path());
      std::string content = load_file(en.path());
      FMatter     fm      = parse_front_matter(content, en.path());
      files_.push_back({en.path(), content, fm});

      if (fm.nav) {
        std::filesystem::path rel =
          std::filesystem::relative(en.path(), main_dir_);
        std::filesystem::path url = std::filesystem::path("/") / rel;
        url.replace_extension(".html");
        nav_pages_.push_back({fm.title, url.string()});
      }
    }
  }

  navbar_ = "<nav class=\"rattip-nav\">\n";
  for (auto &item : nav_pages_) {
    navbar_ += "  <a href=\"" + item.second + "\" class=\"rattip-nav-a\">" +
               item.first + "</a>\n";
  }
  navbar_ += "</nav>";
}

/* intialize theme struct object */
void SSGen::init_theme(const std::string &name, const std::string &theme_dir) {
  theme_.name      = name;
  theme_.theme_dir = theme_dir;
  std::filesystem::copy(theme_.theme_dir + "/" + name, public_dir_ + "/styles/",
                        std::filesystem::copy_options::recursive |
                          std::filesystem::copy_options::overwrite_existing);
  V66V("Theme '", theme_.name, "' loaded successfully\n");
}

void SSGen::generate_html() {
  int flags = MD_FLAG_STRIKETHROUGH | MD_FLAG_UNDERLINE | MD_FLAG_SUPERSCRIPTS |
              MD_FLAG_SUBSCRIPTS | MD_FLAG_TABLES;

  for (const auto &[mf, content, fm] : files_) {
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

    HTMLGen generator(content, flags);
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
    throw SSGError("Could not find the template for: " + front_matter.tmpl,
                   "fix front matter if template name is wrong, or add the "
                   "required template");
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
  size_t nv = tmpl.find("{navbar}");
  if (nv != std::string::npos) {
    tmpl.replace(nv, std::string("{navbar}").size(),
                 front_matter.nav ? navbar_ : "");
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
FMatter SSGen::parse_front_matter(std::string       &content,
                                  const std::string &md_file) {
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

    key == "title" ? fm.title = val : "";
    key == "date" ? fm.date = val : "";
    key == "template" ? fm.tmpl = val : "";
    if (key == "nav") { fm.nav = (val == "true"); }
  }
  if (fm.tmpl.empty()) {
    throw SSGError("No template field found in front_matter of: " + md_file,
                   "fix it.");
  }
  if (!start) { return fm; }
  content = std::string(std::istreambuf_iterator<char>(ss), {});
  V66V("Front Matter parsed successfully!\n");
  return fm;
}

void SSGen::load_templates() {
  if (!std::filesystem::exists(std::filesystem::path(template_dir_)) ||
      std::filesystem::is_empty(std::filesystem::path(template_dir_))) {
    throw SSGError("Templates directory '" + template_dir_ +
                     "' is empty or does not exists (see -h (--templates))",
                   "Try rattip --init");
  }
  for (auto const &en :
       std::filesystem::recursive_directory_iterator(template_dir_)) {
    std::string fn = en.path().filename().stem();
    templates_[fn] = load_file(en.path());
  }
}

void SSGen::load_assets() {
  if (std::filesystem::exists(std::filesystem::path(assets_dir_))) {
    std::filesystem::copy(assets_dir_, public_dir_ + "/" + assets_dir_,
                          std::filesystem::copy_options::recursive |
                            std::filesystem::copy_options::overwrite_existing);
    V66V("Assets loaded successfully from '", assets_dir_, "'");
  } else {
    V66V("Skipping loading assets '", assets_dir_,
         "' - Does not exists.(see -h (--assets))");
  }
}

void SSGen::set_force() {
  V66V(
    "Force option set, so html will be generated for all the markdown files\n");
  force_ = true;
}
