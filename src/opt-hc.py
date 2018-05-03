#!/usr/bin/env python3

import sys
import random
import networkx as nx
from argparse import ArgumentParser
from graph_config import GraphConfig
from edge_swap import random_swap

def optimize(G, conf, steps, log=None):
    def write_log_row(t, diam, aspl, ofile):
        print('{},{},{}'.format(t, diam, aspl), file=ofile)

    min_diam = nx.diameter(G)
    min_aspl = nx.average_shortest_path_length(G)
    if log:
        log_fp = open(log, 'w')
        print('t,diam,aspl', file=log_fp)
        write_log_row(0, min_diam, min_aspl, log_fp)
    for t in range(1, steps+1):
        H = G.copy()
        random_swap(H, conf)
        diam = nx.diameter(H)
        aspl = nx.average_shortest_path_length(H)
        if diam < min_diam or\
           diam == min_diam and aspl < min_aspl:
            G, min_diam, min_aspl = H, diam, aspl
        if log:
            write_log_row(t, min_diam, min_aspl, log_fp)

    if log:
        log_fp.close()
    return G

def get_options():
    parser = ArgumentParser()
    parser.add_argument('--size', type=int, required=True, nargs='+')
    parser.add_argument('-r', type=int, required=True, default=-1)
    parser.add_argument('-i', '--input', type=str, required=True)
    parser.add_argument('-o', '--output', type=str)
    parser.add_argument('--seed', type=int)
    parser.add_argument('--step', type=int, default=1000)
    parser.add_argument('--log', type=str)
    opts = parser.parse_args()
    return opts

def main():
    opts = get_options()
    random.seed(opts.seed)
    G = nx.read_edgelist(opts.input, nodetype=int)
    G = optimize(G, GraphConfig(opts.size, opts.r), opts.step, opts.log)

    os = open(opts.output, 'w') if opts.output else sys.stdout
    for e in nx.generate_edgelist(G, data=False):
        print(e, file=os)
    if opts.output:
        os.close()

if __name__ == '__main__':
    main()
