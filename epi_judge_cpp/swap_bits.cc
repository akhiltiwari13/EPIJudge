#include "test_framework/generic_test.h"
long long SwapBits(long long x, int i, int j) {
  // TODO - you fill in here.
  auto shouldswap = (1 & (x >> i)) != (1 & (x >> j)) ? true : false;
  if (shouldswap) {
    auto bitmask = (1L << i) | (1L << j);
    x ^= bitmask;
  }
  return x;
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"x", "i", "j"};
  return GenericTestMain(args, "swap_bits.cc", "swap_bits.tsv", &SwapBits,
                         DefaultComparator{}, param_names);
}
