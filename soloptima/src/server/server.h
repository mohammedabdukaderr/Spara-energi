
#ifndef SOLOPTIMA_SERVER_H
#define SOLOPTIMA_SERVER_H

/*
 * SolOptima Server (Week 1 skeleton)
 *
 * This header intentionally exposes only minimal types.
 * TODO (Week 2): expand with real configuration + server lifecycle APIs.
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sol_server_config {
	const char *socket_path;
	/* TODO (Week 2): add log level, worker limits, cache paths, etc. */
} sol_server_config_t;

#ifdef __cplusplus
}
#endif

#endif /* SOLOPTIMA_SERVER_H */

