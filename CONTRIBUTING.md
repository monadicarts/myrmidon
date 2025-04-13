# Contributing to Myrmidon

First off, thank you for considering contributing to Myrmidon! We're excited to build a powerful and efficient C++17 rule engine, and we welcome your help. Every contribution, big or small, is valuable as we forge this project.

This document provides guidelines for contributing.

## Code of Conduct

While we don't have a formal Code of Conduct yet, we expect all contributors to interact respectfully and constructively. Please be welcoming and considerate of others. Harassment or exclusionary behavior will not be tolerated. *(Consider adding a formal Code of Conduct, e.g., from Contributor Covenant)*

## How Can I Contribute?

Myrmidon is in its early stages, so there are many ways to help!

### Reporting Bugs

*   As the engine grows, bugs will inevitably appear. If you find one, please check the [GitHub Issues](https://github.com/monadicarts/myrmidon/issues) to see if it's already reported.
*   If not, open a new issue with:
    *   A clear, descriptive title (e.g., "Compilation error in BaseFact::equals with specific type").
    *   Detailed steps to reproduce the bug.
    *   Expected behavior vs. actual behavior (include error messages, incorrect output).
    *   Your environment details (OS, Compiler version, CMake version).

### Suggesting Enhancements or Features

*   Have an idea for the Pattern matching system? Thoughts on the LEAP network implementation? Want a new Fact type? Open an issue!
*   Clearly describe the enhancement and why it would be valuable.
*   Discuss potential implementation approaches if you have them.

### Documentation Improvements

*   Found a typo in the README? Think the build instructions could be clearer? Documentation contributions are always welcome via Pull Requests.

### Code Contributions (Pull Requests)

Ready to write some code? We'd love to see it!

1.  **Fork the repository** on GitHub.
2.  **Clone your fork** locally: `git clone https://github.com/YOUR_FORK/myrmidon.git`
3.  **Create a new branch** for your changes (please use descriptive names):
    *   `git checkout -b feature/pattern-matching-core`
    *   `git checkout -b fix/basefact-comparison-bug`
4.  **Make your changes.** Follow the coding style guidelines below.
5.  **Add Tests!** This is crucial. If you add functionality or fix a bug, add corresponding unit tests using Google Test in the `tests/` directory. Ensure your changes don't break existing tests.
6.  **Ensure all tests pass:** Run `make test` from the project root.
7.  **Commit your changes** using Conventional Commit messages (see below).
    ```bash
    git add .
    git commit -m "feat: Implement basic Pattern structure" -m "Adds Pattern struct with name and constraints vector. No matching logic yet."
    ```
8.  **Push your branch** to your fork: `git push origin feature/pattern-matching-core`
9.  **Open a Pull Request** from your fork's branch to the `main` branch of the `monadicarts/myrmidon` repository.
10. **Describe your changes** clearly in the Pull Request description. Explain the "what" and "why". Link to any relevant issues (e.g., "Closes #12").

## Development Setup

Refer to the `README.md` for instructions on setting up your build environment (CMake, C++17 compiler, Google Test) and building/testing the project using the provided `Makefile` wrapper.

## Coding Style

*   **Language:** Use modern C++17 features appropriately.
*   **Formatting:** Aim for consistency. *(Consider adding a `.clang-format` file later)*. Use spaces for indentation (e.g., 4 spaces).
*   **Naming:** Use `PascalCase` for classes/structs/enums, `camelCase` for functions/methods, and `snake_case` for variables/parameters. Use `snake_case_` for private member variables.
*   **Readability:** Write clear, self-documenting code where possible. Add comments to explain complex logic or non-obvious decisions.
*   **Namespace:** Place engine code within the `Myrmidon` namespace.
*   **Documentation:** Use Doxygen-style comments (`/** ... */`) for public APIs (classes, methods).

## Commit Messages

Please follow the Conventional Commits specification. This helps maintain a clear history and enables potential automation.

**Format:** `<type>[optional scope]: <description>`

**Common types:**
*   `feat`: A new feature (e.g., adding variable binding).
*   `fix`: A bug fix (e.g., correcting comparison logic).
*   `chore`: Build process, tooling, or config changes (e.g., updating CMakeLists).
*   `refactor`: Code changes that don't add features or fix bugs (e.g., renaming).
*   `test`: Adding or improving tests.
*   `docs`: Documentation changes.
*   `perf`: Performance improvements.

**Example:** `fix(BaseFact): Correct comparison logic for non-comparable collections`

## Pull Request Review

1.  Maintainers will review your Pull Request.
2.  Feedback may be given. Please address comments and push updates to your branch (the PR will update automatically).
3.  Once approved and checks pass, the PR will be merged.

Thank you for contributing to the legion! Let's build something great together.
