#ifndef SOLOPTIMA_SIGNAL_HANDLER_H
#define SOLOPTIMA_SIGNAL_HANDLER_H

#include <signal.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sol_signal_handler {
    struct sigaction old_sigint;
    struct sigaction old_sigterm;
} sol_signal_handler_t;

/*
 * Week 2:
 * - Installs SIGINT/SIGTERM handlers that set `server_running = 0`.
 * - Uses only async-signal-safe operations inside the handlers.
 */
int sol_signal_handler_init(sol_signal_handler_t *handler);

void sol_signal_handler_shutdown(sol_signal_handler_t *handler);

/*
 * Global server lifecycle flag (Week 2 requirement).
 * - 1 while the server should keep running
 * - 0 when a termination signal was received
 */
extern volatile sig_atomic_t server_running;

#ifdef __cplusplus
}
#endif

#endif /* SOLOPTIMA_SIGNAL_HANDLER_H */
