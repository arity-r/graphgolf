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
from edge_swap import random_swap

def get_options():
    parser = ArgumentParser()
    parser.add_argument('--osize', required=True)
    parser.add_argument('--ofile', required=True)
    parser.add_argument('--isize', nargs='*', required=True)
    parser.add_argument('--ifiles', nargs='*', required=True)
    parser.add_argument('-d', type=int)
    parser.add_argument('-r', type=int)
    return parser.parse_args()

def base_graph(conf, d, osize, isize, ifiles):
    pos = [(0, 0)]
    curr_row_height = 0
    # validate dimension
    for i, s in enumerate(isize):
        if curr_row_height > 0 and curr_row_height != s[0]:
            raise RuntimeError('Invalid height at input#{}', i)
        prev_pos = pos[-1]
        if prev_pos[1]+s[1] < osize[1]: # current row
            curr_row_height = s[0]
            next_pos = (prev_pos[0], prev_pos[1]+s[1])
        elif prev_pos[1]+s[1] == osize[1]: # new row
            curr_row_height = 0
            next_pos = (prev_pos[0]+s[0], 0)
        else:
            raise RuntimeError('Invalid width at input#{}', i)
        pos.append(next_pos)
    if pos[-1][0] != osize[0] or pos[-1][1] != 0:
        raise RuntimeError('Incorrect grid size (in:{} out:{})', pos[-1], osize)

    G = nx.Graph()
    for f, s, p in zip(ifiles, isize, pos):
        H = nx.read_edgelist(f, nodetype=int, data=False)
        miniconf = GraphConfig(s)
        for u, v in H.edges():
            u = conf.vid(tuple(map(sum, zip(p, miniconf.loc(u)))))
            v = conf.vid(tuple(map(sum, zip(p, miniconf.loc(v)))))
            G.add_edge(u, v)

    if d is None:
        d = max(dict(G.degree).values())
    if conf.get_r() is None:
        conf.set_r(max(conf.dist(e[0], e[1]) for e in G.edges()))

    degseq = list(map(
        lambda v: min(d, len(list(conf.neighbors(v)))) - G.degree[v],
        G.nodes()
    ))
    return G, degseq

def make_sat_vars(conf, G, degseq):
    E = dict()
    for u in range(conf.get_n()):
        if degseq[u] == 0:
            continue
        E[u] = dict()
        for v in conf.neighbors(u):
            if degseq[v] == 0 or G.has_edge(u, v):
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

def main():
    opts = get_options()
    opts.osize = tuple(map(int, opts.osize.split('x')))
    opts.isize = list(map(lambda x: tuple(map(int, x.split('x'))), opts.isize))
    conf = GraphConfig(opts.osize, opts.r)
    G, degseq = base_graph(conf, opts.d, opts.osize, opts.isize, opts.ifiles)

    if any(map(lambda x: x > 0, degseq)):
        E = make_sat_vars(conf, G, degseq)
        s = make_sat_solver(E, degseq)
        if s.check() == unsat:
            raise RuntimeError('No such graph')
        m = s.model()
        for u, v in product(G.nodes(), G.nodes()):
            if u in E and v in E[u] and m.eval(E[u][v]):
                G.add_edge(u, v)

    while not nx.is_connected(G):
        random_swap(G, conf)
    nx.write_edgelist(G, opts.ofile, data=False)

if __name__ == '__main__':
    main()
