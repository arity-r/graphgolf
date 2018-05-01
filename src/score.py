#!/usr/bin/env python3
from __future__ import print_function, division
import os
import sys
from argparse import ArgumentParser
from graph_config import GraphConfig
import networkx as nx

def get_options():
    parser = ArgumentParser()
    parser.add_argument('-n', '--size', type=int, nargs='*')
    parser.add_argument('-o', '--output', type=str)
    parser.add_argument('ifiles', nargs='*')
    return parser.parse_args()

def print_row(ifile, ofile, size):
    G = nx.read_edgelist(ifile, nodetype=int, data=False)
    d = max(dict(G.degree).values())
    if size is None: # No dimension -> general category
        r = -1
    else:
        conf = GraphConfig(size)
        r = max(conf.dist(e[0], e[1]) for e in G.edges())
    if nx.is_connected(G):
        diam = nx.diameter(G)
        aspl = nx.average_shortest_path_length(G)
    else:
        diam, aspl = float('inf'), float('inf')
    print('{},{},{},{},{},{},{}'.format(
        ifile,
        G.number_of_nodes() if size is None else
        'x'.join(map(str, size)),
        G.number_of_nodes(), d, r,
        diam, aspl
    ), file=ofile)

def main():
    opts = get_options()
    ostream = open(opts.output, 'w') if opts.output else sys.stdout

    print('name,size,n,d,r,diam,aspl', file=ostream)
    for ifile in opts.ifiles:
        print_row(ifile, ostream, opts.size)

if __name__ == '__main__':
    main()
