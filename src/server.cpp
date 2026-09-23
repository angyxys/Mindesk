#include "server.h"

#include <cstdlib>

namespace mindesk {

Server::Server() {
  wlr_log_init(WLR_DEBUG, nullptr);
  wlr_log(WLR_INFO, "Server creado");
}

Server::~Server() {
  if (allocator_) {
    wlr_allocator_destroy(allocator_);
    allocator_ = nullptr;
  }
  if (renderer_) {
    wlr_renderer_destroy(renderer_);
    renderer_ = nullptr;
  }
  if (backend_) {
    wlr_backend_destroy(backend_);
    backend_ = nullptr;
  }
  if (display_) {
    wl_display_destroy(display_);
    display_ = nullptr;
  }
  wlr_log(WLR_INFO, "Server Destruido");
}

bool Server::init() {
  display_ = wl_display_create();
  if (!display_) {
    wlr_log(WLR_ERROR, "No se pudo crear wl_display");
    return false;
  }

  wl_event_loop *loop = wl_display_get_event_loop(display_);
  backend_ = wlr_backend_autocreate(loop, nullptr);
  if (!backend_) {
    wlr_log(WLR_ERROR, "No se pudo crear el backend");
    return false;
  }

  renderer_ = wlr_renderer_autocreate(backend_);
  if (!renderer_) {
    wlr_log(WLR_ERROR, "No se pudo crear el Renderer");
    return false;
  }

  wlr_renderer_init_wl_display(renderer_, display_);

  allocator_ = wlr_allocator_autocreate(backend_, renderer_);
  if (!allocator_) {
    wlr_log(WLR_ERROR, "No se pudo crear el allocator");
    return false;
  }

  wlr_log(WLR_INFO, "Todos los recursos inicializados");
  return true;
}

void Server::run() {
  const char *socket = wl_display_add_socket_auto(display_);
  if (!socket) {
    wlr_log(WLR_ERROR, "No se pudo crear el socket Wayland. Comprueba XDG_RUNTIME_DIR.");
    return;
  }

  setenv("WAYLAND_DISPLAY", socket, 1);

  if (!wlr_backend_start(backend_)) {
    wlr_log(WLR_ERROR, "No se pudo arrancar el backend");
    return;
  }

  wlr_log(WLR_INFO, "  mindesk escuchando en WAYLAND_DISPLAY=%s", socket);

  wl_display_run(display_);
}

void Server::terminate() {
  if (display_) {
    wl_display_terminate(display_);
  }
}

}