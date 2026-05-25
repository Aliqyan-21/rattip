#ifndef SS_GEN_H
#define SS_GEN_H

#include <atomic>
#include <string>
#include <unordered_map>
#include <vector>

struct FMatter {
  std::string title; /* title: home */
  std::string date;  /* date: 2-2-2022 */
  std::string tmpl;  /* tmpl: page */
  bool        nav{false};
  int         nav_order{99}; /* nav: 0 */
  std::string css{"global"}; /* css: contact - css file: contact.css */
};

struct Theme {
  std::string name;
  std::string theme_dir;
};

struct NavL {
  std::string title;
  std::string url;
  int         order;
};

class SSGen {
public:
  SSGen(const std::string &main_dir_path, const std::string &public_dir_path,
        const std::string template_dir, const std::string &assets_dir);

  void generate_site();
  void init_theme(const std::string &name, const std::string &theme_dir);
  void watch_and_regen(std::atomic<int> &reload_gen);
  void set_force();
  void load_templates();
  void load_assets();

private:
  std::string main_dir_;
  std::string public_dir_; /* final html will be generated to be served */
  std::string template_dir_;
  std::string assets_dir_;
  std::vector<std::tuple<std::string, std::string, FMatter>>
        files_; /* [md_file, content, front_matter] */
  Theme theme_;

  /* templates */
  std::unordered_map<std::string, std::string> templates_;

  std::vector<NavL> nav_pages_;
  std::string       navbar_;

  bool force_{false};  // force generation of all files

  FMatter parse_front_matter(std::string       &content,
                             const std::string &md_file /*for error*/);

  void content_walker();
  void generate_html();

  void save_html_file(const std::string &html_content,
                      const FMatter &front_matter, const std::string &md_file);
};

#endif  //! SS_GEN_H
