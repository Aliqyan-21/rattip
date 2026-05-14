#include "server.h"
#include "ss_gen.h"
#include "utils.hpp"

int main(void) {
  verbose = true;
  SSGen ssgen;
  try {
    ssgen.init_theme("dark");
    // ssgen.set_force();
    ssgen.generate_site();
  } catch (const std::exception &e) { std::cerr << e.what() << std::endl; }
  serve("public", 2020);
  return 0;
}
