# ==========================================
# Weather CLI Application Makefile
# macOS + Linux compatible
# ==========================================

# Detect OS
UNAME_S := $(shell uname -s)

# Compiler flags
CFLAGS = -Wall -Wextra -g -std=c17 -Wno-deprecated-non-prototype

# Base include paths
INCLUDES = -I./includes -I./src/libs/cJSON

# OpenSSL / compiler per OS
ifeq ($(UNAME_S),Darwin)
	CC = clang
	OPENSSL_PREFIX := $(shell brew --prefix openssl@3 2>/dev/null)
	INCLUDES += -I$(OPENSSL_PREFIX)/include
	LDFLAGS = -L$(OPENSSL_PREFIX)/lib -lssl -lcrypto
else
	CC = gcc
	INCLUDES += -I/usr/include/x86_64-linux-gnu
	LDFLAGS = -lssl -lcrypto
endif

# Project directories
SRC_DIR = projekt
BUILD_DIR = build
CJSON_DIR = src/libs/cJSON

# Source files
PROJECT_SOURCES := $(wildcard $(SRC_DIR)/*.c)
CJSON_SOURCES := $(wildcard $(CJSON_DIR)/*.c)

MAIN_SOURCE = main.c
SERVER_SOURCE = server_main.c

# Object files
OBJECTS := $(PROJECT_SOURCES:%.c=$(BUILD_DIR)/%.o)
OBJECTS += $(CJSON_SOURCES:%.c=$(BUILD_DIR)/%.o)

MAIN_OBJ := $(BUILD_DIR)/main.o
SERVER_OBJ := $(BUILD_DIR)/server_main.o

# Robinsfile HTTP server
ROBINS_SOURCES := \
	Robinsfile/HTTPServer.c \
	Robinsfile/HTTPServerConnection.c \
	Robinsfile/TCPServer.c \
	Robinsfile/TCPClient.c \
	Robinsfile/smw.c

ROBINS_OBJECTS := $(ROBINS_SOURCES:%.c=$(BUILD_DIR)/%.o)

# Targets
TARGET = Weather
SERVER_TARGET = WeatherServer

# ==========================================
# Build rules
# ==========================================

all: $(TARGET) $(SERVER_TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)/$(SRC_DIR)
	mkdir -p $(BUILD_DIR)/$(CJSON_DIR)
	mkdir -p $(BUILD_DIR)/Robinsfile

# Client build
$(TARGET): $(BUILD_DIR) $(MAIN_OBJ) $(OBJECTS)
	$(CC) $(MAIN_OBJ) $(OBJECTS) $(LDFLAGS) -o $@

# Server build
$(SERVER_TARGET): $(BUILD_DIR) $(SERVER_OBJ) $(OBJECTS) $(ROBINS_OBJECTS)
	$(CC) $(SERVER_OBJ) $(OBJECTS) $(ROBINS_OBJECTS) $(LDFLAGS) -o $@

# Generic compile rule
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# ==========================================
# Run helpers
# ==========================================

run: $(TARGET)
	./$(TARGET)

run-server: $(SERVER_TARGET)
	./$(SERVER_TARGET)

# ==========================================
# Cleanup
# ==========================================

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(SERVER_TARGET)

clean-all: clean
	rm -rf cache cities

# ==========================================
# Help
# ==========================================

help:
	@echo "=== Weather build system ==="
	@echo " make              - Build everything"
	@echo " make run          - Run Weather client"
	@echo " make run-server   - Run Weather server"
	@echo " make clean        - Remove build files"
	@echo " make clean-all    - Full cleanup"
	@echo "============================="

.PHONY: all run run-server clean clean-all help
