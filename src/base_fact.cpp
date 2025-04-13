// File: src/base_fact.cpp

#include "rule_engine/base_fact.hpp" // Adjust path if necessary

// Include standard library headers for the types used in explicit instantiations
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <vector> // Specifically for std::vector<bool> specialization

// --- Forward Declaration or Include for Custom Types ---
// If you use RefFact with your own classes/structs, you need to either:
// 1. Include the full definition (e.g., #include "my_object.hpp")
// OR
// 2. Provide a forward declaration if the full definition isn't needed
//    for the template instantiation itself (often sufficient for pointer types).
namespace RuleEngine {
    // Add forward declarations for other custom types used in RefFact if needed
    // struct AnotherCustomType;
} // namespace RuleEngine


namespace RuleEngine {

// --- Explicit Template Instantiations ---
// Pre-compile common BaseFact specializations to potentially improve
// overall build times. Other cpp files linking against the library
// will use these pre-compiled versions instead of generating their own.
//
// **Important:** Customize this list based on the actual, most frequent
// concrete types you use with ListFact, MapFact, SetFact, ValueFact, and RefFact.

// --- Instantiations for ListFact<T> / ValueFact<T> ---
// Underlying type: std::vector<T>
template class BaseFact<std::vector<int>>;
template class BaseFact<std::vector<double>>;
template class BaseFact<std::vector<std::string>>;
template class BaseFact<std::vector<bool>>;

// --- Instantiations for MapFact<K, V> ---
// Underlying type: std::map<K, V>
template class BaseFact<std::map<std::string, int>>;
template class BaseFact<std::map<std::string, double>>;
template class BaseFact<std::map<std::string, std::string>>;
template class BaseFact<std::map<int, std::string>>;
template class BaseFact<std::map<std::string, bool>>;

// --- Instantiations for SetFact<T> ---
// Underlying type: std::unordered_set<T>
template class BaseFact<std::unordered_set<int>>;
template class BaseFact<std::unordered_set<std::string>>;
template class BaseFact<std::unordered_set<bool>>;

// --- Instantiations for RefFact<T> ---
// Underlying type: T*
// Requires forward declaration (above) or include for custom types
// template class BaseFact<MyObject*>;
// template class BaseFact<Object*>; 
// Add other common pointer types used with RefFact if needed, e.g.:
// template class BaseFact<AnotherCustomType*>;
// template class BaseFact<int*>; // If you reference primitive pointers

} // namespace RuleEngine
