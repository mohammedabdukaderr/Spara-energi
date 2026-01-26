#include "signal_handler.h"

#include <errno.h>
#include <poll.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#ifdef __linux__
#include <sys/signalfd.h>
#endif

int sol_signal_handler_init(sol_signal_handler_t *handler)
{
    if (!handler) {
        return -1;
    }

    handler->fd = -1;

    sigemptyset(&handler->mask);
    sigaddset(&handler->mask, SIGINT);
    sigaddset(&handler->mask, SIGTERM);

    /* Block signals so they are delivered via signalfd() (Linux). */
    if (pthread_sigmask(SIG_BLOCK, &handler->mask, NULL) != 0) {
        return -1;
    }

#ifdef __linux__
    handler->fd = signalfd(-1, &handler->mask, SFD_CLOEXEC);
    if (handler->fd < 0) {
        return -1;
    }
#else
    /* TODO: non-Linux fallback (e.g., sigaction + self-pipe). */
    (void)handler;
    return -1;
#endif

    return 0;
}

int sol_signal_handler_poll_terminate(sol_signal_handler_t *handler, int timeout_ms)
{
    if (!handler || handler->fd < 0) {
        errno = EINVAL;
        return -1;
    }

    struct pollfd pfd;
    memset(&pfd, 0, sizeof(pfd));
    pfd.fd = handler->fd;
    pfd.events = POLLIN;

    int rc = poll(&pfd, 1, timeout_ms);
    if (rc == 0) {
        return 0; /* timeout */
    }
    if (rc < 0) {
        return -1;
    }

#ifdef __linux__
    struct signalfd_siginfo info;
    ssize_t n = read(handler->fd, &info, sizeof(info));
    if (n != (ssize_t)sizeof(info)) {
        return -1;
    }

    if (info.ssi_signo == SIGINT || info.ssi_signo == SIGTERM) {
        return 1;
    }

    return 0;
#else
    return 0;
#endif
}

void sol_signal_handler_shutdown(sol_signal_handler_t *handler)
{
    if (!handler) {
        return;
    }

    if (handler->fd >= 0) {
        close(handler->fd);
        handler->fd = -1;
    }

    /* TODO: optionally restore previous signal mask */
}
