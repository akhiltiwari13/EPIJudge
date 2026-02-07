#include "list_node.h"
#include "test_framework/generic_test.h"
#include <memory>
void appendNode(shared_ptr<ListNode<int>> &node,
                shared_ptr<ListNode<int>> &tail) {

  tail->next = node;
  tail = node;
  node = node->next;
}

shared_ptr<ListNode<int>> MergeTwoSortedLists(shared_ptr<ListNode<int>> L1,
                                              shared_ptr<ListNode<int>> L2) {
  auto sentinal_node = make_shared<ListNode<int>>();
  auto tail = sentinal_node;
  tail->next = nullptr;

  while (L1 && L2) {
    appendNode(L1->data > L2->data ? L2 : L1, tail);
  }

  // append remaining nodes
  tail->next = L1 ? L1 : L2;

  return sentinal_node->next;
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"L1", "L2"};
  return GenericTestMain(args, "sorted_lists_merge.cc",
                         "sorted_lists_merge.tsv", &MergeTwoSortedLists,
                         DefaultComparator{}, param_names);
}
