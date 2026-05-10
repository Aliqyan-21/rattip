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
  std::unordered_map<std::string, std::string>
    config;  // [html tags : class names]
  std::unordered_map<std::string, std::string>
    templates;  // [template (page/blog) : loaded html]
};

class SSGen {
public:
  SSGen(const std::string &main_folder_path   = "content",
        const std::string &public_folder_path = "public");

  void generate_site();
  void init_theme(const std::string &name, const std::string &dir,
                  const std::string &conf_file);

private:
  std::string main_folder_;
  std::string public_folder_; /* final html will be generated to be served */
  std::vector<std::string> md_files_;
  Theme                    theme_;

  /* templates */
  std::string base_;

  FMatter parse_front_matter(std::string &content);

  void content_walker();
  void generate_html();

  void save_html_file(const std::string &html_content,
                      const FMatter &front_matter, const std::string &md_file);

  void parse_config_file(const std::string &config_file);
};

#endif  //! SS_GEN_H
