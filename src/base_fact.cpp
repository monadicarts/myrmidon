// Copyright 2025 Justin Greisiger Frost
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "core/rule.hpp"
#include "myrmidon/base_fact.hpp"
#include <iostream>
#include <type_traits> // For std::remove_pointer_t
#include <stdexcept>   // For std::logic_error
#include <typeinfo>    // For typeid
#include <any>         // For std::any_cast if needed later, and type comparison
#include <functional>  // For std::function
#include <vector>
#include <map>
#include <unordered_set>
#include <string>
#include <optional>    // For optional return from compare_any

// Start the namespace block for the entire file content
namespace Myrmidon {

// Helper function to compare two std::any values
// Returns std::optional<bool>. nullopt if comparison is not possible/defined.
std::optional<bool> compare_any(const std::any& lhs, const std::any& rhs) {
    if (!lhs.has_value() || !rhs.has_value() || lhs.type() != rhs.type()) {
        return std::nullopt; // Cannot compare if types differ or empty
    }

    const std::type_info& type = lhs.type();

    try {
        if (type == typeid(int)) {
            return std::any_cast<int>(lhs) == std::any_cast<int>(rhs);
        } else if (type == typeid(double)) {
            return std::any_cast<double>(lhs) == std::any_cast<double>(rhs);
        } else if (type == typeid(bool)) {
            return std::any_cast<bool>(lhs) == std::any_cast<bool>(rhs);
        } else if (type == typeid(std::string)) {
            return std::any_cast<const std::string&>(lhs) == std::any_cast<const std::string&>(rhs);
        }
        // Add more comparable types as needed
        // else if (type == typeid(MyComparableType)) {
        //     return std::any_cast<const MyComparableType&>(lhs) == std::any_cast<const MyComparableType&>(rhs);
        // }
        else {
            // Type is known but comparison not implemented here
            std::cerr << "Warning: Comparison for type " << type.name() << " not implemented in compare_any." << std::endl;
            return std::nullopt;
        }
    } catch (const std::bad_any_cast& e) {
        // This shouldn't happen if typeid matches, but catch just in case.
        std::cerr << "Warning: bad_any_cast during comparison: " << e.what() << std::endl;
        return std::nullopt;
    }
}


// --- Pattern::matches Definition ---
// No Myrmidon:: prefix needed here anymore
template <typename FactType>
bool Pattern::matches(const BaseFact<FactType>* fact, std::map<std::string, std::any>& bindings) const {
    if (fact == nullptr || fact->getName() != factName) {
        return false;
    }
    const auto& factValues = fact->getValues();
    bool foundMatch = false; // Tracks if the current constraint finds a match

    // --- Vector Handling ---
    if constexpr (std::is_same_v<std::vector<std::any>, FactType> ||
                  std::is_same_v<std::vector<std::string>, FactType> ||
                  std::is_same_v<std::vector<int>, FactType> ||
                  std::is_same_v<std::vector<double>, FactType> ||
                  std::is_same_v<std::vector<bool>, FactType>) {
        if (constraints.empty() && !factValues.empty()) return true; // Match if any elements exist and no constraints
        if (constraints.empty() && factValues.empty()) return true; // Match if no elements and no constraints
        if (!constraints.empty() && factValues.empty()) return false; // Cannot satisfy constraints with no elements

        for (const auto& constraint : constraints) {
            foundMatch = false;
            for (const auto& factValueElement : factValues) {
                bool currentElementMatches = false;
                // Wrap fact element in std::any for consistent comparison/predicate call
                std::any factValueAny = factValueElement;

                // Check if constraint holds a specific value (std::any)
                if (constraint.test.index() == 0) {
                    const auto& constraintValueAny = std::get<std::any>(constraint.test);
                    std::optional<bool> comparisonResult = compare_any(constraintValueAny, factValueAny);
                    // Only proceed if comparison was successful (not nullopt)
                    if (comparisonResult.has_value()) {
                         currentElementMatches = comparisonResult.value();
                    } else {
                        // If compare_any returns nullopt, treat as non-match for this element
                        currentElementMatches = false;
                         // Optional: Log warning if comparison failed for potentially matching types
                         if (constraintValueAny.has_value() && factValueAny.has_value() && constraintValueAny.type() == factValueAny.type()) {
                              std::cerr << "Warning: compare_any returned nullopt for matching types ("
                                        << constraintValueAny.type().name() << ") in vector." << std::endl;
                         }
                    }
                }
                // Check if constraint holds a predicate function
                else if (constraint.test.index() == 1) {
                    // Predicate takes const std::any&
                    currentElementMatches = std::get<std::function<bool(const std::any&)>>(constraint.test)(factValueAny);
                }

                // Apply negation and check for match
                if (constraint.negate ? !currentElementMatches : currentElementMatches) {
                    foundMatch = true;
                    if (!constraint.variable.empty()) { bindings[constraint.variable] = factValueAny; }
                    break; // Found a matching element for this constraint, move to next constraint
                }
            }
            if (!foundMatch) return false; // One constraint failed to find any match in the vector
        }
        return true; // All constraints found at least one match
    }
    // --- Map Handling ---
     else if constexpr (std::is_same_v<std::map<std::string, std::any>, FactType> ||
                         std::is_same_v<std::map<std::string, std::string>, FactType> ||
                         std::is_same_v<std::map<std::string, int>, FactType> ||
                         std::is_same_v<std::map<std::string, double>, FactType> ||
                         std::is_same_v<std::map<std::string, bool>, FactType>) {
        if (constraints.empty() && !factValues.empty()) return true;
        if (constraints.empty() && factValues.empty()) return true;
        if (!constraints.empty() && factValues.empty()) return false;

        for (const auto& constraint : constraints) {
            foundMatch = false;
            // Check if the attribute exists in the map
            auto it = factValues.find(constraint.attribute);
            if (it != factValues.end()) {
                const auto& factValue = it->second;
                // Wrap fact value in std::any for consistent comparison/predicate call
                std::any factValueAny = factValue;
                bool currentAttrMatches = false;

                // Check if constraint holds a specific value (std::any)
                if (constraint.test.index() == 0) {
                     const auto& constraintValueAny = std::get<std::any>(constraint.test);
                     std::optional<bool> comparisonResult = compare_any(constraintValueAny, factValueAny);
                     if (comparisonResult.has_value()) {
                         currentAttrMatches = comparisonResult.value();
                     } else {
                         currentAttrMatches = false;
                         if (constraintValueAny.has_value() && factValueAny.has_value() && constraintValueAny.type() == factValueAny.type()) {
                              std::cerr << "Warning: compare_any returned nullopt for matching types ("
                                        << constraintValueAny.type().name() << ") in map for attribute '"
                                        << constraint.attribute << "'." << std::endl;
                         }
                     }
                }
                // Check if constraint holds a predicate function
                else if (constraint.test.index() == 1) {
                    currentAttrMatches = std::get<std::function<bool(const std::any&)>>(constraint.test)(factValueAny);
                }

                // Apply negation and check for match
                if (constraint.negate ? !currentAttrMatches : currentAttrMatches) {
                    foundMatch = true;
                    if (!constraint.variable.empty()) { bindings[constraint.variable] = factValueAny; }
                    // Don't break here, need to check all constraints against the map
                }
            }
            // If the attribute wasn't found OR the test failed (after negation), this constraint fails
            if (!foundMatch) return false;
        }
        return true; // All constraints matched successfully
    }
    // --- Set Handling ---
     else if constexpr (std::is_same_v<std::unordered_set<std::any>, FactType> ||
                         std::is_same_v<std::unordered_set<std::string>, FactType> ||
                         std::is_same_v<std::unordered_set<int>, FactType> ||
                         std::is_same_v<std::unordered_set<double>, FactType> ||
                         std::is_same_v<std::unordered_set<bool>, FactType>) {
        // Note: unordered_set requires hashable types. std::any is not hashable by default.
        // Instantiating with std::unordered_set<std::any> might require a custom hasher.
        // Assuming it compiles for now, but this is a potential issue.
        if (constraints.empty() && !factValues.empty()) return true;
        if (constraints.empty() && factValues.empty()) return true;
        if (!constraints.empty() && factValues.empty()) return false;

         for (const auto& constraint : constraints) {
             foundMatch = false;
             for (const auto& factValueElement : factValues) {
                 bool currentElementMatches = false;
                 std::any factValueAny = factValueElement;

                 // Check if constraint holds a specific value (std::any)
                 if (constraint.test.index() == 0) {
                    const auto& constraintValueAny = std::get<std::any>(constraint.test);
                    std::optional<bool> comparisonResult = compare_any(constraintValueAny, factValueAny);
                    if (comparisonResult.has_value()) {
                        currentElementMatches = comparisonResult.value();
                    } else {
                        currentElementMatches = false;
                         if (constraintValueAny.has_value() && factValueAny.has_value() && constraintValueAny.type() == factValueAny.type()) {
                              std::cerr << "Warning: compare_any returned nullopt for matching types ("
                                        << constraintValueAny.type().name() << ") in set." << std::endl;
                         }
                    }
                 }
                 // Check if constraint holds a predicate function
                 else if (constraint.test.index() == 1) {
                    currentElementMatches = std::get<std::function<bool(const std::any&)>>(constraint.test)(factValueAny);
                 }

                 // Apply negation and check for match
                 if (constraint.negate ? !currentElementMatches : currentElementMatches) {
                     foundMatch = true;
                     if (!constraint.variable.empty()) { bindings[constraint.variable] = factValueAny; }
                     break; // Found a matching element for this constraint
                 }
             }
             if (!foundMatch) return false; // One constraint failed to find any match in the set
         }
         return true; // All constraints found at least one match
    }
    // else if constexpr (std::is_pointer_v<FactType>) { // Still commented out
    // }

    // If FactType didn't match any of the handled types
    std::cerr << "Warning: Pattern::matches called with unhandled FactType: " << typeid(FactType).name() << std::endl;
    return false;
}


// --- TreeNode::evaluate Implementation ---
// No Myrmidon:: prefix needed
bool TreeNode::evaluate(const std::vector<const BaseFact<void>*>& facts, std::map<std::string, std::any>& bindings) const {
    switch (this->type) {
        case NodeType::AND_NODE: {
            // ... existing code ...
            for (const auto& child : children) {
                if (!child->evaluate(facts, bindings)) {
                    return false;
                }
            }
            return true;
        }
        case NodeType::OR_NODE: {
             // ... existing code ...
             std::map<std::string, std::any> originalBindings = bindings;
            for (const auto& child : children) {
                std::map<std::string, std::any> branchBindings = originalBindings;
                if (child->evaluate(facts, branchBindings)) {
                    // On successful evaluation of an OR branch, merge bindings carefully.
                    // For now, simple overwrite. Consider more complex merge if needed.
                    bindings = std::move(branchBindings);
                    return true;
                }
            }
            // If no OR branch succeeded, restore original bindings.
            bindings = std::move(originalBindings);
            return false;
        }
        case NodeType::NOT_NODE: {
            // ... existing code ...
            if (children.size() != 1) {
                 throw std::logic_error("NOT node must have exactly one child.");
            }
            // NOT node evaluation should not affect bindings outside its scope.
            std::map<std::string, std::any> localBindings = bindings; // Use a copy
            return !children[0]->evaluate(facts, localBindings); // Evaluate with copy
        }
        case NodeType::PATTERN_NODE: {
            // !!! CRITICAL DESIGN ISSUE REMAINS !!!
            // This node cannot be evaluated correctly with BaseFact<void>*.
            // The evaluate function needs access to the specific FactType to call
            // the correctly instantiated Pattern::matches.
            // This requires a significant redesign, possibly involving visitors
            // or type erasure techniques on the BaseFact level, or changing
            // how facts are passed/stored.
            std::cerr << "Error: Cannot evaluate PATTERN_NODE due to BaseFact<void>* limitations. Requires redesign." << std::endl;
             // Returning false as it cannot be evaluated successfully.
            return false;
        }
        default:
             throw std::logic_error("Unknown NodeType encountered in TreeNode::evaluate.");
    }
}


// --- Constraint Constructors Definition ---
// No Myrmidon:: prefix needed
Constraint::Constraint(std::string attr, std::any val, std::string var, bool neg)
    : attribute(std::move(attr)), test(std::move(val)), variable(std::move(var)), negate(neg) {}

// No Myrmidon:: prefix needed
Constraint::Constraint(std::string attr, std::function<bool(const std::any&)> pred, std::string var, bool neg)
    : attribute(std::move(attr)), test(std::move(pred)), variable(std::move(var)), negate(neg) {}


// --- TreeNode Constructors Definition ---
// No Myrmidon:: prefix needed
TreeNode::TreeNode(NodeType type) : type(type) {}

// No Myrmidon:: prefix needed
TreeNode::TreeNode(Pattern pattern) : type(NodeType::PATTERN_NODE), pattern(std::move(pattern)) {}


// --- Explicit Template Instantiations ---
// No Myrmidon:: prefixes needed here either, Pattern and BaseFact are resolved within the namespace

// Note: Instantiating with std::unordered_set<std::any> might fail if std::hash<std::any>
// and std::equal_to<std::any> are not provided or specialized correctly.
// This depends on the C++ standard library implementation and version.
// If you encounter errors here, you might need to avoid std::unordered_set<std::any>
// or provide the necessary hash/equality support.

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
// template bool Pattern::matches<MyDataObject*>(const BaseFact<MyDataObject*>* fact, std::map<std::string, std::any>& bindings) const; // Pointer example


// Close the namespace block at the end of the file
} // namespace Myrmidon