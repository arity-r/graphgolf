
import sys
import random
import networkx as nx

from graph_config import GraphConfig

def edge_swap(G, conf, u, v, x, y):
    if conf.are_neighbors(u, x) and conf.are_neighbors(v, y) and\
       (x not in G[u]) and (y not in G[v]):
        G.add_edge(u, x)
        G.add_edge(v, y)
        G.remove_edge(u, v)
        G.remove_edge(x, y)
        return True
    return False

def random_swap(G, conf, nswap=1, max_tries_per_swap=10):
    tries = 0
    swapcount = 0
    max_tries = nswap * max_tries_per_swap
    while swapcount < nswap:
        u = random.randint(0, len(G)-1)
        v = random.choice(list(G.neighbors(u)))
        assert conf.are_neighbors(u, v)
        nu = set(conf.neighbors(u))
        nv = set(conf.neighbors(v))
        x = random.choice(tuple(nu.union(nv).difference([u, v])))

        if conf.are_neighbors(u, x) and conf.are_neighbors(v, x):
            candidates = [y for y in G.neighbors(x) if
                          y != u and y != v and\
                          (conf.are_neighbors(u, y) or conf.are_neighbors(v, y))]
        elif conf.are_neighbors(u, x):
            candidates = [y for y in G.neighbors(x) if
                          y != u and y != v and conf.are_neighbors(v, y)]
        else: # conf.are_neighbors(v, x):
            candidates = [y for y in G.neighbors(x) if
                          y != u and y != v and conf.are_neighbors(u, y)]
        if len(candidates) == 0:
            continue

        y = random.choice(candidates)
        assert (conf.are_neighbors(u, x) and conf.are_neighbors(v, y)) or\
            (conf.are_neighbors(u, y) and conf.are_neighbors(v, x))
        if conf.are_neighbors(u, x) and conf.are_neighbors(v, y):
            success = edge_swap(G, conf, u, v, x, y)
        elif conf.are_neighbors(u, y) and conf.are_neighbors(v, x):
            success = edge_swap(G, conf, u, v, y, x)
        if success:
            swapcount += 1

        if tries >= max_tries:
            msg = 'Maximum number of swap trials (%s) exceeded' % max_tries
            raise RuntimeError(msg)
        tries += 1

    return G
