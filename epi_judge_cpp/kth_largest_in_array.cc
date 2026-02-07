#include <random>
#include <utility>
#include <vector>

#include "test_framework/generic_test.h"
using std::vector;

// The numbering starts from one, i.e., if A = [3, 1, -1, 2] then
// FindKthLargest(1, A) returns 3, FindKthLargest(2, A) returns 2,
// FindKthLargest(3, A) returns 1, and FindKthLargest(4, A) returns -1.

// helper function to partition the data.
int partitionV(vector<int> &vec, int left, int right, int ppos) {

  // logic to quickly partition this array and return the final position of the
  // pivot.
  int lesser = left, same = left, greater = right;
  auto pivot = vec[ppos];
  std::swap(vec[ppos], vec[right]); // moving pivot to the end.
  int store_idx = left;

  int i = left;
  // while (lesser <= greater) {
  while (i <= greater) {
    if (vec[i] < pivot) {
      std::swap(vec[i++], vec[lesser++]);
      // } else if (vec[i] > pivot)
    }
    // else
    //   std::swap(vec[i], vec[greater--]);
  }

  std::swap(vec[store_idx], vec[right]);
  // TODO:
  // check what should be returned ? ? return same;
  return store_idx;
}

// this is found using randomized comparison algo...
// select randonm pivots.
// partition for the pivot.
int FindKthLargest(int k, vector<int> *A_ptr) {
  std::default_random_engine rande((std::random_device())());
  auto vec = *A_ptr;
  // initialize range for binary search ops
  int left = 0, right = vec.size() - 1;
  // auto expected_idx = vec.size() - k - 1;
  auto expected_idx = vec.size() - k; // this is wrong....
  // binary serach on the partitioned array for the random pivot.
  // TODO perform binary search for this subarray.
  while (left <= right) {
    // conditions for the positioning of the poivot index.
    // gen a randonm pivot
    auto pivot_idx = std::uniform_int_distribution<int>{left, right}(rande);
    int corr_idx = partitionV(vec, left, right, pivot_idx);

    if (corr_idx == expected_idx) {
      return vec[corr_idx];
    } else if (corr_idx < expected_idx) {
      left = corr_idx + 1;
    } else {
      right = corr_idx - 1;
    }
  }

  return vec[expected_idx]; // return result;
}

int FindKthLargestWrapper(int k, vector<int> &A) {
  return FindKthLargest(k, &A);
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"k", "A"};
  return GenericTestMain(args, "kth_largest_in_array.cc",
                         "kth_largest_in_array.tsv", &FindKthLargestWrapper,
                         DefaultComparator{}, param_names);
}
