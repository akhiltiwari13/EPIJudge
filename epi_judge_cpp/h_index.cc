#include <vector>

#include "test_framework/generic_test.h"
using std::vector;
int HIndex(vector<int> citations) {
  // TODO - you fill in here.
    int h_index=0;
    //sort the array in ascending order.
    sort(citations.begin(), citations.end());
    for(int i=citations.size()-1; i>=0; --i){
        if(citations[i] > h_index ){
            ++h_index;
        }
        else{
            break;
        }
    }
  return h_index;
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"citations"};
  return GenericTestMain(args, "h_index.cc", "h_index.tsv", &HIndex,
                         DefaultComparator{}, param_names);
}
