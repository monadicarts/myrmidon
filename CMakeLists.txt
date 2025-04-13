# File: CMakeLists.txt

# Minimum CMake version required
cmake_minimum_required(VERSION 3.8)

# Define the project name, version (optional), and language
project(MyCpp17Project VERSION 1.0 LANGUAGES CXX)

# --- Set C++ Standard ---
# Require C++17
set(CMAKE_CXX_STANDARD 17)
# Enforce the standard (don't allow compiler to downgrade)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Optional: Prevent compiler-specific extensions (good practice)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Find Source Files ---
# You can list them explicitly:
# set(SOURCES src/main.cpp)
# Or use globbing (simpler for starting, but can have issues if files are added/removed without re-running CMake)
file(GLOB SOURCES "src/*.cpp")

# --- Define Executable ---
# Create an executable named MyCpp17Project from the source files
add_executable(MyCpp17Project ${SOURCES})

# --- Include Direc>tories ---
# Tell CMake where to find header files (if you use the include directory)
target_include_directories(MyCpp17Project PUBLIC include)

# --- Optional: Link Libraries ---
# If you add external libraries later, you'll link them here, e.g.:
# find_package(Boost REQUIRED COMPONENTS system filesystem)
# target_link_libraries(MyCpp17Project PRIVATE Boost::system Boost::filesystem)

# --- Optional: Installation ---
# install(TARGETS MyCpp17Project DESTINATION bin) # Example

# --- Optional: Testing (using CTest) ---
# enable_testing()
# add_test(NAME MyTest COMMAND MyCpp17Project) # Example test
