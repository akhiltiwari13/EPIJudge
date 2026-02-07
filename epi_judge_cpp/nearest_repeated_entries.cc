#include <algorithm>
#include <limits>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "test_framework/generic_test.h"
using std::string;
using std::vector;

int FindNearestRepetition(const vector<string> &paragraph) {
  std::unordered_map<string, int> cache{};
  int result = std::numeric_limits<int>::max();
  for (int i = 0; i < paragraph.size(); ++i) {
    if (cache.find(paragraph[i]) != cache.end()) {
      result = std::min(result, abs(i - cache[paragraph[i]]));
    }
    cache[paragraph[i]] = i;
  }
  return (result == std::numeric_limits<int>::max()) ? -1 : result;
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"paragraph"};
  return GenericTestMain(args, "nearest_repeated_entries.cc",
                         "nearest_repeated_entries.tsv", &FindNearestRepetition,
                         DefaultComparator{}, param_names);
}
