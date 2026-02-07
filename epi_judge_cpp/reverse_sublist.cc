#include "list_node.h"
#include "test_framework/generic_test.h"
#include <memory>

shared_ptr<ListNode<int>> ReverseSublist(shared_ptr<ListNode<int>> L, int start,
                                         int finish) {
  auto sentinal_node = make_shared<ListNode<int>>(ListNode<int>{0, L});
  auto slhead = sentinal_node;
  int i = 1;
  while (i++ < start) {
    slhead = slhead->next;
  }

  auto sliterator = slhead->next;

  while (start++ < finish) {
    auto tmp = sliterator->next;
    sliterator->next = tmp->next;
    tmp->next = slhead->next;
    slhead->next = tmp;
  }
  return sentinal_node->next;
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"L", "start", "finish"};
  return GenericTestMain(args, "reverse_sublist.cc", "reverse_sublist.tsv",
                         &ReverseSublist, DefaultComparator{}, param_names);
}
