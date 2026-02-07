#include <iterator>
#include <list>
#include <unordered_map>
#include <utility>
#include <vector>

#include "test_framework/generic_test.h"
#include "test_framework/serialization_traits.h"
#include "test_framework/test_failure.h"

class LruCache {
public:
  LruCache(size_t capacity) : capacity_(capacity) {}

  int Lookup(int isbn) {
    if (cache_.find(isbn) != end(cache_)) { // present in cache
      auto eleitr = cache_.find(isbn)->second.second;
      auto eleprice = cache_.find(isbn)->second.first;
      // move to front in the recency table.
      recency_list_.erase(eleitr);
      recency_list_.push_front(isbn);
      cache_[isbn] = {eleprice, recency_list_.begin()};

      return cache_[isbn].first;
    }
    return -1;
  }

  void Insert(int isbn, int price) {
    int eleprice{0};
    auto it = cache_.find(isbn);
    if (it != end(cache_)) {
      // for existing only update the recency info not the value/price.
      auto valpair = it->second;
      auto eleitr = valpair.second;
      auto oldprice = valpair.first;
      recency_list_.erase(eleitr);
      recency_list_.push_front(isbn);
      it->second.second = recency_list_.begin();
      return;
    }
    if (cache_.size() == capacity_) {
      // auto itr = std::prev(recency_list_.end());
      auto lru_isbn = recency_list_.back();
      recency_list_.pop_back();
      cache_.erase(lru_isbn);
    }
    recency_list_.push_front(isbn);
    cache_[isbn] = {price, recency_list_.begin()};
  }

  bool Erase(int isbn) {
    if (cache_.find(isbn) != end(cache_)) {
      auto itr = cache_.find(isbn)->second.second;
      recency_list_.erase(itr);
      cache_.erase(isbn);
      return true;
    }
    return false;
  }

private:
  std::unordered_map<int, std::pair<int, std::list<int>::iterator>>
      cache_; // isbn | <price|recency_info which can be represented as an
              // iterator to a list with the ordering.>
  int capacity_;
  std::list<int> recency_list_;
};

struct Op {
  std::string code;
  int arg1;
  int arg2;
};

namespace test_framework {
template <>
struct SerializationTrait<Op> : UserSerTrait<Op, std::string, int, int> {};
} // namespace test_framework

void LruCacheTester(const std::vector<Op> &commands) {
  if (commands.empty() || commands[0].code != "LruCache") {
    throw std::runtime_error("Expected LruCache as first command");
  }
  LruCache cache(commands[0].arg1);

  for (int i = 1; i < commands.size(); i++) {
    auto &cmd = commands[i];
    if (cmd.code == "lookup") {
      int result = cache.Lookup(cmd.arg1);
      if (result != cmd.arg2) {
        throw TestFailure("Lookup: expected " + std::to_string(cmd.arg2) +
                          ", got " + std::to_string(result));
      }
    } else if (cmd.code == "insert") {
      cache.Insert(cmd.arg1, cmd.arg2);
    } else if (cmd.code == "erase") {
      bool result = cache.Erase(cmd.arg1);
      if (result != cmd.arg2) {
        throw TestFailure("Erase: expected " + std::to_string(cmd.arg2) +
                          ", got " + std::to_string(result));
      }
    } else {
      throw std::runtime_error("Unexpected command " + cmd.code);
    }
  }
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"commands"};
  return GenericTestMain(args, "lru_cache.cc", "lru_cache.tsv", &LruCacheTester,
                         DefaultComparator{}, param_names);
}
