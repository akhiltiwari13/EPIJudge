#include <algorithm>
#include <iterator>
#include <vector>

#include "test_framework/generic_test.h"
using std::vector;

// int SearchFirstOfK(const vector<int> &A, int k) {
//   // TODO - you fill in here.
//   auto resitr = std::find(begin(A), end(A), k);
//   return resitr == A.end() ? -1 : resitr - A.begin();
//   // return 0;
// }

int SearchFirstOfK(const vector<int> &A, int k) {
  int start = 0, end = A.size() - 1;
  auto result{-1};
  while (start <= end) {
    auto mid = start + (end - start) / 2;
    if (A[mid] == k) {
      result = mid;
      end = mid - 1; // don't know why the fuck this is needed.
    } else if (A[mid] < k) {
      start = mid + 1;
    } else {
      end = mid - 1;
    }
  }
  return result;
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"A", "k"};
  return GenericTestMain(args, "search_first_key.cc", "search_first_key.tsv",
                         &SearchFirstOfK, DefaultComparator{}, param_names);
}
