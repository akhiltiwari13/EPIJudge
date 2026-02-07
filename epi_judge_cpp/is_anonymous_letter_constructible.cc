#include <string>
#include <unordered_map>

#include "test_framework/generic_test.h"
using std::string;

bool IsLetterConstructibleFromMagazine(const string &letter_text,
                                       const string &magazine_text) {
  std::unordered_map<char, int> cache;

  for (auto c : letter_text) {
    cache[c]++;
  }

  for (auto c : magazine_text) {
    if (cache.find(c) != cache.end())
      cache[c]--;
  }
  // return !cache.empty() ? false : true;
  // check if all of them are 0...
  for (auto el : cache) {
    if (el.second > 0) {
      // if (el.second) { this fails cause we can have negative numbers as well.
      return false;
    }
  }
  return true;
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"letter_text", "magazine_text"};
  return GenericTestMain(args, "is_anonymous_letter_constructible.cc",
                         "is_anonymous_letter_constructible.tsv",
                         &IsLetterConstructibleFromMagazine,
                         DefaultComparator{}, param_names);
}
