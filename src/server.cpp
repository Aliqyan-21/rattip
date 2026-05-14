#include "server.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <sstream>

std::string get_mime_type(const std::string &path) {
  if (path.ends_with(".html")) { return "text/html"; }
  if (path.ends_with(".css")) { return "text/css"; }
  if (path.ends_with(".js")) { return "application/javascript"; }
  if (path.ends_with(".png")) { return "image/png"; }
  if (path.ends_with(".jpg") || path.ends_with(".jpeg")) {
    return "image/jpeg";
  }
  if (path.ends_with(".ico")) { return "image/x-icon"; }
  if (path.ends_with(".svg")) { return "image/svg+xml"; }
  return "text/plain";
}

void serve(const std::string public_dir, int port) {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    std::cerr << "Socket failed!" << std::endl;
    return;
  }

  int opt = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  sockaddr_in sin = {AF_INET, htons(port), {INADDR_ANY}};
  if (bind(fd, (sockaddr *)&sin, sizeof(sin)) == -1) {
    std::cerr << "Bind failed!" << std::endl;
    return;
  }

  if (listen(fd, 10) == -1) {
    std::cerr << "Listen failed!" << std::endl;
    return;
  }

  std::clog << "Serving on http://localhost:" << port << std::endl;

  while (true) {
    int cfd = accept(fd, nullptr, nullptr);
    if (cfd == -1) { continue; }

    char buffer[4096] = {};
    int  rn           = recv(cfd, buffer, sizeof(buffer), 0);
    if (rn <= 0) {
      close(cfd);
      continue;
    }

    std::stringstream req(std::string(buffer, rn));
    std::string       method, path;
    req >> method >> path;

    if (path == "/") { path = "/index.html"; }

    std::string   fpath = public_dir + path;
    std::ifstream inf(fpath, std::ios::binary);

    if (!inf) {
      std::string body = "404 Not Found";
      std::string response =
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: " +
        std::to_string(body.size()) +
        "\r\n"
        "\r\n" +
        body;
      send(cfd, response.c_str(), response.size(), 0);
    } else {
      std::string content((std::istreambuf_iterator<char>(inf)),
                          std::istreambuf_iterator<char>());
      std::string mime = get_mime_type(fpath);
      std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: " +
        mime +
        "\r\n"
        "Content-Length: " +
        std::to_string(content.size()) +
        "\r\n"
        "\r\n" +
        content;
      size_t sent = 0;
      while (sent < response.size()) {
        int n = send(cfd, response.c_str(), response.size(), 0);
        if (n <= 0) { break; }
        sent += n;
      }
    }
    close(cfd);
  }

  close(fd);
}
