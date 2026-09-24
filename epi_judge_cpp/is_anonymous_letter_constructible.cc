#include <string>

#include "test_framework/generic_test.h"
using std::string;

bool IsLetterConstructibleFromMagazine(const string& letter_text,
                                       const string& magazine_text) {
  // TODO - you fill in here.
  std::unordered_map<char, int> char_freq;
  for(char c: letter_text) {
    ++char_freq[c];
  }
  for(char c: magazine_text) {
    auto itr = char_freq.find(c);
    if(itr != char_freq.end()) {
      --itr->second;
      if(itr->second==0) {
        char_freq.erase(itr);
      }
    }
  }
  return char_freq.empty();
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"letter_text", "magazine_text"};
  return GenericTestMain(args, "is_anonymous_letter_constructible.cc",
                         "is_anonymous_letter_constructible.tsv",
                         &IsLetterConstructibleFromMagazine,
                         DefaultComparator{}, param_names);
}
