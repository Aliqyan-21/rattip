#ifndef SERVER_H
#define SERVER_H

#include <atomic>
#include <string>

void serve(const std::string public_dir, int port = 8080,
           std::atomic<bool> *reload_flag = nullptr);

#endif  //! SERVER_H
