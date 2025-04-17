#ifndef MYRMIDON_CORE_RULE_HPP
#define MYRMIDON_CORE_RULE_HPP

#include <vector>
#include <memory>
#include <any>
#include <map>
#include <functional>
#include <variant>
#include <string>

// Start the namespace HERE
namespace Myrmidon {

// Forward declaration for BaseFact (now inside the namespace)
template <typename Collection>
class BaseFact;

// Constraint struct definition (ensure it's before Pattern)
struct Constraint {
    std::string attribute;
    std::variant<std::any, std::function<bool(const std::any&)>> test;
    std::string variable;
    bool negate;

    // Constructors (declarations only - definitions go in .cpp)
    Constraint(std::string attr, std::any val, std::string var = "", bool neg = false);
    Constraint(std::string attr, std::function<bool(const std::any&)> pred, std::string var = "", bool neg = false);
};

// Pattern struct definition
struct Pattern {
    std::string factName;
    std::vector<Constraint> constraints;

    template <typename FactType>
    bool matches(const BaseFact<FactType>* fact, std::map<std::string, std::any>& bindings) const;
};


// NodeType enum definition
enum class NodeType {
    AND_NODE,
    OR_NODE,
    NOT_NODE,
    PATTERN_NODE
};

// TreeNode struct definition
struct TreeNode { // Renamed to TreeNode
    NodeType type;
    std::vector<std::unique_ptr<TreeNode>> children; // Updated type
    Pattern pattern; // Valid only for PATTERN_NODE

    TreeNode(NodeType type); // Define constructor in .cpp
    TreeNode(Pattern pattern); // Define constructor in .cpp

    bool evaluate(const std::vector<const BaseFact<void>*>& facts, std::map<std::string, std::any>& bindings) const; // Declaration stays here
};

// Rule struct definition
struct Rule {
    std::unique_ptr<TreeNode> lhsTree; // Updated type
    std::vector<std::function<void(const std::vector<const BaseFact<void>*>&)>> rhsActions; // Consider type safety here too
};

// End the namespace HERE
} // namespace Myrmidon

#endif // MYRMIDON_CORE_RULE_HPP