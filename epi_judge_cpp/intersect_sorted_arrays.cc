#include <algorithm>
#include <iterator>
#include <vector>

#include "test_framework/generic_test.h"
using std::vector;

// TODO: find the usage for std::back_inserter(Container &x)?
// vector<int> IntersectTwoSortedArraysUsingSTL(const vector<int> &A,
//                                              const vector<int> &B) {
//   std::set sa(A.begin(), A.end());
//   std::set sb(B.begin(), B.end());
//   std::vector<int> result;
//   auto end_Itr =
//       std::set_intersection(std::begin(sa), std::end(sa), std::begin(sb),
//                             std::end(sb), std::back_inserter(result));
//   return result;
// }

vector<int> IntersectTwoSortedArrays(const vector<int> &A,
                                     const vector<int> &B) {
  vector<int> result;
  int i = 0, j = 0;
  while (i < size(A) && j < size(B)) {
    if ((A[i] == B[j]) && (!i || (A[i] != A[i - 1]))) {
      result.emplace_back(A[i]);
      ++i;
      ++j;
    } else if (A[i] < B[j]) {
      ++i;
    } else {
      ++j;
    }
  }
  return result;
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"A", "B"};
  return GenericTestMain(
      args, "intersect_sorted_arrays.cc", "intersect_sorted_arrays.tsv",
      &IntersectTwoSortedArrays, DefaultComparator{}, param_names);
}
