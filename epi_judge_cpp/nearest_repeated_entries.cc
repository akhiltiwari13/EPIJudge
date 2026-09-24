#include <string>
#include <vector>

#include "test_framework/generic_test.h"
using std::string;
using std::vector;

int FindNearestRepetition(const vector<string>& paragraph) {
  // TODO - you fill in here.
  std::unordered_map<string, int> latest_index;
  int result = std::numeric_limits<int>::max();
  for(int i=0; i<paragraph.size(); i++) {
    auto latest_equal = latest_index.find(paragraph[i]);
    if(latest_equal != latest_index.end()) {
      result= std::min(result, i-latest_index[paragraph[i]]);
    }
    latest_index[paragraph[i]]=i;
  }
  return result == std::numeric_limits<int>::max() ? -1 : result;
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"paragraph"};
  return GenericTestMain(args, "nearest_repeated_entries.cc",
                         "nearest_repeated_entries.tsv", &FindNearestRepetition,
                         DefaultComparator{}, param_names);
}
