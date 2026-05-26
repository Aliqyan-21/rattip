#include <thread>
#include "rattip_init.hpp"
#include "ss_gen.h"
#include "utils.hpp"

extern "C" void serve(const char *public_dir, int port, int *reload_gen);

int main(int argc, char *argv[]) {
  Args args = parse(argc, argv);

  verbose = args.verbose;

  SSGen ssg(args.main_dir, args.public_dir, args.templates_dir,
            args.assets_dir);
  try {
    if (args.init) {
      rattip_init();
      return 0;
    }
    if (!args.no_gen) {
      if (args.force) { ssg.set_force(); }
      ssg.load_templates();
      ssg.init_theme(args.theme_name, args.theme_dir);
      ssg.load_assets();
      ssg.generate_site();
    }

    if (args.serve) {
      std::atomic<int> reload_gen{0};
      std::thread      watcher([&]() { ssg.watch_and_regen(reload_gen); });
      watcher.detach();
      serve(args.public_dir.c_str(), args.port,
            reinterpret_cast<int *>(&reload_gen));
    }
  } catch (const RappitError &e) { std::cerr << e.format() << std::endl; }

  return 0;
}
