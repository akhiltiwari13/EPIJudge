#include "test_framework/generic_test.h"
// why does this bruteforce fail?
double PowerBruteforce(double x, int y) {
  // TODO - you fill in here.
  if (y < 0) {
    x = 1 / x;
    y *= -1;
  }
  auto result = 1.0f;
  for (int i = 0; i < y; ++i) {
    result *= x;
  }
  return result;
}

double Power(double x, int y) {
  // TODO - you fill in here.
  if (y == 0)
    return 1.0f;
  if (y == 1)
    return x;
  if (y < 0) {
    x = 1 / x;
    y *= -1;
  }

  if (y % 2) {
    return x * Power(x * x, y >> 1);
  } else
    return Power(x * x, y >> 1);
}

double PowerIterative(double x, int y) {
  // TODO - you fill in here.
  if (y < 0) {
    x = 1 / x;
    y *= -1;
  }
  auto result = 1.0f;
  // result *= y & 1 ? x : 1;
  while (y) {
    if (y & 1) { // checking if y is odd. this also gaurantees that x gets
                 // stored in the result when it becomes 1.
      result *= x;
    }
    x *= x;
    y >>= 1;
  }

  return result;
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"x", "y"};
  return GenericTestMain(args, "power_x_y.cc", "power_x_y.tsv", &PowerIterative,
                         DefaultComparator{}, param_names);
}
