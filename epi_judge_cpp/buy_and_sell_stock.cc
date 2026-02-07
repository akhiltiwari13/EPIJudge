#include <algorithm>
#include <vector>

#include "test_framework/generic_test.h"
using std::vector;
double BuyAndSellStockOnce(const vector<double> &prices) {
  // key insight: max profit = max( (curr - min_till_now), prev_max)
  double max_profit_till_now = std::numeric_limits<double>::lowest();
  double min_price_till_now = std::numeric_limits<double>::infinity();

  for (int i = 0; i < prices.size(); ++i) {
    min_price_till_now = std::min(prices[i], min_price_till_now);
    max_profit_till_now =
        std::max(max_profit_till_now, (prices[i] - min_price_till_now));
  }

  return max_profit_till_now;
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"prices"};
  return GenericTestMain(args, "buy_and_sell_stock.cc",
                         "buy_and_sell_stock.tsv", &BuyAndSellStockOnce,
                         DefaultComparator{}, param_names);
}
