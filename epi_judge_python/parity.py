from test_framework import generic_test


def paritybitbruteforce(x: int) -> int:
    c=0
    while x:
        # pass
        c ^= x & 1
        x >>= 1
    return c


def paritybydroppinglowestsetbit(x: int) -> int:
    c=0
    while x :
        c^=1
        x &=  x-1
    return c

def parity64bit(x: int) -> int:
    x ^= x>>32
    x ^= x>>16
    x ^= x>>8
    x ^= x>>4
    x ^= x>>2
    x ^= x>>1
    return x

def parity16bit(x: int) -> int:
    while x:
        x ^= x>>8
        x ^= x>>4
        x ^= x>>2
        x ^= x>>1
    return x

def create_16_bit_parity_lookup(d:dict)->void: 
    for idx in range(0,2**16-1):
        d[idx] = parity16bit(idx) 


if __name__ == '__main__':
    # exit(generic_test.generic_test_main('parity.py', 'parity.tsv', paritybitbruteforce))
    # exit(generic_test.generic_test_main('parity.py', 'parity.tsv', paritybydroppinglowestsetbit))
    exit(generic_test.generic_test_main('parity.py', 'parity.tsv', parity64bit))
    # exit(generic_test.generic_test_main('parity.py', 'parity.tsv', parity))
    # exit(generic_test.generic_test_main('parity.py', 'parity.tsv', parity))
    # exit(generic_test.generic_test_main('parity.py', 'parity.tsv', parity))
