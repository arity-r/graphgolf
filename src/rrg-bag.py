#!/usr/bin/env python3

import sys
import random
import networkx as nx
from argparse import ArgumentParser
from graph_config import GraphConfig

def rrg_bag(d, dim, r=-1, seed=None):
    conf = GraphConfig(dim, r)
    assert (conf.get_n() * d) % 2 == 0
    assert d >= 0 and conf.get_n() > d
    if seed is not None:
        random.seed(seed)

    # modified from networkx.random_regular_graph
    def _suitable(edges, stubs):
        if not stubs:
            return True
        for s in stubs:
            for t in stubs:
                if s == t: break
                if s > t: s, t = t, s
                if (s, t) not in edges and conf.are_neighbors(s, t):
                    return True
        return False

    def _try_creation(d, n):
        edges = set()
        stubs = list(range(n)) * d
        while stubs:
            random.shuffle(stubs)
            for i, s in reversed(list(enumerate(stubs[:-1]))):
                ts = set(stubs[i+1:])
                ts = ts.intersection(set(conf.neighbors(s)))
                ts = [t for t in ts if
                      not ((s, t) in edges or (t, s) in edges)]
                if len(ts) == 0: continue
                t = random.choice(tuple(ts))
                if s > t: s, t = t, s
                edges.add((s, t))
                stubs.reverse()
                stubs.remove(s)
                stubs.remove(t)
                stubs.reverse()
            if not _suitable(edges, stubs):
                return None
        return edges

    G = None
    while not G or not nx.is_connected(G):
        edges = _try_creation(d, conf.get_n())
        while edges is None:
            edges = _try_creation(d, conf.get_n())
        G = nx.Graph()
        G.add_edges_from(edges)
    return G

def get_options():
    parser = ArgumentParser()
    parser.add_argument('-n', '--size', type=int, required=True, nargs='+')
    parser.add_argument('-d', type=int, required=True)
    parser.add_argument('-r', type=int, default=-1)
    parser.add_argument('-o', '--output', type=str)
    parser.add_argument('--seed', type=int)
    return parser.parse_args()

def main():
    opts = get_options()
    G = rrg_bag(opts.d, opts.size, opts.r, opts.seed)
    os = open(opts.output, 'w') if opts.output else sys.stdout
    for e in nx.generate_edgelist(G, data=False):
        print(e, file=os)
    if opts.output:
        os.close()

if __name__ == '__main__':
    main()
