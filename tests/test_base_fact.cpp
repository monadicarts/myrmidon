#include <gtest/gtest.h>
#include "rule_engine/base_fact.hpp" // Include the header under test

#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <memory> // For std::unique_ptr if managing TestObject lifetime

// --- Test Fixture or Helper Struct ---
// Define the dummy object for RefFact tests
struct TestObject {
    int id;
    std::string data;

    // Optional: Add equality operator if needed for specific tests,
    // but BaseFact::equals for RefFact compares pointers by default.
    bool operator==(const TestObject& other) const {
        return id == other.id && data == other.data;
    }
};

// Use the RuleEngine namespace to avoid verbose qualification
using namespace RuleEngine;

// --- Test Cases ---

// Test Suite for Basic Construction and Getters
TEST(BaseFactTest, ConstructionAndGetters) {
    // ListFact
    std::vector<int> intVec = {1, 2, 3};
    ListFact<int> listFact("intList", intVec);
    ASSERT_EQ(listFact.getName(), "intList");
    ASSERT_EQ(listFact.getValues(), intVec);

    // MapFact
    std::map<std::string, double> doubleMap = {{"pi", 3.14}, {"e", 2.71}};
    MapFact<std::string, double> mapFact("constants", doubleMap);
    ASSERT_EQ(mapFact.getName(), "constants");
    ASSERT_EQ(mapFact.getValues(), doubleMap);

    // SetFact
    std::unordered_set<std::string> stringSet = {"apple", "banana"};
    SetFact<std::string> setFact("fruits", stringSet);
    ASSERT_EQ(setFact.getName(), "fruits");
    ASSERT_EQ(setFact.getValues(), stringSet);

    // ValueFact (internally a vector of size 1)
    ValueFact<bool> boolFact("isEnabled", {true}); // Construct with initializer list for vector
    ASSERT_EQ(boolFact.getName(), "isEnabled");
    ASSERT_EQ(boolFact.getValues().size(), 1);
    ASSERT_TRUE(boolFact.getValues()[0]);

    // RefFact
    TestObject obj1 = {101, "TestData"};
    RefFact<TestObject> refFact("objectPtr", &obj1);
    ASSERT_EQ(refFact.getName(), "objectPtr");
    ASSERT_EQ(refFact.getValues(), &obj1); // Check if pointer value is stored
    ASSERT_EQ(refFact.getValues()->id, 101); // Check dereferencing
}

// Test Suite for the equals() method
TEST(BaseFactTest, EqualsComparison) {
    // --- ListFact ---
    ListFact<int> listFact1("list1", {1, 2});
    ListFact<int> listFact2("list1", {1, 2}); // Same name, same values
    ListFact<int> listFact3("list1", {1, 3}); // Same name, different values
    ListFact<int> listFact4("list2", {1, 2}); // Different name, same values

    ASSERT_TRUE(listFact1.equals(&listFact2));
    ASSERT_FALSE(listFact1.equals(&listFact3));
    ASSERT_FALSE(listFact1.equals(&listFact4));
    ASSERT_FALSE(listFact1.equals(nullptr)); // Test null comparison

    // --- MapFact ---
    MapFact<std::string, int> mapFact1("map1", {{"a", 1}, {"b", 2}});
    MapFact<std::string, int> mapFact2("map1", {{"a", 1}, {"b", 2}});
    MapFact<std::string, int> mapFact3("map1", {{"a", 1}, {"c", 3}});
    MapFact<std::string, int> mapFact4("map2", {{"a", 1}, {"b", 2}});

    ASSERT_TRUE(mapFact1.equals(&mapFact2));
    ASSERT_FALSE(mapFact1.equals(&mapFact3));
    ASSERT_FALSE(mapFact1.equals(&mapFact4));
    ASSERT_FALSE(mapFact1.equals(nullptr));

    // --- SetFact ---
    SetFact<int> setFact1("set1", {10, 20});
    SetFact<int> setFact2("set1", {20, 10}); // Order doesn't matter for unordered_set comparison
    SetFact<int> setFact3("set1", {10, 30});
    SetFact<int> setFact4("set2", {10, 20});

    ASSERT_TRUE(setFact1.equals(&setFact2)); // Should be true due to unordered_set equality
    ASSERT_FALSE(setFact1.equals(&setFact3));
    ASSERT_FALSE(setFact1.equals(&setFact4));
    ASSERT_FALSE(setFact1.equals(nullptr));

    // --- ValueFact ---
    ValueFact<double> valFact1("val1", {3.14});
    ValueFact<double> valFact2("val1", {3.14});
    ValueFact<double> valFact3("val1", {2.71});
    ValueFact<double> valFact4("val2", {3.14});

    ASSERT_TRUE(valFact1.equals(&valFact2));
    ASSERT_FALSE(valFact1.equals(&valFact3));
    ASSERT_FALSE(valFact1.equals(&valFact4));
    ASSERT_FALSE(valFact1.equals(nullptr));

    // --- RefFact ---
    TestObject objA = {1, "A"};
    TestObject objB = {2, "B"};
    TestObject objA_copy = {1, "A"}; // Same content, different object

    RefFact<TestObject> refFactA1("refA", &objA);
    RefFact<TestObject> refFactA2("refA", &objA);    // Same name, same pointer
    RefFact<TestObject> refFactB("refA", &objB);     // Same name, different pointer
    RefFact<TestObject> refFactA_copy("refA", &objA_copy); // Same name, different pointer (same content)
    RefFact<TestObject> refFactA_diffName("refA_other", &objA); // Different name, same pointer

    ASSERT_TRUE(refFactA1.equals(&refFactA2));      // Compares name and pointer value
    ASSERT_FALSE(refFactA1.equals(&refFactB));
    ASSERT_FALSE(refFactA1.equals(&refFactA_copy)); // False because pointers differ
    ASSERT_FALSE(refFactA1.equals(&refFactA_diffName));
    ASSERT_FALSE(refFactA1.equals(nullptr));

    RefFact<TestObject> refFactNull("refNull", nullptr);
    RefFact<TestObject> refFactNull2("refNull", nullptr);
    ASSERT_TRUE(refFactNull.equals(&refFactNull2)); // Null pointers compare equal if names match
    ASSERT_FALSE(refFactA1.equals(&refFactNull));
    ASSERT_FALSE(refFactNull.equals(&refFactA1));
}

