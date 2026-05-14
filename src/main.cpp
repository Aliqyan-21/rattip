#include "ss_gen.h"
#include "utils.hpp"

int main(void) {
  verbose = true;
  SSGen ssgen;
  try {
    ssgen.init_theme("default", "theme");
    // ssgen.set_force();
    ssgen.generate_site();
  } catch (const std::exception &e) { std::cerr << e.what() << std::endl; }
  return 0;
}
