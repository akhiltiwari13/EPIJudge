#include <algorithm>
#include <numeric>
#include <string>

#include "test_framework/generic_test.h"
#include "test_framework/test_failure.h"
using std::string;

string IntToString(int x) {
  // TODO - you fill in here.
  if (x == 0)
    return "0"; // handle corner case.
  std::string result{};
  long long num{x}; // simple fix to manage overflow...
  while (num) {
    auto c = '0' + (std::abs(num) % 10ll);
    result.push_back(c);
    num /= 10;
  }
  if (x < 0) {
    result.push_back('-');
  }

  std::reverse(result.begin(), result.end());
  return result;
}
int StringToInt(const string &s) {
  // TODO - you fill in here.
  return (s[0] == '-' ? -1 : 1) *
         std::accumulate(
             ((s[0] == '-') || (s[0] == '+') ? s.begin() + 1 : s.begin()),
             s.end(), 0, [](int running_sum, char c) {
               return running_sum * 10 + (c - '0');
             });
}

void Wrapper(int x, const string &s) {
  if (stoi(IntToString(x)) != x) {
    throw TestFailure("Int to string conversion failed");
  }

  if (StringToInt(s) != x) {
    throw TestFailure("String to int conversion failed");
  }
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"x", "s"};
  return GenericTestMain(args, "string_integer_interconversion.cc",
                         "string_integer_interconversion.tsv", &Wrapper,
                         DefaultComparator{}, param_names);
}
