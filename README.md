# Myrmidon Rule Engine

<!-- [![Build Status](https://img.shields.io/github/actions/workflow/status/monadicarts/myrmidon/ci.yml?branch=main&style=flat-square)](https://github.com/monadicarts/myrmidon/actions) -->

[![License](https://img.shields.io/github/license/monadicarts/myrmidon?style=flat-square)](LICENSE)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg?style=flat-square)](https://isocpp.org/std/the-standard)

**Forge intelligent decisions with the precision and efficiency of a [Myrmidon](https://en.wikipedia.org/wiki/Myrmidons).**

Myrmidon is a high-performance, flexible, and extensible rule engine crafted in modern C++17. It aims to fuse the declarative power of systems like [CLIPS](https://www.clipsrules.net) with the blazing-fast execution potential inspired by the [LEAP](https://dl.acm.org/doi/10.1145/3377929.3398147) network architecture. Designed for applications demanding real-time decision-making, complex pattern matching, and dynamic reactions to evolving data.

---

## ✨ Core Philosophy

Myrmidon is built on the belief that expressive rule definition shouldn't come at the cost of performance. We leverage modern C++ features and proven algorithms to create an engine that is both powerful and efficient, ready to serve as the logical core of demanding applications.

## 🔥 Key Features & Design Goals

- ✅ **Flexible Fact Representation:** Utilizes C++ templates (`BaseFact<Collection>`) to represent facts using various standard containers (`std::vector`, `std::map`, `std::unordered_set`, pointers) without boilerplate code. Type aliases (`ListFact`, `MapFact`, `SetFact`, `ValueFact`, `RefFact`) provide convenient usage. _(Implemented & Tested)_
- ⚙️ **Design for Expressive Pattern Matching:** A CLIPS-inspired `Pattern` and `Constraint` system allows specific value matching, predicate functions, variable binding, and negation. C++17 `if constexpr` ensures efficient type dispatch. _(Partially Implemented - Core structures defined, matching logic needs refinement, especially for `std::any`)_
- ⚙️ **Design for Complex Logic via Trees:** Rule conditions (LHS) are represented as trees (`TreeNode`), enabling sophisticated combinations of patterns using AND, OR, and NOT operators. _(Partially Implemented - Core structures defined, `evaluate` logic needs refinement, especially for PATTERN_NODE)_
- 🚀 **Goal: High Performance:** Engineered for near real-time execution, targeting efficient algorithms and data structures.
- 💎 **Modern C++17:** Built entirely using C++17 standards for performance, safety, and maintainability.
- 🧩 **Goal: Extensibility:** Designed with modularity in mind, allowing easy addition of new fact types, conditions, and actions.

## 🚀 Current Status (Early Development)

Myrmidon is in the early stages of development.

- The core **Fact Representation** system (`BaseFact`, type aliases like `ListFact`, `MapFact`, etc.) is implemented and unit-tested.
- Foundational structures for **Patterns, Constraints, and Rules** (`Pattern`, `Constraint`, `TreeNode`, `Rule`) are defined.
- The basic build system (CMake) and structural tests are functional.
- The core rule **matching and evaluation logic** (`Pattern::matches`, `TreeNode::evaluate`) is partially implemented but requires significant **refinement**, particularly regarding robust `std::any` comparison and type-safe handling of heterogeneous fact collections.

## 🛠️ Getting Started

### Prerequisites

- CMake (version 3.15 or higher recommended)
- A C++17 compliant compiler (GCC, Clang, MSVC)
- Ninja Build (Recommended build tool, usually detected by CMake if installed)
- Google Test (for running tests, automatically found by CMake if installed)

### Building and Testing

Myrmidon uses CMake for building and CTest/Google Test for testing. The following commands are run from the project's root directory:

1.  **Clone the repository:**

    ```bash
    git clone https://github.com/monadicarts/myrmidon.git
    cd myrmidon
    ```

2.  **Configure CMake (Generate build files):** (Run once initially, or after changing `CMakeLists.txt`)

    ```bash
    # Creates a 'build' directory and generates Ninja files inside it
    cmake -S . -B build
    # Optional: Specify build type (Debug, Release, RelWithDebInfo, MinSizeRel)
    # cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
    ```

3.  **Build the project (Compile & Link):** (Run after configuring or changing source code)

    ```bash
    ninja -C build
    ```

4.  **Run tests:** (Run after a successful build)

    ```bash
    ctest --test-dir build
    # Or for more verbose output:
    # ctest --test-dir build -V
    ```

5.  **Clean build artifacts:** (Removes compiled objects, libraries, executables)

    ```bash
    ninja -C build clean
    ```

6.  **Full Clean:** (Removes the entire build directory)
    ```bash
    rm -rf build
    ```

The compiled library (`libRuleEngineCore.a`), example application (`RuleEngineApp`), and test runner (`RuleEngineTests`) will be located in the `build` directory after a successful build.

## 💡 Basic Usage Examples

### Fact Creation

```cpp
#include <iostream>
#include "myrmidon/base_fact.hpp" // Core fact definitions
#include <string>
#include <vector>

// Use the namespace for convenience
using namespace Myrmidon;

int main() {
    // Create a fact representing a temperature reading (single value)
    ValueFact<double> temp("sensor-A-temp", {25.7});

    std::cout << "Fact Name: " << temp.getName() << std::endl;
    if (!temp.getValues().empty()) {
        std::cout << "Fact Value: " << temp.getValues()[0] << std::endl;
    }

    // Create a fact representing a list of active users
    ListFact<std::string> users("active_users", {"Alice", "Bob"});
    std::cout << "Fact Name: " << users.getName() << std::endl;
    // ... access users.getValues() ...

    return 0;
}
```

### Conceptual Rule Example

**Note**: The following demonstrates the structure of defining a rule. The core evaluation logic (TreeNode::evaluate) and rule execution engine are not yet fully functional and require significant refinement (especially regarding type handling).

```cpp
#include "myrmidon/base_fact.hpp"
#include "core/rule.hpp" // Rule, TreeNode, Pattern, Constraint definitions
#include <iostream>
#include <vector>
#include <string>
#include <functional> // For std::function
#include <memory>     // For std::make_unique
#include <any>        // For std::any

using namespace Myrmidon;

// Example predicate function for a constraint
bool is_critical_temp(const std::any& value) {
    if (const double* pval = std::any_cast<double>(&value)) {
        return *pval > 90.0;
    }
    return false;
}

int main() {
    // --- Example Rule Definition ---
    // Rule: IF temperature > 90.0 THEN print critical alert.

    // 1. Define the Pattern for the LHS (Left-Hand Side)
    // Matches facts named "sensor-temp" where the value satisfies the predicate
    Pattern tempPattern("sensor-temp", // Fact name to match
        { Constraint("", // Attribute name (empty for simple Value/List/Set facts)
                      std::function<bool(const std::any&)>(is_critical_temp), // Predicate test
                      "temp_val" // Variable to bind the matching value to (optional)
        )}
    );

    // 2. Create the LHS Tree Structure (simple tree with one pattern node)
    auto lhsRoot = std::make_unique<TreeNode>(tempPattern);

    // 3. Define the RHS (Right-Hand Side) Action
    // Takes a map of bindings captured by the LHS patterns
    auto rhsAction = [](const std::map<std::string, std::any>& bindings) {
        std::cout << "RHS ACTION: Critical Temperature Alert! ";
        // Access bound variable if needed (requires proper casting)
        if (bindings.count("temp_val")) {
            try {
                double temp = std::any_cast<double>(bindings.at("temp_val"));
                std::cout << "Measured temp: " << temp;
            } catch (const std::bad_any_cast& e) {
                std::cerr << "Error casting bound variable: " << e.what();
            }
        }
        std::cout << std::endl;
    };

    // 4. Define the Rule
    Rule criticalTempRule;
    criticalTempRule.lhsTree = std::move(lhsRoot);
    // The RHS needs adjustment based on how the engine passes facts/bindings
    // This is a simplified conceptual action lambda:
    criticalTempRule.rhsActions.push_back(
       [rhsAction](const std::vector<const BaseFact<void>*>& /*matchedFacts - unused here*/) {
            // FIXME: This part requires the engine to actually capture and pass bindings.
            // For now, we just call the action conceptually.
            std::map<std::string, std::any> capturedBindings; // These would be filled by the engine
             // Example: capturedBindings["temp_val"] = std::any(95.0); // Manually for demo
            rhsAction(capturedBindings);
       }
    );

    std::cout << "Conceptual rule 'criticalTempRule' defined." << std::endl;

    // --- Engine Execution (Conceptual) ---
    // RuleEngine engine;
    // engine.addRule(std::move(criticalTempRule));
    // engine.addFact(std::make_shared<ValueFact<double>>("sensor-temp", {95.5}));
    // engine.run(); // This would evaluate rules and trigger RHS actions

    std::cout << "(Note: Rule execution logic is not yet implemented)" << std::endl;

    return 0;
}
```

## 🤝 Contributing

Contributions are welcome! Please see `CONTRIBUTING.md` for guidelines on reporting bugs, suggesting features, and submitting pull requests.

## 📜 License

Myrmidon is licensed under the Apache License, Version 2.0. See the `LICENSE` file for details.
