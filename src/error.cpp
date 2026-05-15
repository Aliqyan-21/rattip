#include "error.h"
#include <sstream>

std::string RappitError::format() const {
  std::stringstream ss;
  ss << msg_ << std::endl;
  if (!ctx_.empty()) { ss << "  " << ctx_ << std::endl; }
  return ss.str();
}

RappitError::RappitError(const std::string &msg, const std::string &ctx)
  : msg_(msg), ctx_(ctx) {}
const char *RappitError::what() const noexcept { return msg_.c_str(); }

RuntimeError::RuntimeError(const std::string &msg, const std::string &ctx)
  : RappitError("[Runtime Error] " + msg, ctx) {}

ServerError::ServerError(const std::string &msg, const std::string &ctx)
  : RappitError("[Server Error] " + msg, ctx) {}
