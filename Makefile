# File: /home/justin/repos/cpp-rules/Makefile
# A wrapper Makefile for convenience around the CMake build process.

# --- Variables ---
BUILD_DIR := build
CMAKE_GENERATOR := # Specify if needed, e.g., -G Ninja
CMAKE_CONFIG_ARGS := $(CMAKE_GENERATOR) ..
MAKE_CMD := $(MAKE) # Use $(MAKE) to ensure recursive calls use the same make

# --- Targets ---

.DEFAULT_GOAL := all

# Target to ensure CMake configuration is done
# This file is a good indicator that CMake has run successfully in the build dir.
# We now ensure the build directory exists right before running cmake.
$(BUILD_DIR)/Makefile: CMakeLists.txt
	@echo "📁 Ensuring Build Directory $(BUILD_DIR) exists..."
	@mkdir -p $(BUILD_DIR)
	@echo "⚙️  Configuring CMake..."
	@cd $(BUILD_DIR) && cmake $(CMAKE_CONFIG_ARGS)

# Configure target (explicitly run CMake)
# Depends on the rule above completing.
configure: $(BUILD_DIR)/Makefile

# Build the project (compile and link all targets)
# Depends on configuration being done.
build: $(BUILD_DIR)/Makefile
	@echo "🏗️  Building Project..."
	@$(MAKE_CMD) -C $(BUILD_DIR)

# Default target 'all' usually builds everything defined in CMake.
all: build

# Run tests using CTest
# Depends on the 'build' target completing successfully.
test: build
	@echo "🏃 Running Tests..."
	@$(MAKE_CMD) -C $(BUILD_DIR) test

# Install the project
# Depends on the 'build' target completing successfully.
install: build
	@echo "📦 Installing Project..."
	@$(MAKE_CMD) -C $(BUILD_DIR) install

# Clean the build directory
clean:
	@echo "🧹 Cleaning Build Directory..."
	@rm -rf $(BUILD_DIR)

# Phony targets prevent conflicts with files of the same name
# We removed the separate $(BUILD_DIR) target as it's handled implicitly now.
.PHONY: all configure build test install clean

