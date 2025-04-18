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

#include <gtest/gtest.h>
#include "rule.hpp"                 // Correct include path relative to src/core include dir
#include "myrmidon/base_fact.hpp" // Correct include path relative to include dir
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <any>
#include <functional>

// NOTE (Future Refactoring):
// 1. The use of raw `new` in createListFact can lead to memory leaks in tests.
//    Consider returning std::unique_ptr or creating facts on the stack.
// 2. The `std::vector<const Myrmidon::BaseFact<void>*> facts;` declaration is
//    type-unsafe and prevents proper calling of `Pattern::matches`.
//    This needs to be refactored using a virtual base class (IFact), std::variant,
//    or std::any to hold different fact types polymorphically.
//    Tests using `facts[0]` (currently commented out) will fail or be incorrect
//    until this fundamental design issue is addressed.

// Helper function to create a simple fact (using raw new - see note above)
// Qualify return type
template <typename T>
Myrmidon::BaseFact<std::vector<T>>* createListFact(const std::string& name, const std::vector<T>& values) {
    // Creates a ListFact (which is BaseFact<std::vector<T>>)
    return new Myrmidon::BaseFact<std::vector<T>>(name, values);
}

// Helper function to create a simple pattern
// Qualify Pattern and Constraint
Myrmidon::Pattern createSimplePattern(const std::string& factName, const std::string& attribute, const std::any& value) {
    // Note: Ensure Constraint constructor definition exists in rule.cpp
    return Myrmidon::Pattern{factName, {Myrmidon::Constraint{attribute, value}}};
}

// Helper function to create a predicate function
bool greaterThan(const std::any& value) {
    // Use std::any_cast with pointer check for safety
    if (const int* pval = std::any_cast<int>(&value)) {
        return *pval > 10;
    }
    return false;
}

TEST(ConstraintTest, ValueConstraint) {
    // Qualify Constraint
    Myrmidon::Constraint constraint("age", std::any(30));
    EXPECT_EQ(constraint.attribute, "age");
    ASSERT_EQ(constraint.test.index(), 0); // Should hold std::any
    ASSERT_TRUE(std::get<std::any>(constraint.test).has_value());
    EXPECT_EQ(std::any_cast<int>(std::get<std::any>(constraint.test)), 30);
    EXPECT_EQ(constraint.variable, "");
    EXPECT_FALSE(constraint.negate);
}

TEST(ConstraintTest, PredicateConstraint) {
    // Qualify Constraint, explicitly cast predicate to std::function
    Myrmidon::Constraint constraint("value", std::function<bool(const std::any&)>(greaterThan), "x", true);
    EXPECT_EQ(constraint.attribute, "value");
    EXPECT_TRUE(std::holds_alternative<std::function<bool(const std::any&)>>(constraint.test));
    EXPECT_EQ(constraint.variable, "x");
    EXPECT_TRUE(constraint.negate);
    // Test the predicate itself
    auto pred_func = std::get<std::function<bool(const std::any&)>>(constraint.test);
    EXPECT_TRUE(pred_func(std::any(15)));
    EXPECT_FALSE(pred_func(std::any(5)));
}

// Tests involving Pattern::matches are commented out due to the BaseFact<void>* issue
/*
TEST(PatternTest, MatchesSimpleFact) {
    // FIXME: Vector type BaseFact<void>* is problematic
    // std::vector<const Myrmidon::BaseFact<void>*> facts; // Incorrect type
    Myrmidon::ListFact<std::string>* fact_ptr = createListFact("person", std::vector<std::string>{"Alice"});
    // facts.push_back(fact_ptr); // Cannot push derived into void* base vector this way

    Myrmidon::Pattern pattern = createSimplePattern("person", "", std::any(std::string("Alice")));
    std::map<std::string, std::any> bindings;
    // FIXME: Cannot call matches correctly yet. Needs specific type or virtual call.
    // ASSERT_TRUE(pattern.matches(fact_ptr, bindings)); // Call with specific type for now?

    delete fact_ptr; // Manual cleanup needed for raw new
}

// ... other commented-out Pattern tests ...

*/

// Tests for TreeNode need qualification and potentially revision based on evaluate implementation
TEST(TreeNodeTest, CreatePatternNode) {
    // Qualify Pattern, TreeNode, NodeType
    Myrmidon::Pattern pattern = createSimplePattern("person", "", std::any(std::string("Alice")));
    Myrmidon::TreeNode node(pattern); // Uses Pattern constructor
    ASSERT_EQ(node.type, Myrmidon::NodeType::PATTERN_NODE);
    ASSERT_EQ(node.pattern.factName, "person");
}

TEST(TreeNodeTest, CreateAndNode) {
    // Qualify TreeNode and NodeType
    Myrmidon::TreeNode node(Myrmidon::NodeType::AND_NODE); // Uses NodeType constructor
    ASSERT_EQ(node.type, Myrmidon::NodeType::AND_NODE);
    ASSERT_TRUE(node.children.empty());
}

TEST(TreeNodeTest, CreateOrNode) {
     // Qualify TreeNode and NodeType
    Myrmidon::TreeNode node(Myrmidon::NodeType::OR_NODE);
    ASSERT_EQ(node.type, Myrmidon::NodeType::OR_NODE);
    ASSERT_TRUE(node.children.empty());
}

TEST(TreeNodeTest, CreateNotNode) {
     // Qualify TreeNode and NodeType
    Myrmidon::TreeNode node(Myrmidon::NodeType::NOT_NODE);
    ASSERT_EQ(node.type, Myrmidon::NodeType::NOT_NODE);
    ASSERT_TRUE(node.children.empty());
}