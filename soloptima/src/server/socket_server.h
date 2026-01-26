#ifndef SOLOPTIMA_SOCKET_SERVER_H
#define SOLOPTIMA_SOCKET_SERVER_H

#include "logger.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sol_socket_server {
    int listen_fd;
    char socket_path[108]; /* UNIX_PATH_MAX on Linux */
    sol_logger_t *logger;   /* non-owning */
} sol_socket_server_t;

/* Week 1: stubs only. */
int sol_socket_server_init(sol_socket_server_t *server, sol_logger_t *logger, const char *socket_path);

/*
 * Accept at most one client.
 * Returns:
 *  1 if a client was accepted and handled
 *  0 if timed out waiting for a client
 * -1 on error
 */
int sol_socket_server_accept_once(sol_socket_server_t *server, int timeout_ms);
void sol_socket_server_shutdown(sol_socket_server_t *server);

#ifdef __cplusplus
}
#endif

#endif /* SOLOPTIMA_SOCKET_SERVER_H */
