#include "test_framework/generic_test.h"

int SquareRoot(int k) {
  // TODO - you fill in here.
  int l=0, r=k;
  while (l<=r) {
    int mid = (l+ (r-l)/2);
    long long mid_sq = 1LL *mid*mid;
    if(mid_sq==k) {
      return mid;
    }
    else if (mid_sq<k){
      l = mid+1;
    }
    else {
      r= mid-1;
    }
  }
  return r;
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"k"};
  return GenericTestMain(args, "int_square_root.cc", "int_square_root.tsv",
                         &SquareRoot, DefaultComparator{}, param_names);
}
