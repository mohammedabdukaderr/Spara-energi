#ifndef SOLOPTIMA_SIGNAL_HANDLER_H
#define SOLOPTIMA_SIGNAL_HANDLER_H

#include <signal.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sol_signal_handler {
    int fd;           /* signalfd() on Linux; -1 if unused */
    sigset_t mask;    /* blocked signals */
} sol_signal_handler_t;

/*
 * Week 1 skeleton:
 * - Prepares SIGINT/SIGTERM handling.
 * - Uses signalfd() on Linux to avoid global state in signal handlers.
 * TODO: add portability path for non-Linux systems if needed.
 */
int sol_signal_handler_init(sol_signal_handler_t *handler);

/*
 * Returns:
 *  1 if a termination signal was received
 *  0 if timed out
 * -1 on error
 */
int sol_signal_handler_poll_terminate(sol_signal_handler_t *handler, int timeout_ms);

void sol_signal_handler_shutdown(sol_signal_handler_t *handler);

#ifdef __cplusplus
}
#endif

#endif /* SOLOPTIMA_SIGNAL_HANDLER_H */
