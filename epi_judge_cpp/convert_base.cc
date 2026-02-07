#include <algorithm>
#include <cctype>
#include <iterator>
#include <numeric>
#include <string>

#include "test_framework/generic_test.h"
using std::string;

// helper definition.
string ConvertBaseStr(long num, int base) {
  return num == 0 ? ""
                  : ConvertBaseStr((num / base), base) +
                        (static_cast<char>((num % base) >= 10
                                               ? 'A' + ((num % base) - 10)
                                               : num % base + '0'));
}
// problem statment
string ConvertBase(const string &num_str, int b1, int b2) {

  // parse the num_str and crete it's decimal equivalent. s0, s1, s2,
  // s3.....sn... chars etc [least-sig -> most-sig] s0 x b1^0 + s1 x b1^1+ s2 x
  // b1^2+ s3 x b1^3+..... sn x bn
  // convert decimal to base 2 and return it's
  // srting. dont' use any string parsing.libs.
  auto is_negative = num_str.front() == '-' ? true : false;

  long long num = std::accumulate(
      (std::begin(num_str) + is_negative), std::end(num_str), 0ull,
      [b1](int val, char dig) {
        return val * b1 + (isdigit(dig) ? dig - '0' : (10 + (dig - 'A')));
      });

  return (is_negative ? "-" : "") + (num == 0 ? "0" : ConvertBaseStr(num, b2));
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"num_str", "b1", "b2"};
  return GenericTestMain(args, "convert_base.cc", "convert_base.tsv",
                         &ConvertBase, DefaultComparator{}, param_names);
}
