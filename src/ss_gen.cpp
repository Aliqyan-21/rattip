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

  auto snap_dir = [&](const std::string &dir) {
    if (!std::filesystem::exists(dir)) { return; }
    for (const auto &en : std::filesystem::recursive_directory_iterator(dir)) {
      if (en.is_regular_file()) {
        snaps[en.path()] = std::filesystem::last_write_time(en.path());
      }
    }
  };

  snap_dir(main_dir_);
  snap_dir(template_dir_);
  snap_dir(theme_.theme_dir + "/" + theme_.name);
  snap_dir(assets_dir_);

  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    bool changed{false};
    bool needs_content_reload{false};
    bool needs_template_reload{false};
    bool needs_theme_reload{false};
    bool needs_assets_reload{false};

    auto check_dir = [&](const std::string &dir, bool &needs_reload) {
      if (!std::filesystem::exists(dir)) { return; }
      for (const auto &en :
           std::filesystem::recursive_directory_iterator(dir)) {
        if (!en.is_regular_file()) { continue; }
        auto ct = std::filesystem::last_write_time(en.path());
        if (ct != snaps[en.path()]) {
          snaps[en.path()] = ct;
          changed          = true;
          V66V("change detected: ", en.path().string());
          needs_reload = true;
        }
      }
    };

    check_dir(main_dir_, needs_content_reload);
    check_dir(template_dir_, needs_template_reload);
    check_dir(theme_.theme_dir + "/" + theme_.name, needs_theme_reload);
    check_dir(assets_dir_, needs_assets_reload);

    if (changed) {
      if (needs_template_reload) {
        V66V("Templates changed, reloading...");
        load_templates();
      }
      if (needs_theme_reload) {
        V66V("Themes changed, reloading...");
        init_theme(theme_.name, theme_.theme_dir);
      }
      if (needs_assets_reload) {
        V66V("Assets changed, reloading...");
        load_assets();
      }
      if (needs_template_reload || needs_theme_reload || needs_content_reload) {
        V66V("Content changed, reloading...");
        files_.clear();
        nav_pages_.clear();
        navbar_.clear();
        content_walker();
        generate_html();
      }
      reload_flag = true;
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
        nav_pages_.push_back({fm.title, url.string(), fm.nav_order});
      }
    }
  }
  V66V("Found ", files_.size(), " markdown files");
  V66V("Found ", nav_pages_.size(), " nav items");

  std::sort(nav_pages_.begin(), nav_pages_.end(),
            [](const NavL &a, const NavL &b) { return a.order < b.order; });

  navbar_ = "<nav class=\"rattip-nav\">\n";
  for (auto &item : nav_pages_) {
    navbar_ += "  <a href=\"" + item.url + "\" class=\"rattip-nav-a\">" +
               item.title + "</a>\n";
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
              MD_FLAG_SUBSCRIPTS | MD_FLAG_TABLES | MD_FLAG_TASKLISTS |
              MD_FLAG_SPOILERS;

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
    int     res = generator.parse_markdown();
    if (res != 0) {
      /* md4c is quiet lenient so..., we'll see */
      W66W("An error occured while parsing markdown file: ", md_path);
    }
    save_html_file(generator.get_html(), fm, mf);
  }
  V66V("HTML generation complete");
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
  bool        no_tmpl{false};
  if (it != templates_.end()) {
    tmpl = it->second;
  } else {
    no_tmpl = true;
    front_matter.tmpl.empty()
      ? W66W("Template value empty in front matter, fix it.")
      : W66W("Template '", front_matter.tmpl, "' not found for: ", md_file);
    W66W("HTML will be generated without template");
  }
  std::ofstream of(out_path);

  if (!no_tmpl) {
    size_t fr = tmpl.find("{title}");
    tmpl.replace(fr, std::string("{title}").size(), front_matter.title);
    fr = tmpl.find("{md_content}");
    tmpl.replace(fr, std::string("{md_content}").size(), html_content);
    if ("blog" == front_matter.tmpl) {
      fr = tmpl.find("{blog_date}");
      tmpl.replace(fr, std::string("{blog_date}").size(), front_matter.date);
    }
    fr = tmpl.find("{navbar}");
    if (fr != std::string::npos) {
      tmpl.replace(fr, std::string("{navbar}").size(),
                   front_matter.nav ? navbar_ : "");
    }
    fr = tmpl.find("{css}");
    tmpl.replace(fr, std::string("{css}").size(), front_matter.css + ".css");
  } else {
    tmpl = html_content;
  }
  V66V("Writing: ", out_path.string());
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
    if (key == "nav") {
      fm.nav = true;
      if (!val.empty()) {
        try {
          fm.nav_order = std::stoi(val);
        } catch (...) { fm.nav_order = 99; }
      }
    }
    key == "css" ? fm.css = val : "";
  }
  if (fm.tmpl.empty()) {
    W66W("No template field found in front_matter of: ", md_file);
    W66W(
      "The html formed will be without any template, fix it by putting "
      "template field");
    fm.tmpl = "";
  }
  if (fm.css.empty()) {
    W66W(
      "the css field in front matter is there but empty, so default css will "
      "be used: 'global.css'");
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
    V66V("Template loaded: ", fn);
  }
}

void SSGen::load_assets() {
  if (std::filesystem::exists(std::filesystem::path(assets_dir_))) {
    std::filesystem::copy(assets_dir_, public_dir_ + "/" + assets_dir_,
                          std::filesystem::copy_options::recursive |
                            std::filesystem::copy_options::overwrite_existing);
    V66V("Assets loaded successfully from '", assets_dir_, "'");
  } else {
    W66W("Skipping loading assets '", assets_dir_,
         "' - Does not exists.(see -h (--assets))");
  }
}

void SSGen::set_force() {
  V66V(
    "Force option set, so html will be generated for all the markdown files\n");
  force_ = true;
}
