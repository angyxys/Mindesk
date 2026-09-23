#include "server.h"

#include <csignal>
#include <cstdlib>

static mindesk::Server *g_server = nullptr;

static void on_signal(int signo) {
  if (g_server) {
    g_server->terminate();
  } else {
    _Exit(128 + signo);
  }
}

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  struct sigaction sa{};

  sa.sa_handler = on_signal;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  sigaction(SIGINT, &sa, nullptr);
  sigaction(SIGTERM, &sa, nullptr);
  signal(SIGPIPE, SIG_IGN);

  mindesk::Server server;
  g_server = &server;

  if (!server.init()) {
    return EXIT_FAILURE;
  }

  server.run();

  g_server = nullptr;
  
  return EXIT_SUCCESS;
}
