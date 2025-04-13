# File: /home/justin/repos/cpp-rules/Makefile
# A wrapper Makefile for convenience around the CMake build process for Myrmidon.

# --- Variables ---
BUILD_DIR := build
CMAKE_GENERATOR := # Specify if needed, e.g., -G Ninja
CMAKE_CONFIG_ARGS := $(CMAKE_GENERATOR) ..
MAKE_CMD := $(MAKE) # Use $(MAKE) to ensure recursive calls use the same make

# --- Targets ---

.DEFAULT_GOAL := all

# Target to ensure CMake configuration is done
$(BUILD_DIR)/Makefile: CMakeLists.txt
	@echo "📁 Ensuring Build Directory $(BUILD_DIR) exists..."
	@mkdir -p $(BUILD_DIR)
	@echo "⚙️  Configuring CMake for Myrmidon..."
	@cd $(BUILD_DIR) && cmake $(CMAKE_CONFIG_ARGS)

# Configure target (explicitly run CMake)
configure: $(BUILD_DIR)/Makefile

# Build the project (compile and link all targets)
build: $(BUILD_DIR)/Makefile
	@echo "🏗️  Building Myrmidon Project..."
	@$(MAKE_CMD) -C $(BUILD_DIR)

# Default target 'all' usually builds everything defined in CMake.
all: build

# Run tests using CTest
test: build
	@echo "🏃 Running Myrmidon Tests..."
	@$(MAKE_CMD) -C $(BUILD_DIR) test

# Install the project
install: build
	@echo "📦 Installing Myrmidon Project..."
	@$(MAKE_CMD) -C $(BUILD_DIR) install

# Clean the build directory
clean:
	@echo "🧹 Cleaning Build Directory..."
	@rm -rf $(BUILD_DIR)

# Phony targets prevent conflicts with files of the same name
.PHONY: all configure build test install clean

