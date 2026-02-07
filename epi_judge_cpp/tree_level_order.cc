#include <memory>
#include <queue>
#include <vector>

#include "binary_tree_node.h"
#include "test_framework/generic_test.h"
using std::unique_ptr;
using std::vector;

vector<vector<int>>
BinaryTreeDepthOrder(const unique_ptr<BinaryTreeNode<int>> &tree) {

  if (!tree.get()) {
    return {};
  } else {
    std::queue<BinaryTreeNode<int> *> curr_level{};
    std::vector<vector<int>> result{};
    curr_level.emplace(tree.get());

    while (!curr_level.empty()) {

      std::queue<BinaryTreeNode<int> *> child_level{};
      std::vector<int> peer_nodes{};

      while (!curr_level.empty()) {
        auto tmp = curr_level.front();
        // front() return nullptr when it's empty.
        curr_level.pop();
        if (tmp) {
          peer_nodes.emplace_back(tmp->data);
          child_level.emplace(tmp->left.get());
          child_level.emplace(tmp->right.get());
        }
      }
      if (!peer_nodes.empty()) {
        result.push_back(peer_nodes);
      }
      curr_level = child_level;
    }
    return result;
  }
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"tree"};
  return GenericTestMain(args, "tree_level_order.cc", "tree_level_order.tsv",
                         &BinaryTreeDepthOrder, DefaultComparator{},
                         param_names);
}
