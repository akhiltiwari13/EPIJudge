from typing import List

from test_framework import generic_test


def buy_and_sell_stock_once(prices: List[float]) -> float:
    # for each element, the maximum profit that can be made is "price_today" - "min price so far"
    #     max ("current max","elements max val")
    max_profit, min_so_far = 0,  float("inf")
    
    for price in prices:
        min_so_far = min(min_so_far,price)
        max_profit = max(max_profit, price-min_so_far )
    return max_profit 


if __name__ == '__main__':
    exit(
        generic_test.generic_test_main('buy_and_sell_stock.py',
                                       'buy_and_sell_stock.tsv',
                                       buy_and_sell_stock_once))
