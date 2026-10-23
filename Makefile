# Thin convenience wrapper around CMake. The real build lives in CMakeLists.txt.

BIN       := operative
BUILD_DIR := build
DEBUG_DIR := $(BUILD_DIR)/debug
REL_DIR   := $(BUILD_DIR)/release
PREFIX    ?= $(HOME)/.local
ARGS      ?=

# Use Ninja when it's installed, otherwise fall back to CMake's default generator.
GENERATOR := $(if $(shell command -v ninja 2>/dev/null),-G Ninja)
JOBS      ?= $(shell nproc)

.PHONY: all build release run run-release test install uninstall clean

all: build

$(DEBUG_DIR)/CMakeCache.txt:
	cmake -S . -B $(DEBUG_DIR) $(GENERATOR) \
		-DCMAKE_BUILD_TYPE=Debug \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON

$(REL_DIR)/CMakeCache.txt:
	cmake -S . -B $(REL_DIR) $(GENERATOR) \
		-DCMAKE_BUILD_TYPE=Release \
		-DBUILD_TESTING=OFF

build: $(DEBUG_DIR)/CMakeCache.txt
	cmake --build $(DEBUG_DIR) -j $(JOBS)
	@ln -sf $(DEBUG_DIR)/compile_commands.json compile_commands.json

release: $(REL_DIR)/CMakeCache.txt
	cmake --build $(REL_DIR) -j $(JOBS)

run: build
	./$(DEBUG_DIR)/$(BIN) $(ARGS)

run-release: release
	./$(REL_DIR)/$(BIN) $(ARGS)

test: build
	ctest --test-dir $(DEBUG_DIR) --output-on-failure

install: release
	cmake --install $(REL_DIR) --prefix $(PREFIX) --strip

uninstall:
	rm -f $(PREFIX)/bin/$(BIN)

clean:
	rm -rf $(BUILD_DIR) compile_commands.json
