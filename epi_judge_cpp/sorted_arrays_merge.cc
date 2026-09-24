#include <vector>

#include "test_framework/generic_test.h"
using std::vector;

struct itr {
  vector<int>:: const_iterator current;
  vector<int>::const_iterator end;
  bool operator> (const itr& that) const {
    return *current >*that.current;
  }
};
vector<int> MergeSortedArrays(const vector<vector<int>>& sorted_arrays) {

  std::priority_queue<itr, vector<itr>, std::greater<>> min_heap;

  for(const vector<int>& sorted_array: sorted_arrays){
    // if(!empty(sorted_array)) {
      min_heap.emplace(itr{begin(sorted_array), cend(sorted_array)});
    // }
  }
  vector<int> result;
  while(!empty(min_heap)) {
    itr smallest_array = min_heap.top();
    min_heap.pop();
    result.emplace_back(*smallest_array.current);
    if(next(smallest_array.current)!= smallest_array.end){
      min_heap.emplace(itr{next(smallest_array.current), smallest_array.end});
    }
  }
  return result;
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"sorted_arrays"};
  return GenericTestMain(args, "sorted_arrays_merge.cc",
                         "sorted_arrays_merge.tsv", &MergeSortedArrays,
                         DefaultComparator{}, param_names);
}
