#ifndef RULE_ENGINE_BASE_FACT_HPP
#define RULE_ENGINE_BASE_FACT_HPP

#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <any>       // Needed for isEqualUntyped parameter type
#include <utility>   // For std::move
#include <stdexcept> // For potential errors (e.g., in ValueFact handling if needed)
#include <type_traits> // Potentially useful for comparisons or constraints

namespace RuleEngine {

    // --- C++17 compatible equality comparable check (Recursive) ---
namespace detail {
    // Base check: Does T define operator==?
    template <typename T, typename = void>
    struct has_equality_operator : std::false_type {};
    template <typename T>
    struct has_equality_operator<T,
        std::void_t<decltype(std::declval<const T&>() == std::declval<const T&>())>>
        : std::is_convertible<decltype(std::declval<const T&>() == std::declval<const T&>()), bool> {};

    // Forward declaration for recursion
    template <typename T, typename = void>
    struct is_equality_comparable_recursive;

    // Helper to check if T has value_type (like containers)
    template <typename T, typename = void>
    struct has_value_type : std::false_type {};
    template <typename T>
    struct has_value_type<T, std::void_t<typename T::value_type>> : std::true_type {};

    // Main recursive trait definition
    template <typename T, typename /* SFINAE */>
    struct is_equality_comparable_recursive : has_equality_operator<T> {}; // Default: Just check T

    // Specialization for types that have a value_type (like containers)
    template <typename T>
    struct is_equality_comparable_recursive<T, std::enable_if_t<has_value_type<T>::value>>
        : std::conjunction< // Both container AND its elements must be comparable
            has_equality_operator<T>,
            is_equality_comparable_recursive<typename T::value_type> // Recursive check
          > {};

    // The final value template
    template <typename T>
    inline constexpr bool is_equality_comparable_v = is_equality_comparable_recursive<T>::value;

} // namespace detail
// --- End helper trait ---

/**
 * @brief Base template class for all facts in the rule engine.
 *
 * @tparam Collection The type of the container holding the fact's data.
 */
template <typename Collection>
class BaseFact {
private:
    std::string name_;
    Collection values_;

public:
    /**
     * @brief Constructs a BaseFact.
     *
     * @param name The name/identifier of the fact.
     * @param values The data payload of the fact (moved into the object).
     */
    BaseFact(std::string name, Collection values)
        : name_(std::move(name)), values_(std::move(values)) {}

    /**
     * @brief Virtual destructor for proper cleanup in derived classes (if any)
     *        or when holding BaseFact pointers.
     */
    virtual ~BaseFact() = default;

    // Prevent copying and assignment to avoid slicing and ownership issues,
    // especially with RefFact. Facts are typically managed by the engine.
    BaseFact(const BaseFact&) = delete;
    BaseFact& operator=(const BaseFact&) = delete;

    // Allow moving
    BaseFact(BaseFact&&) noexcept = default;
    BaseFact& operator=(BaseFact&&) noexcept = default;

    /**
     * @brief Gets the name of the fact.
     * @return const std::string& The fact's name.
     */
    const std::string& getName() const noexcept {
        return name_;
    }

    /**
     * @brief Gets the underlying data collection of the fact.
     * @return const Collection& A constant reference to the fact's data.
     */
    const Collection& getValues() const noexcept {
        return values_;
    }

    /**
     * @brief Compares this fact with another fact of the exact same type.
     *
     * Default implementation compares name and uses the == operator of the Collection type.
     * Can be overridden for custom comparison logic.
     *
     * @param other A pointer to another BaseFact of the same Collection type.
     * @return true If the facts are considered equal, false otherwise.
     */
    virtual bool equals(const BaseFact<Collection>* other) const {
        if (!other) {
            return false;
        }
        // Use the C++17 compatible helper trait
        if constexpr (detail::is_equality_comparable_v<Collection>) { // <--- MODIFIED LINE
             return name_ == other->name_ && values_ == other->values_;
        } else {
            // If Collection is not comparable, maybe only compare names?
            // Or throw? Or require specialization? For now, compare names.
            // Consider logging a warning here if comparison is not supported.
            return name_ == other->name_;
            // Alternative: static_assert(!std::is_same_v<Collection, Collection>, "Collection type must be equality comparable for default equals");
        }
    }
    

    /**
     * @brief Compares this fact with another fact represented generically.
     *
     * This method is intended for heterogeneous comparisons where the 'other'
     * fact's type might not be known at compile time or needs to be treated
     * generically. The default implementation provides a basic check.
     * This might need more sophisticated handling or overriding depending on
     * specific engine requirements for cross-type comparisons.
     *
     * @param other A pointer to a BaseFact specialized with std::vector<std::any>.
     * @return true If the facts are considered equal in this untyped context, false otherwise.
     */
    virtual bool isEqualUntyped(const BaseFact<std::vector<std::any>>* other) const {
        // Default implementation for untyped comparison is challenging.
        // A minimal check might compare only names.
        // More complex logic would require type introspection and conversion,
        // which can be complex and potentially slow.
        if (!other) {
            return false;
        }
        // Basic check: compare names.
        return name_ == other->getName();
        // Returning false is also a safe default if name-only comparison isn't desired.
        // return false;
    }
};

// --- Type Aliases for Common Fact Types ---

/**
 * @brief Fact holding an ordered sequence (vector) of values.
 * @tparam T The type of elements in the vector.
 */
template <typename T>
using ListFact = BaseFact<std::vector<T>>;

/**
 * @brief Fact holding key-value pairs (map).
 * @tparam K The key type.
 * @tparam V The value type.
 */
template <typename K, typename V>
using MapFact = BaseFact<std::map<K, V>>;

/**
 * @brief Fact holding a unique collection (unordered set) of values.
 * @tparam T The type of elements in the set.
 */
template <typename T>
using SetFact = BaseFact<std::unordered_set<T>>;

/**
 * @brief Fact holding a single value.
 *
 * Internally represented as a vector containing one element.
 * Use like: ValueFact<int> myIntFact("count", std::vector<int>{10});
 * Or consider adding a helper function/constructor if direct vector creation is cumbersome.
 *
 * @tparam T The type of the single value.
 */
template <typename T>
using ValueFact = BaseFact<std::vector<T>>;

/**
 * @brief Fact holding a raw pointer to an external object.
 *
 * The lifetime management of the pointed-to object is external to the rule engine.
 * Use like: MyObject* ptr = ...; RefFact<MyObject> ref("obj_ptr", ptr);
 *
 * @tparam T The type of the pointed-to object. Note: Stores T*, not T.
 */
template <typename T>
using RefFact = BaseFact<T*>; // Stores a pointer of type T*

} // namespace RuleEngine

#endif // RULE_ENGINE_BASE_FACT_HPP
