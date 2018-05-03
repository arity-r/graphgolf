#!/usr/bin/env python3
from __future__ import division, print_function
import os.path
from argparse import ArgumentParser

import networkx as nx
from networkx.algorithms import isomorphism as iso
from z3 import *

from graph_config import GraphConfig

def get_options():
    parser = ArgumentParser()
    parser.add_argument('--size', type=int, required=True, nargs='+')
    parser.add_argument('-d', type=int, required=True)
    parser.add_argument('-r', type=int, default=-1)
    parser.add_argument('-p', '--prefix', type=str, required=True)
    parser.add_argument('--ngraphs', type=int, default=1)
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
    E, Erev = dict(), dict()
    for u in range(conf.get_n()):
        if degseq[u] == 0:
            continue
        nn = conf.neighbors(u)
        for v in nn:
            if degseq[v] == 0:
                continue
            if u < v:
                E[(u, v)] = Bool('E_{}_{}'.format(u, v))
                Erev[E[(u, v)]] = (u, v)
            else:
                E[(u, v)] = E[(v, u)]

    return E, Erev

def make_sat_solver(E, degseq):
    s = Solver()
    fs = [0]*len(degseq)
    for (u, v), val in E.items():
        fs[u] = fs[u] + If(val, 1, 0)
    for u in range(len(fs)):
        fs[u] = fs[u] == degseq[u]
    s.add(fs)
    return s

def main():
    opts = get_options()

    conf = GraphConfig(opts.size, opts.r)
    Gbase, degseq = base_graph(conf, opts.d)
    E, Erev = make_sat_vars(conf, Gbase, degseq)
    s = make_sat_solver(E, degseq)

    dirname = os.path.dirname(opts.prefix)
    if len(dirname) > 0 and not os.path.isdir(dirname):
        os.mkdir(dirname)

    mcount = 1
    fmtstr = '{}-{:0' + str(len(str(opts.ngraphs))) + 'd}.elist'
    while s.check() == sat and (opts.ngraphs <= 0 or mcount <= opts.ngraphs):
        m = s.model()

        G = Gbase.copy()
        G.add_edges_from((u, v) for u, v in Erev.values() if m.eval(E[(u, v)]))
        filename = fmtstr.format(opts.prefix, mcount)
        nx.write_edgelist(G, filename, data=False)

        # add new solution
        # s = add_new_solution(s, m, E)
        for i in iso.GraphMatcher(G, G.copy()).isomorphisms_iter():
            if any((i[u], i[v]) not in E for u, v in Erev.values()):
                continue
            s.add(Not(And([
                l if m.eval(E[(i[u], i[v])]) else Not(l)
                for l, (u, v) in Erev.items()
            ])))
        mcount += 1

if __name__ == '__main__':
    main()
