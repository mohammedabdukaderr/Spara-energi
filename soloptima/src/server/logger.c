#include "logger.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void sol_log_v_unlocked(const char *level, const char *fmt, va_list args)
{
    if (!level || !fmt) {
        return;
    }

    /* Week 1: stderr-only logging (no file I/O). */
    time_t now = time(NULL);
    struct tm tm_now;
    (void)localtime_r(&now, &tm_now);

    char ts[32];
    (void)strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", &tm_now);

    fprintf(stderr, "%s [%s] ", ts, level);
    vfprintf(stderr, fmt, args);
    fputc('\n', stderr);
}

static void sol_log_v(sol_logger_t *logger, const char *level, const char *fmt, va_list args)
{
    if (!logger || !logger->mutex || !fmt) {
        return;
    }

    pthread_mutex_t *mtx = (pthread_mutex_t *)logger->mutex;
    pthread_mutex_lock(mtx);
    sol_log_v_unlocked(level, fmt, args);
    pthread_mutex_unlock(mtx);
}

int sol_logger_init(sol_logger_t *logger)
{
    if (!logger) {
        return -1;
    }

    pthread_mutex_t *mtx = (pthread_mutex_t *)calloc(1, sizeof(*mtx));
    if (!mtx) {
        return -1;
    }

    if (pthread_mutex_init(mtx, NULL) != 0) {
        free(mtx);
        return -1;
    }

    logger->mutex = mtx;
    return 0;
}

void sol_logger_shutdown(sol_logger_t *logger)
{
    if (!logger || !logger->mutex) {
        return;
    }

    pthread_mutex_t *mtx = (pthread_mutex_t *)logger->mutex;
    pthread_mutex_destroy(mtx);
    free(mtx);
    logger->mutex = NULL;
}

void sol_log_info(sol_logger_t *logger, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    sol_log_v(logger, "INFO", fmt, args);
    va_end(args);
}

void sol_log_error(sol_logger_t *logger, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    sol_log_v(logger, "ERROR", fmt, args);
    va_end(args);
}

void sol_log_errno(sol_logger_t *logger, const char *context)
{
    if (!logger || !logger->mutex || !context) {
        return;
    }

    int err = errno;
    pthread_mutex_t *mtx = (pthread_mutex_t *)logger->mutex;
    pthread_mutex_lock(mtx);
    /* Avoid stdarg ceremony by printing directly under lock. */
    time_t now = time(NULL);
    struct tm tm_now;
    (void)localtime_r(&now, &tm_now);

    char ts[32];
    (void)strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", &tm_now);
    fprintf(stderr, "%s [ERROR] %s: %s\n", ts, context, strerror(err));
    pthread_mutex_unlock(mtx);
}
