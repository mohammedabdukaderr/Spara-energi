#include "server.h"

#include "logger.h"
#include "signal_handler.h"
#include "socket_server.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

static pid_t spawn_fetcher_process(sol_logger_t *logger)
{
    pid_t pid = fork();
    if (pid < 0) {
        if (logger) {
            sol_log_errno(logger, "fork() for fetcher failed");
        }
        return -1;
    }

    if (pid == 0) {
        /* Child: placeholder fetcher process (Week 2 foundation). */
        /* TODO (Week 3+): exec real fetcher executable and set up IPC. */
        for (;;) {
            pause();
        }
        _exit(0);
    }

    if (logger) {
        sol_log_info(logger, "Fetcher spawned (pid=%ld)", (long)pid);
    }
    return pid;
}

static void log_wait_status(sol_logger_t *logger, const char *label, int status)
{
    if (!logger || !label) {
        return;
    }

    if (WIFEXITED(status)) {
        sol_log_info(logger, "%s exited (code=%d)", label, WEXITSTATUS(status));
        return;
    }
    if (WIFSIGNALED(status)) {
        sol_log_info(logger, "%s killed by signal %d", label, WTERMSIG(status));
        return;
    }
    sol_log_info(logger, "%s changed state (status=0x%x)", label, status);
}

int main(void)
{
    sol_logger_t logger;
    sol_signal_handler_t sig;
    sol_socket_server_t socket_server;
    pid_t fetcher_pid = -1;

    server_running = 1;

    if (sol_logger_init(&logger) != 0) {
        fprintf(stderr, "[soloptima] failed to init logger\n");
        return 1;
    }

    sol_log_info(&logger, "SolOptima server starting up (Week 2 foundation)");

    if (sol_signal_handler_init(&sig) != 0) {
        sol_log_errno(&logger, "Failed to init signal handling (SIGINT/SIGTERM)");
        sol_logger_shutdown(&logger);
        return 1;
    }

    if (sol_socket_server_init(&socket_server, &logger, "/tmp/soloptima.sock") != 0) {
        sol_log_error(&logger, "Failed to init socket server");
        sol_signal_handler_shutdown(&sig);
        sol_logger_shutdown(&logger);
        return 1;
    }

    fetcher_pid = spawn_fetcher_process(&logger);
    if (fetcher_pid < 0) {
        sol_log_error(&logger, "Fetcher spawn failed; continuing without fetcher");
    }

    sol_log_info(&logger, "Server running; press Ctrl+C to stop");

    while (server_running) {
        /* Check child status (fetcher crash detection + zombie prevention). */
        if (fetcher_pid > 0) {
            int status = 0;
            pid_t rc = waitpid(fetcher_pid, &status, WNOHANG);
            if (rc == fetcher_pid) {
                log_wait_status(&logger, "Fetcher", status);
                sol_log_error(&logger, "Fetcher exited unexpectedly (TODO: restart policy)");
                fetcher_pid = -1;
            } else if (rc < 0 && errno != EINTR) {
                sol_log_errno(&logger, "waitpid(fetcher) failed");
            }
        }

        int arc = sol_socket_server_accept_once(&socket_server, 250);
        if (arc < 0) {
            if (!server_running) {
                break;
            }
            sol_log_errno(&logger, "Socket server accept failed");
        }
    }

    sol_log_info(&logger, "Shutdown requested; stopping server");

    if (fetcher_pid > 0) {
        sol_log_info(&logger, "Terminating fetcher (pid=%ld)", (long)fetcher_pid);
        if (kill(fetcher_pid, SIGTERM) != 0) {
            sol_log_errno(&logger, "kill(fetcher, SIGTERM) failed");
        }

        int status = 0;
        int waited = 0;
        for (;;) {
            pid_t wrc = waitpid(fetcher_pid, &status, 0);
            if (wrc == fetcher_pid) {
                waited = 1;
                break;
            }
            if (wrc < 0 && errno == EINTR) {
                continue;
            }
            if (wrc < 0) {
                sol_log_errno(&logger, "waitpid(fetcher) during shutdown failed");
            }
            break;
        }

        if (waited) {
            log_wait_status(&logger, "Fetcher", status);
        }
        fetcher_pid = -1;
    }

    /* Reap any other children (defensive, avoids zombies). */
    for (;;) {
        int status = 0;
        pid_t rc = waitpid(-1, &status, WNOHANG);
        if (rc <= 0) {
            break;
        }
        log_wait_status(&logger, "Child", status);
    }

    sol_log_info(&logger, "SolOptima server shutting down");

    sol_socket_server_shutdown(&socket_server);
    sol_signal_handler_shutdown(&sig);
    sol_logger_shutdown(&logger);

    return 0;
}
