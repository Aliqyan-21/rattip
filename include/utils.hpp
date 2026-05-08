#ifndef UTILS_HPP
#define UTILS_HPP

#include <fstream>
#include <iostream>

/* verbosity
 give program verbosity based on the verbose variable */
static inline bool verbose = false;
template <typename... Args> static inline void V66V(Args &&...args) {
  if (verbose) {
    (std::cout << ... << args) << std::endl;
  }
}

/* load file and return its contents as std::string */
inline std::string load_file(const std::string &filepath) {
  std::ifstream inf(filepath);
  if (!inf) {
    std::cerr << "File not found: " << filepath << std::endl;
    exit(1);
  }
  std::string content((std::istreambuf_iterator<char>(inf)),
                      std::istreambuf_iterator<char>());
  inf.close();
  V66V("File '", filepath, "' loaded successfully.");
  return content;
}

#endif //! UTILS_HPP
