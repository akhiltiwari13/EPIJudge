#include "binary_tree_with_parent_prototype.h"
#include "test_framework/binary_tree_utils.h"
#include "test_framework/generic_test.h"
#include "test_framework/test_failure.h"
#include "test_framework/timed_executor.h"
#include <cstdlib>
#include <utility>

// height is the distance from the leaf node... depth is distance from the root.
int getDepth(BinaryTreeNode<int> *nodeptr) {
  if (!nodeptr)
    return 0;
  else
    return (1 + getDepth(nodeptr->parent));
}

BinaryTreeNode<int> *Lca(const unique_ptr<BinaryTreeNode<int>> &node0,
                         const unique_ptr<BinaryTreeNode<int>> &node1) {
  // TODO - you fill in here.
  auto n0ptr = node0.get();
  auto n1ptr = node1.get();
  auto h0 = getDepth(n0ptr);
  auto h1 = getDepth(n1ptr);

  auto diff = abs(h0 - h1);
  if (h0 < h1) {
    std::swap(n0ptr, n1ptr);
  }

  while (diff--) {
    n0ptr = n0ptr->parent;
  }

  // now both pointers can be moved up in tandom.
  while (n0ptr != n1ptr) {
    n0ptr = n0ptr->parent;
    n1ptr = n1ptr->parent;
  }
  return n0ptr;
}
int LcaWrapper(TimedExecutor &executor,
               const unique_ptr<BinaryTreeNode<int>> &tree, int key0,
               int key1) {
  const unique_ptr<BinaryTreeNode<int>> &node0 = MustFindNode(tree, key0);
  const unique_ptr<BinaryTreeNode<int>> &node1 = MustFindNode(tree, key1);

  auto result = executor.Run([&] { return Lca(node0, node1); });

  if (!result) {
    throw TestFailure("Result can not be nullptr");
  }
  return result->data;
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"executor", "tree", "key0", "key1"};
  return GenericTestMain(args, "lowest_common_ancestor_with_parent.cc",
                         "lowest_common_ancestor.tsv", &LcaWrapper,
                         DefaultComparator{}, param_names);
}
