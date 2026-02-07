#include <algorithm>
#include <iterator>
#include <queue>
#include <vector>

#include "test_framework/generic_test.h"
using std::vector;

struct element {
  vector<int>::const_iterator current;
  vector<int>::const_iterator end;

  // bool operator>(const element &rhs) const {
  bool operator>(const element &rhs) { return *this->current > *rhs.current; }
};

vector<int> MergeSortedArrays(const vector<vector<int>> &sorted_arrays) {
  std::priority_queue<element, vector<element>, std::greater<>> min_heap;

  for (const auto &vec : sorted_arrays) {
    min_heap.emplace(element{vec.cbegin(), vec.cend()});
  }

  std::vector<int> result{};
  while (!min_heap.empty()) {
    auto el = min_heap.top();
    min_heap.pop();
    result.emplace_back(*(el.current));
    if (next(el.current) != el.end)
      min_heap.emplace(element{std::next(el.current), el.end});
  }
  return result;
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"sorted_arrays"};
  return GenericTestMain(args, "sorted_arrays_merge.cc",
                         "sorted_arrays_merge.tsv", &MergeSortedArrays,
                         DefaultComparator{}, param_names);
}
