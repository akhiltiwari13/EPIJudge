#include <algorithm>
#include <limits>
#include <memory>
#include <queue>
#include <tuple>

#include "binary_tree_node.h"
#include "test_framework/generic_test.h"
using std::unique_ptr;

int max_val(const unique_ptr<BinaryTreeNode<int>> &node) {
  if (node == nullptr) {
    return std::numeric_limits<int>::min();
  } else
    return std::max(node->data,
                    std::max(max_val(node->right), max_val(node->left)));
}
int min_val(const unique_ptr<BinaryTreeNode<int>> &node) {
  if (node == nullptr) {
    return std::numeric_limits<int>::max();
    // return 0; should instead return the lowest possible value.
  } else
    return std::min(node->data,
                    std::min(min_val(node->right), min_val(node->left)));
}
bool validateBSTNode(const unique_ptr<BinaryTreeNode<int>> &node) {
  if (node == nullptr) {
    return true;
  } else {
    return (node->data <= min_val(node->right) &&
            node->data >= max_val(node->left));
  }
}
// The argument to the test function is a const unique_ptr.
bool IsBinaryTreeBSTBruteforce(const unique_ptr<BinaryTreeNode<int>> &tree) {
  if (tree == nullptr) {
    return true;
  } else
    return validateBSTNode(tree) && IsBinaryTreeBSTBruteforce(tree->left) &&
           IsBinaryTreeBSTBruteforce(tree->right);
  // return true;
}

bool areValuesInRange(const unique_ptr<BinaryTreeNode<int>> &node, int minval,
                      int maxval) {
  if (!node) {
    return true;

  } else {
    return (node->data >= minval && node->data <= maxval)
               ? (areValuesInRange(node->left, minval, node->data) &&
                  areValuesInRange(node->right, node->data, maxval))
               : false;
  }
}

bool IsBinaryTreeBSTDFSRange(const unique_ptr<BinaryTreeNode<int>> &tree) {
  if (tree == nullptr) {
    return true;
  } else
    return areValuesInRange(tree, std::numeric_limits<int>::min(),
                            std::numeric_limits<int>::max());
}

// using a tuple instead.
struct qElmnt {
  const std::unique_ptr<BinaryTreeNode<int>> &node;
  int allowed_min_val;
  int allowed_max_val;
};

bool IsBinaryTreeBST(const unique_ptr<BinaryTreeNode<int>> &tree) {
  if (tree == nullptr) {
    return true;
  } else {
    std::queue<qElmnt> q;
    q.emplace(qElmnt{tree, std::numeric_limits<int>::min(),
                     std::numeric_limits<int>::max()});

    while (!q.empty()) {
      auto val = q.front();
      // if the range check fails for the current node then return false else
      // populate the queue with it's children and their possible ranges.
      if (val.node->data < val.allowed_min_val ||
          val.node->data > val.allowed_max_val) {
        return false;
      } else {
        if (val.node->left) { // these checks are important to avoid seg faults.
          q.push(qElmnt{val.node->left, val.allowed_min_val, val.node->data});
        }
        if (val.node->right) {
          q.push(qElmnt{val.node->right, val.node->data, val.allowed_max_val});
        }
        q.pop();
      }
    }
  }
  return true;
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"tree"};
  return GenericTestMain(args, "is_tree_a_bst.cc", "is_tree_a_bst.tsv",
                         &IsBinaryTreeBST, DefaultComparator{}, param_names);
}
