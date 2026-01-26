#include "socket_server.h"

#include <stdio.h>
#include <string.h>

int sol_socket_server_init(sol_socket_server_t *server, const char *socket_path)
{
    if (!server || !socket_path) {
        return -1;
    }

    memset(server, 0, sizeof(*server));
    server->listen_fd = -1;
    (void)snprintf(server->socket_path, sizeof(server->socket_path), "%s", socket_path);

    /* TODO (Week 2): create AF_UNIX socket, bind(), listen() */
    return 0;
}

int sol_socket_server_accept_once(sol_socket_server_t *server)
{
    if (!server) {
        return -1;
    }

    /* TODO (Week 2): accept() one client and handle protocol */
    return 0;
}

void sol_socket_server_shutdown(sol_socket_server_t *server)
{
    if (!server) {
        return;
    }

    /* TODO (Week 2): close listen fd, unlink socket path */
    server->listen_fd = -1;
    server->socket_path[0] = '\0';
}
