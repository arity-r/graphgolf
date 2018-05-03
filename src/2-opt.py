#!/usr/bin/env python3

import sys
import random
import networkx as nx
from argparse import ArgumentParser
from graph_config import GraphConfig
from edge_swap import random_swap

def optimize(G, conf, steps):
    min_diam = nx.diameter(G)
    min_aspl = nx.average_shortest_path_length(G)
    while steps > 0:
        steps -= 1
        H = G.copy()
        random_swap(H, conf)
        diam = nx.diameter(H)
        aspl = nx.average_shortest_path_length(H)
        if diam < min_diam or\
           diam == min_diam and aspl < min_aspl:
            G, min_diam, min_aspl = H, diam, aspl
    return G

def get_options():
    parser = ArgumentParser()
    parser.add_argument('--size', type=int, required=True, nargs='+')
    parser.add_argument('-r', type=int, required=True, default=-1)
    parser.add_argument('-i', '--input', type=str, required=True)
    parser.add_argument('-o', '--output', type=str)
    parser.add_argument('--seed', type=int)
    parser.add_argument('--step', type=int, default=1000)
    opts = parser.parse_args()
    return opts

def main():
    opts = get_options()
    random.seed(opts.seed)
    G = nx.read_edgelist(opts.input, nodetype=int)
    G = optimize(G, GraphConfig(opts.size, opts.r), opts.step)

    os = open(opts.output, 'w') if opts.output else sys.stdout
    for e in nx.generate_edgelist(G, data=False):
        print(e, file=os)
    if opts.output:
        os.close()

if __name__ == '__main__':
    main()
