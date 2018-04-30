#!/usr/bin/env python3
from __future__ import division, print_function
import os.path
import operator as op
from functools import reduce
from itertools import product
from argparse import ArgumentParser

import networkx as nx
from z3 import *

from graph_config import GraphConfig

def get_options():
    parser = ArgumentParser()
    parser.add_argument('-n', '--size', type=int, required=True, nargs='+')
    parser.add_argument('-d', type=int, required=True)
    parser.add_argument('-r', type=int, default=-1)
    parser.add_argument('-p', '--prefix', type=str, required=True)
    parser.add_argument('--n-graphs', type=int, default=1)
    return parser.parse_args()

def base_graph(conf, d):
    G = nx.Graph()
    G.add_nodes_from(range(conf.get_n()))
    for u in range(conf.get_n()):
        nn = list(conf.neighbors(u))
        if len(nn) <= d:
            G.add_edges_from((u, v) for v in nn)

    degseq = []
    for u in range(conf.get_n()):
        nn = list(conf.neighbors(u))
        if G.degree(u) == len(nn):
            degseq.append(0)
        else:
            degseq.append(d - G.degree(u))

    return G, degseq

def make_sat_vars(conf, G, degseq):
    E = dict()
    for u in range(conf.get_n()):
        if degseq[u] == 0:
            continue
        E[u] = dict()
        nn = conf.neighbors(u)
        for v in nn:
            if degseq[v] == 0:
                continue
            if u < v:
                E[u][v] = Bool('E_{}_{}'.format(u, v))
            else:
                E[u][v] = E[v][u]

    return E

def make_sat_solver(E, degseq):
    s = Solver()
    s.add([
        reduce(op.add, map(lambda e: If(e, 1, 0), v.values())) == degseq[k]
        for k, v in E.items()
    ])
    return s

def add_new_solution(s, m, E):
    s.add(Not(And(list(map(
        lambda e: e if m.eval(e) else Not(e),
        reduce(op.or_, map(lambda v: set(v.values()), E.values()))
    )))))
    return s

def main():
    opts = get_options()

    conf = GraphConfig(opts.size, opts.r)
    G, degseq = base_graph(conf, opts.d)
    E = make_sat_vars(conf, G, degseq)
    s = make_sat_solver(E, degseq)

    dirname = os.path.dirname(opts.prefix)
    if len(dirname) > 0 and not os.path.isdir(dirname):
        os.mkdir(dirname)

    mcount = 1
    fmtstr = '{}-{:0' + str(len(str(opts.n_graphs-1))) + 'd}.elist'
    while s.check() == sat and mcount < opts.n_graphs:
        m = s.model()
        H = G.copy()
        for u, v in product(G.nodes(), G.nodes()):
            if u in E and v in E[u] and m.eval(E[u][v]):
                H.add_edge(u, v)

        filename = fmtstr.format(opts.prefix, mcount)
        nx.write_edgelist(H, filename, data=False)
        """
        with open(os.path.join(dirname, filename), 'w') as fp:
            fp.write('\n'.join(map(
                lambda e: ' '.join(map(str, e)),
                H.edges_iter())
            ))
        """

        s = add_new_solution(s, m, E)
        mcount += 1

if __name__ == '__main__':
    main()
