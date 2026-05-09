#ifndef SS_GEN_H
#define SS_GEN_H

#include <string>

class SSGen {
public:
  SSGen(const std::string &main_folder_path);

  void content_walker();

private:
  std::string main_folder_;
};

#endif  //! SS_GEN_H
