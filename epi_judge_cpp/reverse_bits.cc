#include "test_framework/generic_test.h"
#include <bit>
#include <vector>

// brute-force and in-place.
unsigned long long ReverseBits(unsigned long long x) { // 16*8 = 128 bits
  // TODO - you fill in here.
  auto i = 0;
  auto j = sizeof(x) * 8 - 1;
  // auto endp = sizeof(x);
  while (i < j) {
    if ((x >> i & 1) != (x >> j & 1)) {
      x ^= 1ull << i;
      x ^= 1ull << j;
    }
    ++i;
    --j;
  }
  return x;
}

// void genCache(std::vector<int> &cache) { return true; }

// brute-force optimized via caching.
// unsigned long long ReverseBitsCached(unsigned long long x,
// std::vector<int> &cache) {
// using 16 bit keys
// look-up and returns  values for a 64 bit value from the cache.
// return cache[]; look-up the reverse-bits and return.
// return 0;
// }

int main(int argc, char *argv[]) {
  std::vector<int> cache(static_cast<size_t>(std::pow(2.0f, 32.0f)), 0);
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"x"};
  return GenericTestMain(args, "reverse_bits.cc", "reverse_bits.tsv",
                         &ReverseBits, DefaultComparator{}, param_names);
}
