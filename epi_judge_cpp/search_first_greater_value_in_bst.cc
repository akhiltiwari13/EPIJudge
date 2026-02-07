#include <memory>

#include "bst_node.h"
#include "test_framework/generic_test.h"
using std::unique_ptr;

// recursive approach
BstNode<int> *FindFirstGreaterThanK(const unique_ptr<BstNode<int>> &tree,
                                    int k) {
  if (!tree)
    return nullptr;
  if (tree->data > k) {
    // first node greater than k.
    auto left_leader = FindFirstGreaterThanK(tree->left, k);
    return left_leader ? left_leader : tree.get();
  } else {
    return FindFirstGreaterThanK(tree->right, k);
  }
}

BstNode<int> *
FindFirstGreaterThanKIterative(const unique_ptr<BstNode<int>> &tree, int k) {
  if (!tree)
    return nullptr;
  BstNode<int> *result_node = nullptr;
  auto tree_node = tree.get();
  while (tree_node) {
    if (tree_node->data > k) {
      result_node = tree_node;
      tree_node = tree_node->left.get();
    } else {
      tree_node = tree_node->right.get();
    }
  }
  return result_node;
}

int FindFirstGreaterThanKWrapper(const unique_ptr<BstNode<int>> &tree, int k) {
  auto result = FindFirstGreaterThanK(tree, k);
  return result ? result->data : -1;
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"tree", "k"};
  return GenericTestMain(args, "search_first_greater_value_in_bst.cc",
                         "search_first_greater_value_in_bst.tsv",
                         &FindFirstGreaterThanKWrapper, DefaultComparator{},
                         param_names);
}
