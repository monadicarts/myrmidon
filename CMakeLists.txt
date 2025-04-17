# File: CMakeLists.txt

# Minimum CMake version - Consider 3.15+ for better C++17 support features
cmake_minimum_required(VERSION 3.15)

# Define the project name, version (optional), and language
project(Myrmidon VERSION 1.0 LANGUAGES CXX)

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
    src/core/rule.cpp
    # src/pattern.cpp
    # src/constraint.cpp
    # src/rule.cpp
    # src/lhs_tree_node.cpp
    # Add other .cpp files belonging to the core engine here
)

# Create the static or shared library for the rule engine core
# (Target name RuleEngineCore is okay, or change to MyrmidonCore if preferred)
add_library(RuleEngineCore STATIC ${ENGINE_SOURCES}) # Or SHARED

# Tell CMake where to find header files for the RuleEngineCore library
# PUBLIC: Consumers of RuleEngineCore also need this include path
# INTERFACE: Only consumers need this path (e.g., inline/header-only parts)
# PRIVATE: Only RuleEngineCore needs this path during its own compilation
target_include_directories(RuleEngineCore
    PUBLIC
        # Expose the 'include' directory for linking targets
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        # Define where headers will be installed (relative to CMAKE_INSTALL_PREFIX/include)
        $<INSTALL_INTERFACE:include>
    PRIVATE
        # Internal include paths needed only by RuleEngineCore itself
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core
)

# --- Define Example Executable Using the Engine ---

# Create an executable (e.g., for testing, examples, or the main application)
# (Target name RuleEngineApp is okay, or change to MyrmidonApp if preferred)
add_executable(RuleEngineApp src/main.cpp)

# Link the executable against the core engine library
# RuleEngineApp inherits PUBLIC include directories from RuleEngineCore
target_link_libraries(RuleEngineApp PRIVATE RuleEngineCore)

# --- Optional: Link External Libraries (Example) ---
# ... (Keep your existing commented-out examples if desired) ...

# --- Optional: Installation ---
# ... (Keep your existing commented-out examples if desired) ...

# --- Testing (using CTest and GoogleTest) ---
enable_testing()

# Find the GoogleTest package
find_package(GTest REQUIRED)

# Define the test executable
# (Target name RuleEngineTests is okay, or change to MyrmidonTests if preferred)
add_executable(RuleEngineTests
    tests/test_base_fact.cpp
    tests/core/test_rule.cpp
    # Add other test source files here as you create them
)

# Add include directories needed specifically by the test executable
# This comes AFTER add_executable(RuleEngineTests ...)
target_include_directories(RuleEngineTests PRIVATE
    # It needs the public headers (could rely on inheritance, but explicit is fine)
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    # CRITICAL: It needs internal headers not inherited via linking
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    # Add ${CMAKE_CURRENT_SOURCE_DIR}/src if other internal headers are needed
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
# add_test(NAME BasicRunTest COMMAND RuleEngineApp)