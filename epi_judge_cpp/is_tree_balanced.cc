#include "binary_tree_node.h"
#include "test_framework/generic_test.h"

int checkHeight(const BinaryTreeNode<int> *node) {

  if (node == nullptr) {
  return 0;
}
  int left_height = checkHeight(node->left.get());
  if(left_height ==-1) {return -1;}

  int right_height = checkHeight(node->right.get());
  if(right_height ==-1) {return -1;}

  if(std::abs(left_height-right_height)>1) {
    return -1;
  }

  return std::max(left_height, right_height) +1;

}
bool IsBalanced(const unique_ptr<BinaryTreeNode<int>>& tree) {
  // TODO - you fill in here.
  return checkHeight(tree.get()) != -1;
}



int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"tree"};
  return GenericTestMain(args, "is_tree_balanced.cc", "is_tree_balanced.tsv",
                         &IsBalanced, DefaultComparator{}, param_names);
}
