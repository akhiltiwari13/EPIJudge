#include <vector>

#include "test_framework/generic_test.h"
using std::vector;
#include <random>  

// The numbering starts from one, i.e., if A = [3, 1, -1, 2] then
// FindKthLargest(1, A) returns 3, FindKthLargest(2, A) returns 2,
// FindKthLargest(3, A) returns 1, and FindKthLargest(4, A) returns -1.

int PartitionAroundtPivot(int l, int r, int pt_idx, vector <int>* A_ptr) {
  vector<int>& A = *A_ptr;
  int pt_value = A[pt_idx];
  int new_pt_idx= l;
  std::swap(A[r], A[pt_idx]);
  for(int i=l; i<r; i++) {
    if(A[i] >pt_value) {
      std::swap(A[i], A[new_pt_idx++]);
    }
  }
  std::swap(A[r], A[new_pt_idx]);
  return new_pt_idx;


}
int FindKthLargest(int k, vector<int>* A_ptr) {
  // TODO - you fill in here.
  vector<int>& A = *A_ptr;
  int l=0, r= A.size()-1;
  std::default_random_engine gen((std::random_device())());

  while(l<=r) {

    int pt_idx = std::uniform_int_distribution<int>{l,r}(gen);
    int new_pt_idx = PartitionAroundtPivot(l, r, pt_idx, &A);
    if(new_pt_idx ==k-1) {
      return A[new_pt_idx];
    }
    else if (new_pt_idx>k-1) {
      r=new_pt_idx-1;
    }
    else {
      l =new_pt_idx+1;
    }

  }
  return 0;
}
int FindKthLargestWrapper(int k, vector<int>& A) {
  return FindKthLargest(k, &A);
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"k", "A"};
  return GenericTestMain(args, "kth_largest_in_array.cc",
                         "kth_largest_in_array.tsv", &FindKthLargestWrapper,
                         DefaultComparator{}, param_names);
}
