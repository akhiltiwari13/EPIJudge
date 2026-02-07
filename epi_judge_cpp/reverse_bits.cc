#include "test_framework/generic_test.h"
#include <bit>
#include <vector>

// brute-force
unsigned long long ReverseBits(unsigned long long x) {
  // TODO - you fill in here.

  return 0;
}

// brute-force optimized via caching.
unsigned long long ReverseBitsCached(unsigned long long x,
                                     std::vector<int> &cache) {
  // using 32 bit keys
  // look-up and returns  values for a 64 bit value from the cache.
  // return cache[]; look-up the reverse-bits and return.
  return 0;
}

// brute-force
bool genCache(std::vector<int> &cache) { return true; }

int main(int argc, char *argv[]) {
  std::vector<int> cache(static_cast<size_t>(std::pow(2.0f, 32.0f)), 0);
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"x"};
  return GenericTestMain(args, "reverse_bits.cc", "reverse_bits.tsv",
                         &ReverseBits, DefaultComparator{}, param_names);
}
