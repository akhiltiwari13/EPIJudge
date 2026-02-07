#include <memory>
#include <vector>

#include "bst_node.h"
#include "test_framework/generic_test.h"
using std::unique_ptr;
using std::vector;

void klarg(const unique_ptr<BstNode<int>> &tree, int k, vector<int> &result) {
  // initial naive recursive implementation.
  // if (result.size() == k) {
  //   return;
  // } else {
  //   klarg(tree->right, k, result);
  //   if (result.size() < k) {
  //     result.push_back(tree->data);
  //   }
  //   klarg(tree->left, k, result);
  // }
  if (tree && result.size() < k) {
    klarg(tree->right, k, result);
    if (result.size() < k) {
      result.emplace_back(tree->data);
    }
    klarg(tree->left, k, result);
  }
}

vector<int> FindKLargestInBST(const unique_ptr<BstNode<int>> &tree, int k) {
  vector<int> result{};
  klarg(tree, k, result);
  return result;
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"tree", "k"};
  return GenericTestMain(args, "k_largest_values_in_bst.cc",
                         "k_largest_values_in_bst.tsv", &FindKLargestInBST,
                         UnorderedComparator{}, param_names);
}
