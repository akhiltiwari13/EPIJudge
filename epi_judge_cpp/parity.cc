#include "test_framework/generic_test.h"
#include <bit>

// brute-force
// given n: len(input)
// TC: O(n)
short ParityBruteforce(unsigned long long x) {
  short count = 0;
  // unsigned long long op{1ULL};

  while (x) {
    count ^= (x & 1); // cool-bit-trick to alternate a flag on each execution.
    x >>= 1;
  }
  return count;
}

// optimized bruteforce by dropping lowest set bit.
// given k: setbits(input)
// TC: O(k)
short ParityByDroppingLowestSetBit(unsigned long long x) {
  short count = 0;
  // unsigned long long op{1ULL};

  while (x) {
    count ^= 1; // cool-bit-trick to alternate a flag on each execution.
    // drops x's last set bit.
    x &= (x - 1);
  }
  return count;
}

// XOR of a group of bits is it's parity.
// given n: len(input)
// TC: O(log(n))
short ParityUsingXor(unsigned long long x) { // unsigned long long: 64 bits.
  x ^= x >> 32;
  x ^= x >> 16;
  x ^= x >> 8;
  x ^= x >> 4;
  x ^= x >> 2;
  x ^= x >> 1;
  return x & 1;
}

short ParityUsingStdLib(unsigned long long x) { return std::__popcount(x) % 2; }

// useful when repeated parity calculations are required for variuos inputs.
// TC: O(n/l) l: total cache lookups.
short ParityViaCache(unsigned long long x) {
  std::vector<unsigned long long> precomp_cache{1, 2, 3, 4};
  constexpr int WORD_SIZE{16};
  constexpr int BIT_MASK{0xFFFF};
  return precomp_cache[x >> (3 * WORD_SIZE)] ^
         precomp_cache[(x >> (2 * WORD_SIZE)) & BIT_MASK] ^
         precomp_cache[x >> WORD_SIZE & BIT_MASK] ^
         precomp_cache[x >> (3 * WORD_SIZE) & BIT_MASK];
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"x"};
  return GenericTestMain(args, "parity.cc", "parity.tsv", &ParityUsingXor,
                         DefaultComparator{}, param_names);
}
