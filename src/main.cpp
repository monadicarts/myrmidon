// File: src/main.cpp
#include <iostream>
#include <string> // Using C++17 features later might involve other headers

int main() {
    // Example using a C++17 feature (structured bindings)
    auto [major, minor, patch] = std::make_tuple(1, 0, 0); // Simple tuple

    std::cout << "Hello, C++17 World!" << std::endl;
    std::cout << "Project Version: " << major << "." << minor << "." << patch << std::endl;

    // Another C++17 feature: constexpr if
    if constexpr (sizeof(int) == 4) {
        std::cout << "Running on a system where int is 32 bits." << std::endl;
    } else {
        std::cout << "Running on a system where int is not 32 bits." << std::endl;
    }

    return 0;
}
