#include "test_framework/generic_test.h"
#include <cmath>
#include <vector>

int SquareRoot(int k) {
  int start = 0, end = k;
  while (start <= end) {
    int mid = start + (end - start) / 2;
    long long sqr = static_cast<long long>(mid) * mid;
    if (sqr <= k) {
      start = mid + 1;
    } else {
      end = mid - 1;
    }
  }
  return start - 1;
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"k"};
  return GenericTestMain(args, "int_square_root.cc", "int_square_root.tsv",
                         &SquareRoot, DefaultComparator{}, param_names);
}
