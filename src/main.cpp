#include <thread>
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

  std::thread watcher([&ssgen]() { ssgen.watch_and_regen(); });

  serve("public", 2020);
  watcher.join();
  return 0;
}
