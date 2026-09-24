#include <memory>

#include "binary_tree_node.h"
#include "test_framework/generic_test.h"
using std::unique_ptr;

struct q {
  const unique_ptr<BinaryTreeNode<int>>& tree_node;
  int l, u;
};

bool IsBinaryTreeBST(const unique_ptr<BinaryTreeNode<int>>& tree) {
  // TODO - you fill in here.
  std::queue <q> qu;
  qu.emplace( q{tree, std::numeric_limits<int>:: min(), std::numeric_limits<int>:: max()});

  while (!qu.empty()) {
    if(qu.front().tree_node.get()) {
      if(qu.front().tree_node->data < qu.front().l || qu.front().tree_node->data > qu.front().u) {
        return false;
      }
      qu.emplace(q{qu.front().tree_node->left, qu.front().l, qu.front().tree_node->data});
      qu.emplace(q{qu.front().tree_node->right, qu.front().tree_node->data, qu.front().u});
    }

    qu.pop();
  }
  return true;
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"tree"};
  return GenericTestMain(args, "is_tree_a_bst.cc", "is_tree_a_bst.tsv",
                         &IsBinaryTreeBST, DefaultComparator{}, param_names);
}
