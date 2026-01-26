#ifndef SOLOPTIMA_SOCKET_SERVER_H
#define SOLOPTIMA_SOCKET_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sol_socket_server {
    int listen_fd;
    char socket_path[108]; /* UNIX_PATH_MAX on Linux */
} sol_socket_server_t;

/* Week 1: stubs only. */
int sol_socket_server_init(sol_socket_server_t *server, const char *socket_path);
int sol_socket_server_accept_once(sol_socket_server_t *server);
void sol_socket_server_shutdown(sol_socket_server_t *server);

#ifdef __cplusplus
}
#endif

#endif /* SOLOPTIMA_SOCKET_SERVER_H */