// Test Suite for the isEqualUntyped() method (basic default behavior)
TEST(BaseFactTest, IsEqualUntypedComparison) {
    ListFact<int> listFact("myFact", {1, 2, 3});

    // Create a compatible BaseFact for the parameter type
    // Note: Constructing BaseFact<std::vector<std::any>> directly is less common,
    // this test primarily checks the default name comparison logic.
    std::vector<std::any> anyVec; // Content doesn't matter for default comparison
    BaseFact<std::vector<std::any>> untypedFactSameName("myFact", anyVec);
    BaseFact<std::vector<std::any>> untypedFactDiffName("otherFact", anyVec);

    // Default implementation compares names
    ASSERT_TRUE(listFact.isEqualUntyped(&untypedFactSameName));
    ASSERT_FALSE(listFact.isEqualUntyped(&untypedFactDiffName));
    ASSERT_FALSE(listFact.isEqualUntyped(nullptr)); // Test null comparison
}

// Optional: Test Move Semantics
TEST(BaseFactTest, MoveSemantics) {
    // Move Construction
    ListFact<std::string> fact1("original", {"a", "b"});
    ListFact<std::string> fact2(std::move(fact1));

    ASSERT_EQ(fact2.getName(), "original");
    ASSERT_EQ(fact2.getValues().size(), 2);
    ASSERT_EQ(fact2.getValues()[0], "a");
    // Check state of moved-from object (usually name empty, values empty)
    // ASSERT_TRUE(fact1.getName().empty()); // Exact state depends on std::string move
    ASSERT_TRUE(fact1.getValues().empty()); // std::vector guarantees empty after move

    // Move Assignment
    MapFact<int, int> fact3("map1", {{1, 10}});
    MapFact<int, int> fact4("map2", {{2, 20}});
    fact4 = std::move(fact3);

    ASSERT_EQ(fact4.getName(), "map1");
    ASSERT_EQ(fact4.getValues().size(), 1);
    ASSERT_EQ(fact4.getValues().at(1), 10);
    // Check state of moved-from object
    ASSERT_TRUE(fact3.getValues().empty());
}

// Test edge cases like empty collections
TEST(BaseFactTest, EmptyCollections) {
    ListFact<int> emptyList("emptyList", {});
    ASSERT_EQ(emptyList.getName(), "emptyList");
    ASSERT_TRUE(emptyList.getValues().empty());

    MapFact<std::string, int> emptyMap("emptyMap", {});
    ASSERT_EQ(emptyMap.getName(), "emptyMap");
    ASSERT_TRUE(emptyMap.getValues().empty());

    SetFact<double> emptySet("emptySet", {});
    ASSERT_EQ(emptySet.getName(), "emptySet");
    ASSERT_TRUE(emptySet.getValues().empty());

    // ValueFact requires a vector, cannot be truly "empty" in the same sense
    // RefFact with nullptr
    RefFact<TestObject> nullRef("nullRef", nullptr);
    ASSERT_EQ(nullRef.getName(), "nullRef");
    ASSERT_EQ(nullRef.getValues(), nullptr);
}
