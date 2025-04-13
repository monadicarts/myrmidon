# File: CMakeLists.txt

# Minimum CMake version - Consider 3.15+ for better C++17 support features
cmake_minimum_required(VERSION 3.15)

# Define the project name, version (optional), and language
project(RuleEngine VERSION 1.0 LANGUAGES CXX)

# --- Set C++ Standard (Globally or Per-Target) ---
# Require C++17
set(CMAKE_CXX_STANDARD 17)
# Enforce the standard (don't allow compiler to downgrade)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Optional: Prevent compiler-specific extensions (good practice)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Define Core Engine Library ---

# List the source files for the core rule engine library
# Explicitly list files for robustness instead of using GLOB
set(ENGINE_SOURCES
    src/base_fact.cpp
    # src/pattern.cpp       # Assuming these exist or will be added
    # src/constraint.cpp
    # src/rule.cpp
    # src/lhs_tree_node.cpp
    # Add other .cpp files belonging to the core engine here
    # e.g., src/leap_network.cpp (when implemented)
)

# Create the static or shared library for the rule engine core
# Choose STATIC or SHARED based on your needs. STATIC is often simpler initially.
add_library(RuleEngineCore STATIC ${ENGINE_SOURCES}) # Or SHARED

# Tell CMake where to find header files for the RuleEngineCore library
# PUBLIC: Consumers of RuleEngineCore also need this include path
# INTERFACE: Only consumers need this path (e.g., inline/header-only parts)
# PRIVATE: Only RuleEngineCore needs this path during its own compilation
target_include_directories(RuleEngineCore
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> # Headers for consumers
        $<INSTALL_INTERFACE:include> # Headers for consumers when installed (adjust path)
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src # If headers are alongside sources or private headers exist
)

# --- Define Example Executable Using the Engine ---

# Create an executable (e.g., for testing, examples, or the main application)
# Ensure you have a src/main.cpp or remove/comment this section if not needed yet
add_executable(RuleEngineApp src/main.cpp)

# Link the executable against the core engine library
target_link_libraries(RuleEngineApp PRIVATE RuleEngineCore)

# --- Optional: Link External Libraries (Example) ---
# ... (Keep your existing commented-out examples if desired) ...

# --- Optional: Installation ---
# ... (Keep your existing commented-out examples if desired) ...

# --- Testing (using CTest and GoogleTest) ---
enable_testing()

# Find the GoogleTest package
# Note: This requires GoogleTest to be installed on your system or discoverable
# by CMake (e.g., via CMAKE_PREFIX_PATH). Alternatively, you could use
# FetchContent or add_subdirectory to manage GoogleTest within your project.
find_package(GTest REQUIRED)

# Define the test executable
add_executable(RuleEngineTests
    tests/test_base_fact.cpp
    # Add other test source files here as you create them, e.g.:
    # tests/test_pattern.cpp
)

# Link the test executable against your core library and GoogleTest
target_link_libraries(RuleEngineTests PRIVATE
    RuleEngineCore     # Link against the library being tested
    GTest::gtest       # GoogleTest framework library
    GTest::gtest_main  # Provides the main() function for the test runner
)

# Include the GoogleTest module provided by CMake
include(GoogleTest)

# Automatically discover tests within the RuleEngineTests executable for CTest
gtest_discover_tests(RuleEngineTests)

# --- Optional: Add test that runs the main executable (if it exists) ---
# If you have RuleEngineApp defined above and it's runnable for a basic check:
# add_test(NAME BasicRunTest COMMAND RuleEngineApp)

