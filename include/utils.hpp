#ifndef UTILS_HPP
#define UTILS_HPP

#include <fstream>
#include <iostream>

/* verbosity
 give program verbosity based on the verbose variable */
static inline bool verbose = false;
template <typename... Args>
static inline void V66V(Args &&...args) {
  if (verbose) { (std::cout << ... << args) << std::endl; }
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

inline std::string ltrim(const std::string &str) {
  size_t i = 0;
  while (i < str.size() && isspace(str[i])) { i++; }
  return str.substr(i);
}
inline std::string rtrim(const std::string &str) {
  size_t j = str.size();
  while (j > str.size() && !isspace(str[j - 1])) { j--; }
  return str.substr(0, j);
}

inline std::string trim(const std::string &str) { return ltrim(rtrim(str)); }

#endif  //! UTILS_HPP
