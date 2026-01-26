
CC := gcc

CFLAGS := -Wall -Wextra -pthread

SERVER_BIN := soloptima_server
SERVER_SRCS := \
	src/server/server_main.c \
	src/server/logger.c \
	src/server/signal_handler.c \
	src/server/socket_server.c

.PHONY: all server run-server clean

all: server

server: $(SERVER_BIN)

$(SERVER_BIN): $(SERVER_SRCS)
	$(CC) $(CFLAGS) -o $@ $(SERVER_SRCS)

run-server: $(SERVER_BIN)
	./$(SERVER_BIN)

clean:
	rm -f $(SERVER_BIN)

