#ifndef SS_GEN_H
#define SS_GEN_H

#include <string>
#include <vector>

struct FMatter {
  std::string title;
  std::string date;
};

class SSGen {
public:
  SSGen(const std::string &main_folder_path   = "content",
        const std::string &public_folder_path = "public");

  void generate_site();

private:
  std::string main_folder_;
  std::string public_folder_; /* final html will be generated to be served */
  std::vector<std::string> md_files_;

  /* templates */
  std::string base_;

  FMatter parse_front_matter(std::string &content);
  void    content_walker();
  void    generate_html();
  void save_html_file(const std::string &html_content,
                      const FMatter &front_matter, const std::string &md_file);
};

#endif  //! SS_GEN_H
