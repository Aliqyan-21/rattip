#ifndef SERVER_H
#define SERVER_H

#include <atomic>
#include <string>

void serve(const std::string public_dir, int port,
           std::atomic<int> *reload_gen = nullptr);

#endif  //! SERVER_H
