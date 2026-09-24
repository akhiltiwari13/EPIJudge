#include <memory>
#include <vector>

#include "binary_tree_node.h"
#include "test_framework/generic_test.h"
using std::unique_ptr;
using std::vector;

vector<vector<int>> BinaryTreeDepthOrder(
    const unique_ptr<BinaryTreeNode<int>>& tree) {
  // TODO - you fill in here.
  std::queue<BinaryTreeNode<int>*> q;
  q.emplace(tree.get());
  vector<vector<int>> result;

  while (!q.empty()) {
    int level_size = q.size();
    vector<int> one_level;

    for(int i=0; i< level_size; i++) {
      auto current = q.front();
      q.pop();
      one_level.push_back(current->data);
      if(current->left){
      q.emplace(current->left.get());
      }
      if(current->right) {
        q.emplace(current->right.get());
      }
    }
    result.push_back(one_level);

  }
  return result;
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"tree"};
  return GenericTestMain(args, "tree_level_order.cc", "tree_level_order.tsv",
                         &BinaryTreeDepthOrder, DefaultComparator{},
                         param_names);
}
