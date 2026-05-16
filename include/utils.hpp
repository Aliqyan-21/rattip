#ifndef UTILS_HPP
#define UTILS_HPP

#include <error.h>
#include <fstream>
#include <iostream>
#include "argh.h"

struct Args {
  std::string main_dir{"content"};
  std::string public_dir{"public"};
  std::string assets_dir{"assets"};
  std::string theme_dir{"themes"};
  std::string theme_name{"dark"};
  bool        force{false};
  bool        serve{false};
  int         port{8080};
  bool        verbose{false};
};

inline Args parse(int argc, char *argv[]) {
  Args         args;
  argh::parser cmdl;
  cmdl.add_params({"--main", "-m", "--public", "-p", "--assets", "-a",
                   "--theme-dir", "--theme", "-t", "--force", "-f", "--serve",
                   "-s", "--port", "-p", "--verbose", "-v"});
  cmdl.parse(argc, argv, argh::parser::SINGLE_DASH_IS_MULTIFLAG);
  //todo: fill args
}

/* verbosity
 give program verbosity based on the verbose variable */
inline bool verbose = false;
template <typename... Args>
static inline void V66V(Args &&...args) {
  if (verbose) {
    std::clog << "[VERB] ";
    (std::clog << ... << args) << std::endl;
  }
}

/* load file and return its contents as std::string */
inline std::string load_file(const std::string &filepath) {
  std::ifstream inf(filepath);
  if (!inf) { throw RuntimeError("File not found: " + filepath); }
  std::string content((std::istreambuf_iterator<char>(inf)),
                      std::istreambuf_iterator<char>());
  inf.close();
  V66V("File '", filepath, "' loaded successfully.");
  return content;
}

inline std::string ltrim(const std::string &str) {
  size_t i = 0;
  while (i < str.size() && isspace(str[i])) { i++; }
  return str.substr(i);
}
inline std::string rtrim(const std::string &str) {
  size_t j = str.size();
  while (j > 0 && isspace(str[j - 1])) { j--; }
  return str.substr(0, j);
}

inline std::string trim(const std::string &str) { return ltrim(rtrim(str)); }

#endif  //! UTILS_HPP
