#include "list_node.h"
#include "test_framework/generic_test.h"

shared_ptr<ListNode<int>> ReverseSublist(shared_ptr<ListNode<int>> L, int start,
                                         int finish) {
  // TODO - you fill in here.
  shared_ptr<ListNode<int>> dummy_head = make_shared<ListNode<int>>(ListNode<int>{0,L});
  auto sublist = dummy_head;
  int k =1;
  while (k++< start) {
    sublist = sublist->next;
  }

  auto itr = sublist->next;
  while(start++<finish) {
    auto tmp = itr->next;
    itr->next = tmp->next;
    tmp->next = sublist->next;
    sublist->next = tmp;
  }
  return dummy_head->next;
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"L", "start", "finish"};
  return GenericTestMain(args, "reverse_sublist.cc", "reverse_sublist.tsv",
                         &ReverseSublist, DefaultComparator{}, param_names);
}
