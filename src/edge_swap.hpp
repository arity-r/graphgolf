#pragma once

#include <boost/graph/adjacency_list.hpp>
#include "gg_defs.hpp"
#include "graph_config.hpp"

namespace gg {
  bool select_edges_nodim(Graph *G,
                          Vertex *u, Vertex *v, Vertex *x, Vertex *y);
  bool select_edges_dim(Graph *G, GraphConfig conf,
                        Vertex *u, Vertex *v, Vertex *x, Vertex *y);
  bool edge_swap(Graph *G, GraphConfig conf, Edge e, Edge f);
  bool edge_swap(Graph *G, GraphConfig conf,
                 Vertex u, Vertex v, Vertex x, Vertex y);
  bool random_swap(Graph *G, GraphConfig conf,
                   int nswap=1, int max_tries_per_swap=10);
}
