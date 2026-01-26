#ifndef SOLOPTIMA_LOGGER_H
#define SOLOPTIMA_LOGGER_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sol_logger {
    void *mutex; /* pthread_mutex_t*, opaque to keep header minimal */
} sol_logger_t;

int sol_logger_init(sol_logger_t *logger);
void sol_logger_shutdown(sol_logger_t *logger);

void sol_log_info(sol_logger_t *logger, const char *fmt, ...);
void sol_log_error(sol_logger_t *logger, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* SOLOPTIMA_LOGGER_H */
