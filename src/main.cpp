#include <thread>
#include "server.h"
#include "ss_gen.h"
#include "utils.hpp"

int main(int argc, char *argv[]) {
  Args args = parse(argc, argv);

  verbose = args.verbose;

  SSGen ssg(args.main_dir, args.public_dir, args.assets_dir);
  try {
    if (!args.no_gen) {
      ssg.load_templates();
      ssg.init_theme(args.theme_name, args.theme_dir);
      if (args.force) { ssg.set_force(); }
      ssg.generate_site();
    }

    if (args.serve) {
      std::atomic<bool> reload_flag{false};
      std::thread       watcher([&]() {
        ssg.watch_and_regen(reload_flag);
        reload_flag = true;
      });
      watcher.detach();
      serve(args.public_dir, args.port, &reload_flag);
    }
  } catch (const RappitError &e) { std::cerr << e.format() << std::endl; }

  return 0;
}
