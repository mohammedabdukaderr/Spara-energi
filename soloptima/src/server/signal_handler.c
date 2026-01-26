#include "signal_handler.h"

volatile sig_atomic_t server_running = 1;

#include <errno.h>
#include <string.h>

static void sol_server_signal_handler(int signo)
{
    (void)signo;
    server_running = 0;
}

int sol_signal_handler_init(sol_signal_handler_t *handler)
{
    if (!handler) {
        return -1;
    }

    memset(&handler->old_sigint, 0, sizeof(handler->old_sigint));
    memset(&handler->old_sigterm, 0, sizeof(handler->old_sigterm));

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sol_server_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, &handler->old_sigint) != 0) {
        return -1;
    }

    if (sigaction(SIGTERM, &sa, &handler->old_sigterm) != 0) {
        int saved = errno;
        (void)sigaction(SIGINT, &handler->old_sigint, NULL);
        errno = saved;
        return -1;
    }

    return 0;
}

void sol_signal_handler_shutdown(sol_signal_handler_t *handler)
{
    if (!handler) {
        return;
    }

    (void)sigaction(SIGINT, &handler->old_sigint, NULL);
    (void)sigaction(SIGTERM, &handler->old_sigterm, NULL);
}
