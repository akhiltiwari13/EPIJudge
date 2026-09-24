#include <vector>

#include "test_framework/generic_test.h"
using std::vector;

int SearchFirstOfK(const vector<int>& A, int k) {
  // TODO - you fill in here.
  int l =0, r= A.size()-1, result = -1;
  while(l<=r) {
    int mid = l+ ((r-l)/2);
    if(A[mid]> k){
      r = mid-1;
    }
    else if( (A[mid]==k)) {
      result = mid;
      r= mid-1;
    }
    else {
      l =mid+1;
    }
  }


  
  return result;
}


int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"A", "k"};
  return GenericTestMain(args, "search_first_key.cc", "search_first_key.tsv",
                         &SearchFirstOfK, DefaultComparator{}, param_names);
}
