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

  std::atomic<bool> reload_flag = false;

  std::thread watcher([&]() {
    ssgen.watch_and_regen(reload_flag);
    reload_flag = true;
  });

  serve("public", 2020, &reload_flag);
  watcher.join();
  return 0;
}
