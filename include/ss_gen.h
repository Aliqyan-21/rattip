#ifndef SS_GEN_H
#define SS_GEN_H

#include <string>
#include <unordered_map>
#include <vector>

struct FMatter {
  std::string title;
  std::string date;
  std::string tmpl;
};

struct Theme {
  std::string name{"default"};
  std::string theme_dir{"themes/default"};
};

class SSGen {
public:
  SSGen(const std::string &main_dir_path   = "content",
        const std::string &public_dir_path = "public",
        const std::string &assets_dir      = "assets");

  void generate_site();
  void init_theme(const std::string &name);
  void set_force();

private:
  std::string main_dir_;
  std::string public_dir_; /* final html will be generated to be served */
  std::string assets_dir_;
  std::vector<std::string>                     md_files_;
  Theme                                        theme_;
  /* templates */
  std::unordered_map<std::string, std::string> templates_;

  bool force_{false};  // force generation of all files

  void load_templates();

  FMatter parse_front_matter(std::string &content);

  void content_walker();
  void generate_html();

  void save_html_file(const std::string &html_content,
                      const FMatter &front_matter, const std::string &md_file);
};

#endif  //! SS_GEN_H
