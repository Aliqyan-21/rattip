#ifndef ERROR_H
#define ERROR_H

#include <stdexcept>

class RappitError : public std::exception {
public:
  std::string msg_;
  std::string ctx_;

  const char *what() const noexcept override;

  RappitError(const std::string &msg, const std::string &ctx = "");

  virtual std::string format() const;
};

class RuntimeError : public RappitError {
public:
  explicit RuntimeError(const std::string &msg, const std::string &ctx = "");
};

class ServerError : public RappitError {
public:
  explicit ServerError(const std::string &msg, const std::string &ctx = "");
};

#endif  //! ERROR_H
