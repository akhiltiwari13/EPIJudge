#include "test_framework/generic_test.h"
#include <bit>
#include <math.h>
#include <vector>
// ============================================
// Programmer's Attempt
// ============================================

// brute-force and in-place.
unsigned long long ReverseBits(unsigned long long x) { // 16*8 = 128 bits
  // TODO - you fill in here.
  auto i = 0;
  auto j = sizeof(x) * 8 - 1;
  // auto endp = sizeof(x);
  while (i < j) {
    if ((x >> i & 1) != (x >> j & 1)) {
      // default type of 1 is int... this is important while doing bitshift
      // operations.
      x ^= 1ull << i;
      x ^= 1ull << j;
    }
    ++i;
    --j;
  }
  return x;
}

std::vector<unsigned long long> genCache() {
  std::vector<unsigned long long> cache(65536);
  // eopicode/epi_judge_cpp akhil/first_pass  ? ❯
  // ./build/clang-release/reverse_bits Test UNHANDLED EXCEPTION (    1/10000)
  // St12length_error Arguments
  //         x:                 1351510410656
  //
  // Failure info
  //         exception message: vector
  //         explanation:       1351510410656 in binary is
  //         0b10011101010101100010011000100010110100000, and its reverse in
  //         binary is
  //         0b10110100010001100100011010101011100100000000000000000000000
  for (unsigned long long i = 0ULL; i < 65536ULL; i++) {
    cache[i] = ReverseBits(i) >> 48; // moves the reversed bits.
  }
  return cache;
}

// brute-force optimized via caching.
unsigned long long ReverseBitsCached(unsigned long long x) {
  // using 16 bit keys
  // look-up and returns  values for a 64 bit value from the cache.
  // return cache[]; look-up the reverse-bits and return.
  // auto masksz = 16; // 2 bytes
  // auto mask = 0xFFFF genCache(cache);
  // auto result{};
  // result |= x >> 16 return ca;
  auto cache = genCache();
  unsigned int mask_size = 16; // bits
  unsigned long long mask = 0xFFFF;

  unsigned long long result{0};
  result |= cache[(x & mask)];
  result <<= (mask_size);
  result |= cache[(x >> mask_size & mask)];
  result <<= (mask_size);
  result |= cache[(x >> 2 * mask_size & mask)];
  result <<= (mask_size);
  result |= cache[(x >> 3 * mask_size & mask)];
  return result;
}
// notes: create a temp return object with modified bits.
// return precomputed_reverse[x & kBitMask] << (3 * kWordSize) |
//        precomputed_reverse[(x >> kWordSize) & kBitMask] << (2 * kWordSize) |
//        precomputed_reverse[(x >> (2 * kWordSize)) & kBitMask] << kWordSize |
//        precomputed_reverse[(x >> (3 * kWordSize)) & kBitMask];

// ============================================
// Ai's optimized solutions
// ============================================

int main(int argc, char *argv[]) {
  // std::vector<int> cache(static_cast<size_t>(std::pow(2.0f, 32.0f)), 0);
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"x"};
  return GenericTestMain(args, "reverse_bits.cc", "reverse_bits.tsv",
                         &ReverseBitsCached, DefaultComparator{}, param_names);
}

// note on "unsigned long long" :
//
//   The "long long = long + long = 8 + 8" reading is the trap — long long isn't
//   two longs glued together. The name is historical: back when int was 16 bits
//   and long was 32, long long was coined for the next size up, 64 bits. On
//   64-bit Linux (the LP64 model), long already reached 64 bits, so long long
//   had nowhere further to go and the two ended up identical. They're still
//   distinct types for overload resolution and printf formats, just the same
//   size.
