#include "binary_tree_node.h"
#include "test_framework/generic_test.h"
#include <algorithm>

struct BalanceStat {
  bool is_bal{false};
  int64_t height{0};
};

BalanceStat checkBalance(const unique_ptr<BinaryTreeNode<int>> &tree) {

  if (!tree)
    return BalanceStat{true, -1};

  auto left = checkBalance(tree->left);
  auto right = checkBalance(tree->right);
  auto subtree_bal =
      left.is_bal && right.is_bal && std::abs(left.height - right.height) <= 1;
  return BalanceStat{subtree_bal, (std::max(left.height, right.height) + 1)};
}

bool IsBalanced(const unique_ptr<BinaryTreeNode<int>> &tree) {
  return checkBalance(tree).is_bal;
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"tree"};
  return GenericTestMain(args, "is_tree_balanced.cc", "is_tree_balanced.tsv",
                         &IsBalanced, DefaultComparator{}, param_names);
}
