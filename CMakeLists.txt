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
    src/pattern.cpp
    src/constraint.cpp
    src/rule.cpp
    src/lhs_tree_node.cpp
    # Add other .cpp files belonging to the core engine here
    # e.g., src/leap_network.cpp (when implemented)
)

# List the public header files if they are separate and need installing
# (Often headers in include/ don't need explicit listing here unless installing)
# set(ENGINE_HEADERS
#     include/rule_engine/base_fact.hpp
#     include/rule_engine/pattern.hpp
#     # ... other public headers
# )

# Create the static or shared library for the rule engine core
# Choose STATIC or SHARED based on your needs. STATIC is often simpler initially.
add_library(RuleEngineCore STATIC ${ENGINE_SOURCES}) # Or SHARED

# Set C++ standard specifically for the library target (alternative to global set)
# target_compile_features(RuleEngineCore PUBLIC cxx_std_17)
# target_compile_options(RuleEngineCore PRIVATE -Wall -Wextra -pedantic) # Example compiler flags

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
add_executable(RuleEngineApp src/main.cpp) # Assuming you have a main.cpp

# Link the executable against the core engine library
target_link_libraries(RuleEngineApp PRIVATE RuleEngineCore)

# --- Optional: Link External Libraries (Example) ---
# If RuleEngineCore needs external libraries:
# find_package(Boost REQUIRED COMPONENTS system filesystem)
# target_link_libraries(RuleEngineCore PUBLIC Boost::system Boost::filesystem) # PUBLIC if headers from Boost are in your public headers
# If only RuleEngineApp needs external libraries:
# find_package(SomeOtherLib REQUIRED)
# target_link_libraries(RuleEngineApp PRIVATE SomeOtherLib::SomeOtherLib)

# --- Optional: Installation ---
# Install the library, its public headers, and the executable
# include(GNUInstallDirs) # Provides standard install paths (CMAKE_INSTALL_LIBDIR, etc.)
# install(TARGETS RuleEngineCore
#     EXPORT RuleEngineCoreTargets
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Runtime
#     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime # For SHARED libs on Windows/Mac
# )
# install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT Development) # Install public headers
# install(TARGETS RuleEngineApp DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)

# # Generate and install the export file for CMake package configuration
# install(EXPORT RuleEngineCoreTargets
#     FILE RuleEngineCoreConfig.cmake
#     NAMESPACE RuleEngine::
#     DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/RuleEngineCore
# )

# --- Optional: Testing (using CTest) ---
enable_testing()

# Example: Add a test that runs the main executable
add_test(NAME BasicRunTest COMMAND RuleEngineApp)

# Example: If you build a separate test executable using a framework like GoogleTest
# find_package(GTest REQUIRED)
# add_executable(RuleEngineTests tests/test_main.cpp tests/test_facts.cpp /*...other test files...*/)
# target_link_libraries(RuleEngineTests PRIVATE RuleEngineCore GTest::gtest GTest::gtest_main)
# include(GoogleTest)
# gtest_discover_tests(RuleEngineTests)

