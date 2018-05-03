#!/usr/bin/env python3

import sys
import networkx as nx
from math import exp, log
from random import random, seed
from argparse import ArgumentParser
from graph_config import GraphConfig
from edge_swap import random_swap

def optimize(G, conf, param):
    def write_log_row(t, diam, aspl, ofile):
        print(
            '{},{},{},{},{},{}'.format(
                param.T, param.heat_loss, param.iteration, t, diam, aspl
            ),
            file=ofile
        )

    n = conf.get_n()
    t = param.T
    cooling_rate = 1 - param.heat_loss

    diam, aspl = nx.diameter(G), nx.average_shortest_path_length(G)
    score = diam*n**3 + aspl*n*(n-1)

    if param.log:
        log_fp = open(param.log, 'w')
        print('T,heat-loss,iteration,t,diam,aspl', file=log_fp)
        write_log_row(t, diam, aspl, log_fp)

    total_iteration = int(-log(param.T)/log(cooling_rate)*param.iteration)
    iprint, nprint = 1, 1000
    for i in range(1, total_iteration):
        H = random_swap(G.copy(), conf)
        if nx.is_connected(H):
            diam_H = nx.diameter(H)
            aspl_H = nx.average_shortest_path_length(H)
            score_H = diam_H*n**3 + aspl_H*n*(n-1)
            if score_H < score or random() < exp((score - score_H) / t):
                G = H
                diam, aspl = diam_H, aspl_H
                score = score_H
        if i % param.iteration == 0:
            t *= cooling_rate
        if i == int(iprint * total_iteration / nprint):
            iprint += 1
            if param.log:
                write_log_row(t, diam, aspl, log_fp)

    if param.log:
        log_fp.close()
    return G

def get_options():
    parser = ArgumentParser()
    parser.add_argument('--size', type=int, required=True, nargs='+')
    parser.add_argument('-r', type=int, default=-1)
    parser.add_argument('-T', type=float, default=10)
    parser.add_argument('--heat-loss', type=float, default=1e-3)
    parser.add_argument('--iteration', type=int, default=1)
    parser.add_argument('-i', '--input', type=str, required=True)
    parser.add_argument('-o', '--output', type=str, required=True)
    parser.add_argument('--log', type=str)
    parser.add_argument('--seed', type=int)
    opts = parser.parse_args()
    return opts

def main():
    opts = get_options()
    seed(opts.seed)
    G = nx.read_edgelist(opts.input, nodetype=int)
    G = optimize(G, GraphConfig(opts.size, opts.r), opts)
    nx.write_edgelist(G, opts.output, data=False)

if __name__ == '__main__':
    main()
