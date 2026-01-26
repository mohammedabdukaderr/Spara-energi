
# SolOptima


This repository is developed in weekly increments. Week 2 focuses strictly on the **server foundation and lifecycle** (startup, shutdown, process management, and a minimal IPC entrypoint) and intentionally avoids implementing business logic.

## Week 1 (Skeleton)

Week 1 provides a compiling baseline with clear module boundaries.

### Server-related files (Week 1)

- `src/server/server_main.c`
	- Program entrypoint (`main`).
	- Wires together logging, signal handling, and socket server modules.
	- Runs a placeholder loop to prove the server binary builds and exits cleanly.

- `src/server/logger.h`, `src/server/logger.c`
	- Thread-safe logging to `stderr` using a `pthread_mutex`.
	- Central place for lifecycle and error logs.

- `src/server/signal_handler.h`, `src/server/signal_handler.c`
	- Signal handling skeleton.
	- (Week 1 version used a Linux-specific approach; Week 2 replaces it with the required `server_running` model.)

- `src/server/socket_server.h`, `src/server/socket_server.c`
	- IPC/server socket API skeleton (stubs only in Week 1).

- `src/server/server.h`
	- Minimal server “public” types (e.g. config placeholder).

- `Makefile`
	- Builds the server with `gcc -Wall -Wextra -pthread`.

## Week 2 (Server foundation + lifecycle)

Week 2 implements a robust server foundation that:
- starts cleanly
- shuts down cleanly
- manages child processes correctly (no zombies)
- exposes a minimal Unix domain socket endpoint for later IPC work

No real business logic, API work, threading pipelines, or shared memory is implemented in Week 2.

### What each file does (Week 2)

- `src/server/signal_handler.h`, `src/server/signal_handler.c`
	- Implements POSIX `sigaction()` handling for `SIGINT` and `SIGTERM`.
	- Introduces the required global lifecycle flag:
		- `volatile sig_atomic_t server_running` (1 = keep running, 0 = shutdown requested)
	- The signal handler does only one async-signal-safe thing: sets `server_running = 0`.

- `src/server/logger.h`, `src/server/logger.c`
	- Thread-safe logging via `pthread_mutex`.
	- Logs lifecycle events (startup/shutdown, socket events, fetcher events).
	- Adds `sol_log_errno(logger, context)` for syscall failures with `strerror(errno)`.

- `src/server/socket_server.h`, `src/server/socket_server.c`
	- Unix domain socket server foundation:
		- creates an `AF_UNIX` stream socket
		- binds and listens on `/tmp/soloptima.sock`
		- unlinks stale socket path before binding
		- accepts **one client at a time**
		- reads a command (ignored for now)
		- replies with a static message:
			- `OK: SolOptima server alive\n`
		- logs client connect/disconnect and socket lifecycle
		- cleans up on shutdown (close + unlink)

- `src/server/server_main.c`
	- Real server lifecycle orchestration:
		- startup sequence: logger → signals → socket listener → spawn fetcher
		- main loop: `while (server_running)`
			- checks fetcher status with `waitpid(WNOHANG)` to detect crashes and avoid zombies
			- accepts/handles one socket client per loop iteration
		- shutdown sequence:
			- stops loop when Ctrl+C/SIGTERM occurs
			- terminates fetcher with `SIGTERM` and reaps it with `waitpid()`
			- reaps any remaining children defensively
			- closes/unlinks socket and shuts down subsystems

- `src/server/server.h`
	- Remains intentionally minimal; used for shared types/config placeholders without exposing internals.

- `Makefile`
	- Builds the runnable server binary as `./server`.

## Build and run

Build:

```sh
make clean
make
```

Run:

```sh
./server
```

Stop:

```text
Ctrl+C
```

## Quick socket test (Linux/WSL)

In another terminal, connect to the Unix socket and send any text:

```sh
printf "PING\n" | nc -U /tmp/soloptima.sock
```

Expected response:

```text
OK: SolOptima server alive
```

