#include <iterator>
#include <string>
#include <vector>

#include "test_framework/generic_test.h"
#include "test_framework/timed_executor.h"
using std::string;
using std::vector;

int ReplaceAndRemove(int size, char s[]) {
  // to solve in O(n) time complexity and O(1) space complexity.
  // use 2 passes on this array.

  auto start = 0, writepos = 0, ac = 0, i = 0;
  for (i = 0; i < size; i++) {
    if (s[i] != 'b')
      s[writepos++] = s[i];
    if (s[i] == 'a')
      ac++;
  }

  // i = size - 1; // not size since a few b's would have been deleted by now.
  i = writepos - 1;
  writepos += ac - 1;
  auto result = writepos + 1;
  while (i >= 0) {
    // pre-decrement is essential.
    if (s[i] == 'a') {
      s[writepos--] = 'd';
      s[writepos--] = 'd';
    } else {
      s[writepos--] = s[i];
    }
    --i;
  }
  return result;
}

vector<string> ReplaceAndRemoveWrapper(TimedExecutor &executor, int size,
                                       const vector<string> &s) {
  std::vector<char> s_copy(s.size(), '\0');
  for (int i = 0; i < s.size(); ++i) {
    if (!s[i].empty()) {
      s_copy[i] = s[i][0];
    }
  }

  int res_size =
      executor.Run([&] { return ReplaceAndRemove(size, s_copy.data()); });

  vector<string> result;
  for (int i = 0; i < res_size; ++i) {
    result.emplace_back(string(1, s_copy[i]));
  }
  return result;
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"executor", "size", "s"};
  return GenericTestMain(args, "replace_and_remove.cc",
                         "replace_and_remove.tsv", &ReplaceAndRemoveWrapper,
                         DefaultComparator{}, param_names);
}
