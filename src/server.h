#pragma once

extern "C" {
#include <wayland-server-core.h>
#include <wlr/backend.h>
#include <wlr/render/allocator.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/util/log.h>
}

namespace mindesk {

class Server {
 public:
  Server();
  ~Server();

  Server(const Server&) = delete;
  Server& operator=(const Server&) = delete;

  bool init();

  void run();

  void terminate();

 private:
  wl_display* display_ = nullptr;
  wlr_backend* backend_ = nullptr;
  wlr_renderer* renderer_ = nullptr;
  wlr_allocator* allocator_ = nullptr;
};

}
