from __future__ import print_function, division
from math import sqrt
from itertools import chain, product
from functools import reduce
import operator

class GraphConfig(object):
    def __init__(self, dim, r=-1):
        assert(type(dim) in (int, tuple, list))
        if type(dim) is int: dim = [dim]
        self._dim = dim
        self._const = list(map(
            lambda i: reduce(operator.mul, dim[i+1:], 1),
            range(len(dim))
        ))
        self._n = self._const[0] * dim[0]
        self._r = r

    def get_r(self):
        return self._r

    def set_r(self, r):
        self._r = r

    def get_dim(self):
        return self._dim

    def get_n(self):
        return self._n

    def vid(self, dim):
        assert type(dim) in (int, tuple, list)
        if type(dim) is int: dim = (dim,)
        if len(dim) == len(self._dim) and\
           all(map(lambda x, y: x >= 0 and x < y, dim, self._dim)):
            return sum(map(
                lambda x, y: x * y, dim, self._const
            ))
        else:
            return -1

    def loc(self, vid):
        return tuple(map(
            lambda c, d: vid // c % d,
            self._const, self._dim
        ))

    def dist(self, u, v):
        loc_u, loc_v = self.loc(u), self.loc(v)
        return sum(map(lambda a, b: abs(a-b), loc_u, loc_v))

    def are_neighbors(self, u, v):
        return u != v and self.dist(u, v) <= self._r if self._r >= 0 else\
                    True

    def neighbors(self, u):
        if(self._r < 0):
            return chain(range(u), range(u+1, self._n))
        loc = self.loc(u)
        return filter(
            lambda v: v >= 0,
            map(
                lambda d: self.vid(tuple(map(sum, zip(loc, d)))),
                filter(
                    lambda d: sum(map(abs, d)) <= self._r and\
                    any(map(lambda e: e != 0, d)),
                    product(*map(
                        lambda d: range(-self._r, self._r+1),
                        self._dim)
                    )
                )
            )
        )

def main():
    """
      0  1  2  3  4  5  6  7
      8  9 10 11 12 13 14 15
     16 17 18 19 20 21 22 23
     24 25 26 27 28 29 30 31
    """
    conf1 = GraphConfig((4, 8), 2)
    print(conf1.vid((1, 2)))
    print(conf1.loc(19))
    print(conf1.dist(10, 20))
    print(conf1.are_neighbors(11, 17))
    print(tuple(conf1.neighbors(11)))

if __name__ == '__main__':
    main()
