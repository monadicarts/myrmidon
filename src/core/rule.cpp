#include "rule.hpp"
#include "myrmidon/base_fact.hpp"
#include <iostream>
#include <type_traits>
#include <stdexcept>
#include <typeinfo>
#include <functional>
#include <map>
#include <vector>
#include <unordered_set>
#include <any>
#include <variant> // Include variant for std::get/index

namespace Myrmidon {

// --- Helper function for std::any comparison (Needs specific types) ---
// You'll need to expand this or use a different approach for robust any comparison
template<typename T>
bool compare_any(const std::any& a1, const std::any& a2) {
    if (a1.type() != typeid(T) || a2.type() != typeid(T)) {
        return false;
    }
    try {
        return std::any_cast<const T&>(a1) == std::any_cast<const T&>(a2);
    } catch (const std::bad_any_cast&) {
        return false; // Should ideally not happen if type check passed
    }
}

// Overload for common types (add more as needed)
bool are_anys_equal(const std::any& a1, const std::any& a2) {
    if (!a1.has_value() || !a2.has_value() || a1.type() != a2.type()) {
        return false;
    }
    // Add specific type comparisons here
    if (a1.type() == typeid(int)) return compare_any<int>(a1, a2);
    if (a1.type() == typeid(std::string)) return compare_any<std::string>(a1, a2);
    if (a1.type() == typeid(double)) return compare_any<double>(a1, a2);
    if (a1.type() == typeid(bool)) return compare_any<bool>(a1, a2);
    // ... add other supported types ...

    std::cerr << "WARN: Unsupported type encountered in are_anys_equal: " << a1.type().name() << std::endl;
    return false; // Cannot compare unknown types directly
}


// --- Pattern::matches Definition ---
template <typename FactType>
bool Pattern::matches(const BaseFact<FactType>* fact, std::map<std::string, std::any>& bindings) const {
    if (fact == nullptr || fact->getName() != factName) {
        return false;
    }
    const auto& factValues = fact->getValues();
    bool patternSatisfied = true;

    if (constraints.empty()) {
        return true;
    }

    for (const auto& constraint : constraints) {
        bool constraintSatisfied = false;

        // Determine the element type (value_type for vector/set, mapped_type for map)
        // Need a helper trait or separate constexpr if blocks for map vs others
        using ElementType = typename FactType::value_type; // Default for vector/set

        if constexpr (std::is_same_v<FactType, std::vector<ElementType>> ||
                      std::is_same_v<FactType, std::unordered_set<ElementType>>)
        {
            // --- Vector or Set Logic ---
            for (const auto& factElement : factValues) {
                bool testResult = false;

                if (constraint.test.index() == 0) { // Value Constraint (holds std::any)
                    const auto& constraint_any = std::get<std::any>(constraint.test);
                    if constexpr (std::is_same_v<ElementType, std::any>) {
                         // Case: Fact stores std::any, Constraint stores std::any
                         testResult = are_anys_equal(factElement, constraint_any);
                    } else {
                         // Case: Fact stores concrete type, Constraint stores std::any
                         if (constraint_any.has_value() && constraint_any.type() == typeid(ElementType)) {
                             try {
                                testResult = (factElement == std::any_cast<ElementType>(constraint_any));
                             } catch (const std::bad_any_cast&) { testResult = false; }
                         } // else: type mismatch, testResult is false
                    }
                } else { // Predicate Constraint (index 1)
                    const auto& pred = std::get<std::function<bool(const std::any&)>>(constraint.test);
                    testResult = pred(std::any(factElement)); // Wrap element for predicate
                }

                if (constraint.negate ? !testResult : testResult) {
                    constraintSatisfied = true;
                    if (!constraint.variable.empty()) {
                        bindings[constraint.variable] = factElement;
                    }
                    break; // Found match for this constraint in vector/set
                }
            } // end element loop
        }
        else if constexpr (std::is_base_of_v<std::map<typename FactType::key_type, typename FactType::mapped_type>, FactType> ||
                           std::is_same_v<FactType, std::map<typename FactType::key_type, typename FactType::mapped_type>> )
                           // More robust check might be needed for map-like types
        {
             // --- Map Logic ---
             using KeyType = typename FactType::key_type;
             using MappedType = typename FactType::mapped_type;

             // Assuming KeyType is compatible with constraint.attribute (e.g., both std::string)
             if constexpr (std::is_same_v<KeyType, std::string>) {
                  if (auto it = factValues.find(constraint.attribute); it != factValues.end()) {
                      const MappedType& factMapValue = it->second;
                      bool testResult = false;

                      if (constraint.test.index() == 0) { // Value Constraint
                          const auto& constraint_any = std::get<std::any>(constraint.test);
                          if constexpr (std::is_same_v<MappedType, std::any>) {
                              // Case: Map stores std::any, Constraint stores std::any
                               testResult = are_anys_equal(factMapValue, constraint_any);
                          } else {
                              // Case: Map stores concrete type, Constraint stores std::any
                               if (constraint_any.has_value() && constraint_any.type() == typeid(MappedType)) {
                                   try {
                                       testResult = (factMapValue == std::any_cast<MappedType>(constraint_any));
                                   } catch (const std::bad_any_cast&) { testResult = false; }
                               } // else: type mismatch, testResult is false
                          }
                      } else { // Predicate Constraint (index 1)
                          const auto& pred = std::get<std::function<bool(const std::any&)>>(constraint.test);
                          testResult = pred(std::any(factMapValue)); // Wrap map value for predicate
                      }

                      // Apply negation
                      if (constraint.negate ? !testResult : testResult) {
                          constraintSatisfied = true;
                          if (!constraint.variable.empty()) {
                              bindings[constraint.variable] = factMapValue;
                          }
                          // No break for maps, check applied to specific attribute
                      }
                  } // end if key exists
             } // end if key type is string
        }
        // else if constexpr (std::is_pointer_v<FactType>) { /* Pointer handling */ }


        if (!constraintSatisfied) {
            patternSatisfied = false;
            break; // Constraint failed, pattern fails
        }
    } // end constraint loop

    return patternSatisfied;
} // <--- !! IMPORTANT: Ensure this brace closes the function !!


// --- TreeNode::evaluate Implementation ---
bool TreeNode::evaluate(const std::vector<const BaseFact<void>*>& facts, std::map<std::string, std::any>& bindings) const {
    switch (this->type) {
        case NodeType::AND_NODE: {
            for (const auto& child : children) {
                if (!child->evaluate(facts, bindings)) {
                    return false;
                }
            }
            return true;
        }
        case NodeType::OR_NODE: {
             std::map<std::string, std::any> originalBindings = bindings;
            for (const auto& child : children) {
                std::map<std::string, std::any> branchBindings = originalBindings;
                if (child->evaluate(facts, branchBindings)) {
                    bindings = std::move(branchBindings);
                    return true;
                }
            }
            return false;
        }
        case NodeType::NOT_NODE: {
            if (children.size() != 1) {
                 throw std::logic_error("NOT node must have exactly one child.");
            }
            std::map<std::string, std::any> localBindings = bindings;
            return !children[0]->evaluate(facts, localBindings);
        }
        case NodeType::PATTERN_NODE: {
            // !!! CRITICAL DESIGN ISSUE REMAINS !!!
             std::cerr << "Error: Cannot evaluate PATTERN_NODE due to BaseFact<void>* limitations." << std::endl;
            return false;
        }
        default:
             throw std::logic_error("Unknown NodeType encountered in TreeNode::evaluate.");
    }
}


// --- Constraint Constructors Definition ---
Constraint::Constraint(std::string attr, std::any val, std::string var, bool neg)
    : attribute(std::move(attr)), test(std::move(val)), variable(std::move(var)), negate(neg) {}

Constraint::Constraint(std::string attr, std::function<bool(const std::any&)> pred, std::string var, bool neg)
    : attribute(std::move(attr)), test(std::move(pred)), variable(std::move(var)), negate(neg) {}


// --- TreeNode Constructors Definition ---
TreeNode::TreeNode(NodeType type) : type(type) {}
TreeNode::TreeNode(Pattern pattern) : type(NodeType::PATTERN_NODE), pattern(std::move(pattern)) {}


// --- Explicit Template Instantiations ---
// (Removed the BaseFact<void> instantiation)
template bool Pattern::matches<std::vector<std::any>>(const BaseFact<std::vector<std::any>>* fact, std::map<std::string, std::any>& bindings) const;
template bool Pattern::matches<std::vector<std::string>>(const BaseFact<std::vector<std::string>>* fact, std::map<std::string, std::any>& bindings) const;
template bool Pattern::matches<std::vector<int>>(const BaseFact<std::vector<int>>* fact, std::map<std::string, std::any>& bindings) const;
template bool Pattern::matches<std::vector<double>>(const BaseFact<std::vector<double>>* fact, std::map<std::string, std::any>& bindings) const;
template bool Pattern::matches<std::vector<bool>>(const BaseFact<std::vector<bool>>* fact, std::map<std::string, std::any>& bindings) const;
template bool Pattern::matches<std::map<std::string, std::any>>(const BaseFact<std::map<std::string, std::any>>* fact, std::map<std::string, std::any>& bindings) const;
template bool Pattern::matches<std::map<std::string, std::string>>(const BaseFact<std::map<std::string, std::string>>* fact, std::map<std::string, std::any>& bindings) const;
template bool Pattern::matches<std::map<std::string, int>>(const BaseFact<std::map<std::string, int>>* fact, std::map<std::string, std::any>& bindings) const;
template bool Pattern::matches<std::map<std::string, double>>(const BaseFact<std::map<std::string, double>>* fact, std::map<std::string, std::any>& bindings) const;
template bool Pattern::matches<std::map<std::string, bool>>(const BaseFact<std::map<std::string, bool>>* fact, std::map<std::string, std::any>& bindings) const;
template bool Pattern::matches<std::unordered_set<std::any>>(const BaseFact<std::unordered_set<std::any>>* fact, std::map<std::string, std::any>& bindings) const;
template bool Pattern::matches<std::unordered_set<std::string>>(const BaseFact<std::unordered_set<std::string>>* fact, std::map<std::string, std::any>& bindings) const;
template bool Pattern::matches<std::unordered_set<int>>(const BaseFact<std::unordered_set<int>>* fact, std::map<std::string, std::any>& bindings) const;
template bool Pattern::matches<std::unordered_set<double>>(const BaseFact<std::unordered_set<double>>* fact, std::map<std::string, std::any>& bindings) const;
template bool Pattern::matches<std::unordered_set<bool>>(const BaseFact<std::unordered_set<bool>>* fact, std::map<std::string, std::any>& bindings) const;


} // namespace Myrmidon