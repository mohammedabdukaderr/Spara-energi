#include "server.h"

#include "logger.h"
#include "signal_handler.h"
#include "socket_server.h"

#include <stdio.h>
#include <unistd.h>

static int spawn_fetcher_process(sol_logger_t *logger)
{
    (void)logger;

    /* TODO (Week 2): fork() and exec fetcher executable; set up pipes if needed. */
    return 0;
}

int main(void)
{
    sol_logger_t logger;
    sol_signal_handler_t sig;
    sol_socket_server_t socket_server;

    if (sol_logger_init(&logger) != 0) {
        fprintf(stderr, "[soloptima] failed to init logger\n");
        return 1;
    }

    sol_log_info(&logger, "SolOptima server starting up (Week 1 skeleton)");

    if (sol_signal_handler_init(&sig) != 0) {
        sol_log_error(&logger, "Failed to init signal handling (SIGINT/SIGTERM)");
        sol_logger_shutdown(&logger);
        return 1;
    }

    /* Placeholder: server socket startup */
    if (sol_socket_server_init(&socket_server, "/tmp/soloptima.sock") != 0) {
        sol_log_error(&logger, "Failed to init socket server");
        sol_signal_handler_shutdown(&sig);
        sol_logger_shutdown(&logger);
        return 1;
    }

    /* Placeholder: spawn fetcher process */
    if (spawn_fetcher_process(&logger) != 0) {
        sol_log_error(&logger, "Failed to spawn fetcher process");
        /* Week 1: continue anyway */
    }

    sol_log_info(&logger, "Server running; press Ctrl+C to stop");

    for (;;) {
        int term = sol_signal_handler_poll_terminate(&sig, 250);
        if (term < 0) {
            sol_log_error(&logger, "Signal polling error; shutting down");
            break;
        }
        if (term == 1) {
            sol_log_info(&logger, "Termination signal received");
            break;
        }

        /* TODO (Week 2): accept clients, dispatch to worker threads, etc. */
        (void)sol_socket_server_accept_once(&socket_server);
        (void)usleep(10 * 1000);
    }

    sol_log_info(&logger, "SolOptima server shutting down");

    sol_socket_server_shutdown(&socket_server);
    sol_signal_handler_shutdown(&sig);
    sol_logger_shutdown(&logger);

    return 0;
}
