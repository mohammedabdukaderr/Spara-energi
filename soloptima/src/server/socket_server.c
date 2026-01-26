#include "socket_server.h"

#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

static int sol_write_all(int fd, const void *buf, size_t len)
{
    const char *p = (const char *)buf;
    size_t off = 0;
    while (off < len) {
        ssize_t n = write(fd, p + off, len - off);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        if (n == 0) {
            return -1;
        }
        off += (size_t)n;
    }
    return 0;
}

int sol_socket_server_init(sol_socket_server_t *server, sol_logger_t *logger, const char *socket_path)
{
    if (!server || !socket_path) {
        return -1;
    }

    memset(server, 0, sizeof(*server));
    server->listen_fd = -1;
    server->logger = logger;
    (void)snprintf(server->socket_path, sizeof(server->socket_path), "%s", socket_path);

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        if (server->logger) {
            sol_log_errno(server->logger, "socket(AF_UNIX) failed");
        }
        return -1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;

    if (strlen(server->socket_path) >= sizeof(addr.sun_path)) {
        if (server->logger) {
            sol_log_error(server->logger, "Socket path too long: %s", server->socket_path);
        }
        close(fd);
        errno = ENAMETOOLONG;
        return -1;
    }
    (void)snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", server->socket_path);

    /* Remove stale socket path if present (previous unclean shutdown). */
    (void)unlink(server->socket_path);

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        if (server->logger) {
            sol_log_errno(server->logger, "bind(/tmp/soloptima.sock) failed");
        }
        close(fd);
        return -1;
    }

    if (listen(fd, 16) != 0) {
        if (server->logger) {
            sol_log_errno(server->logger, "listen() failed");
        }
        close(fd);
        (void)unlink(server->socket_path);
        return -1;
    }

    server->listen_fd = fd;
    if (server->logger) {
        sol_log_info(server->logger, "Socket server listening on %s", server->socket_path);
    }
    return 0;
}

int sol_socket_server_accept_once(sol_socket_server_t *server, int timeout_ms)
{
    if (!server || server->listen_fd < 0) {
        return -1;
    }

    struct pollfd pfd;
    memset(&pfd, 0, sizeof(pfd));
    pfd.fd = server->listen_fd;
    pfd.events = POLLIN;

    int prc;
    do {
        prc = poll(&pfd, 1, timeout_ms);
    } while (prc < 0 && errno == EINTR);

    if (prc == 0) {
        return 0; /* timeout */
    }
    if (prc < 0) {
        if (server->logger) {
            sol_log_errno(server->logger, "poll(listen_fd) failed");
        }
        return -1;
    }

    int client_fd;
    do {
        client_fd = accept(server->listen_fd, NULL, NULL);
    } while (client_fd < 0 && errno == EINTR);

    if (client_fd < 0) {
        if (server->logger) {
            sol_log_errno(server->logger, "accept() failed");
        }
        return -1;
    }

    if (server->logger) {
        sol_log_info(server->logger, "Client connected");
    }

    char buf[1024];
    ssize_t nread;
    do {
        nread = read(client_fd, buf, sizeof(buf));
    } while (nread < 0 && errno == EINTR);

    if (nread < 0) {
        if (server->logger) {
            sol_log_errno(server->logger, "read(client) failed");
        }
        close(client_fd);
        return -1;
    }

    /* Week 2 foundation: ignore command content, return static response. */
    const char reply[] = "OK: SolOptima server alive\n";
    if (sol_write_all(client_fd, reply, sizeof(reply) - 1) != 0) {
        if (server->logger) {
            sol_log_errno(server->logger, "write(client) failed");
        }
        close(client_fd);
        return -1;
    }

    close(client_fd);
    if (server->logger) {
        sol_log_info(server->logger, "Client disconnected");
    }

    return 1;
}

void sol_socket_server_shutdown(sol_socket_server_t *server)
{
    if (!server) {
        return;
    }

    if (server->listen_fd >= 0) {
        close(server->listen_fd);
        server->listen_fd = -1;
    }

    if (server->socket_path[0] != '\0') {
        (void)unlink(server->socket_path);
    }

    if (server->logger) {
        sol_log_info(server->logger, "Socket server shutdown complete");
    }

    server->listen_fd = -1;
    server->socket_path[0] = '\0';
    server->logger = NULL;
}
