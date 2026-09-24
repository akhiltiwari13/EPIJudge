#include "binary_tree_with_parent_prototype.h"
#include "test_framework/binary_tree_utils.h"
#include "test_framework/generic_test.h"
#include "test_framework/test_failure.h"
#include "test_framework/timed_executor.h"

int GetDepth (const BinaryTreeNode<int>* node ) {
  int depth =0;
  while (node) {
    depth ++;
    node= node->parent;
  }
  return depth;
}

BinaryTreeNode<int>* Lca(const unique_ptr<BinaryTreeNode<int>>& node0,
                         const unique_ptr<BinaryTreeNode<int>>& node1) {
  // TODO - you fill in here.
  auto * itr0= node0.get(), *itr1= node1.get();
  int depth0= GetDepth(itr0);
  int depth1= GetDepth(itr1);

  if(depth1>depth0) {
    std::swap(itr0, itr1);
  }
 int depth_diff = std::abs(depth0-depth1);
 while (depth_diff){
  itr0 = itr0->parent;
  depth_diff--;
 }

 while(itr0 != itr1){
  itr0=itr0->parent;
  itr1=itr1->parent;
 }
  return itr0;
}
int LcaWrapper(TimedExecutor& executor,
               const unique_ptr<BinaryTreeNode<int>>& tree, int key0,
               int key1) {
  const unique_ptr<BinaryTreeNode<int>>& node0 = MustFindNode(tree, key0);
  const unique_ptr<BinaryTreeNode<int>>& node1 = MustFindNode(tree, key1);

  auto result = executor.Run([&] { return Lca(node0, node1); });

  if (!result) {
    throw TestFailure("Result can not be nullptr");
  }
  return result->data;
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"executor", "tree", "key0", "key1"};
  return GenericTestMain(args, "lowest_common_ancestor_with_parent.cc",
                         "lowest_common_ancestor.tsv", &LcaWrapper,
                         DefaultComparator{}, param_names);
}
