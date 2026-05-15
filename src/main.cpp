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

    std::atomic<bool> reload_flag = false;

    std::thread watcher([&]() {
      ssgen.watch_and_regen(reload_flag);
      reload_flag = true;
    });
    watcher.detach();

    serve("public", 2020, &reload_flag);
  } catch (const RappitError &e) { std::cerr << e.format() << std::endl; }

  return 0;
}
