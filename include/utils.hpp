#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>

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

struct FMatter {
  std::string title;
  std::string date;
};
/* something like this:
---
title: first blog
date: 1998-01-23
---
*/
static inline FMatter parse_front_matter(std::string &content) {
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

    key == "title" ? fm.title = val : key == "date" ? fm.date = val : "";
  }
  if (!start) { return fm; }
  content = std::string(std::istreambuf_iterator<char>(ss), {});
  V66V("Front Matter parsed successfully!\n");
  return fm;
}

static inline std::string get_filename_from_path(const std::string &path) {
  /* 'content/path/filename.md' => filename */

  std::string ff;
  size_t      pos = path.find_last_of("/");
  if (pos != std::string::npos) {
    ff = path.substr(pos + 1, (path.find(".") - 1) - pos);
  } else {
    //...
  }
  return ff;
}

#endif  //! UTILS_HPP
