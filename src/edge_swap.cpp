
#include "edge_swap.hpp"

#include <cassert>
#include <iostream>

namespace gg {
  bool edge_swap(Graph *G, GraphConfig conf, Edge e, Edge f) {
    return edge_swap(G, conf, source(e, *G), target(e, *G),
                     source(f, *G), target(f, *G));
  }

  bool edge_swap(Graph *G, GraphConfig conf,
                 Vertex u, Vertex v, Vertex x, Vertex y) {
    if(conf.are_neighbors(u, x) && conf.are_neighbors(v, y) &&
       !edge(u, x, *G).second && !edge(v, y, *G).second) {
      add_edge(u, x, *G);
      add_edge(v, y, *G);
      remove_edge(u, v, *G);
      remove_edge(x, y, *G);
      return true;
    } else
      return false;
  }

  bool random_swap(Graph *G, GraphConfig conf,
                   int nswap, int max_tries_per_swap) {
    typedef boost::graph_traits<Graph>::adjacency_iterator adj_it;
    typedef boost::graph_traits<Graph>::vertex_iterator vert_it;
    int tries = 0, swapcount = 0, max_tries = nswap * max_tries_per_swap;
    while(swapcount < nswap) {
      Vertex u = vertex(rand() % num_vertices(*G), *G);
      adj_it neighbors, neighbors_end;
      boost::tie(neighbors, neighbors_end) = adjacent_vertices(u, *G);
      Vertex v = *(neighbors + rand() % (neighbors_end - neighbors));
      assert(conf.are_neighbors(u, v));

      std::vector<Vertex> xs;
      vert_it vi, vi_end;
      // TODO std::copy_if
      for(boost::tie(vi, vi_end) = vertices(*G); vi != vi_end; ++vi) {
        Vertex x = *vi;
        if(x != u && x != v &&
           conf.are_neighbors(x, u) && conf.are_neighbors(x, v))
          xs.push_back(x);
      }
      Vertex x = xs[rand() % xs.size()];

      boost::tie(neighbors, neighbors_end) = adjacent_vertices(x, *G);
      xs.clear();
      std::copy(neighbors, neighbors_end, std::back_inserter(xs));
      std::vector<Vertex> ys;
      if(conf.are_neighbors(u, x) && conf.are_neighbors(v, x)) {
        std::copy_if(xs.begin(), xs.end(), std::back_inserter(ys),
                     [u, v, conf](const Vertex &y) {
                       return y != u && y != v &&
                         (conf.are_neighbors(y, u) || conf.are_neighbors(y, v));
                     });
      } else if(conf.are_neighbors(u, x)) {
        std::copy_if(xs.begin(), xs.end(), std::back_inserter(ys),
                     [u, v, conf](const Vertex &y) {
                       return y != u && y != v && conf.are_neighbors(y, v);
                     });
      } else if(conf.are_neighbors(v, x)) {
        std::copy_if(xs.begin(), xs.end(), std::back_inserter(ys),
                     [u, v, conf](const Vertex &y) {
                       return y != u && y != v && conf.are_neighbors(y, u);
                     });
      } else {
        assert(0);
      }
      if(ys.size() == 0)
        continue;
      Vertex y = ys[rand() % ys.size()];
      assert((conf.are_neighbors(u, x) && conf.are_neighbors(v, y)) ||
             (conf.are_neighbors(u, y) && conf.are_neighbors(v, x)));

      bool success;
      if(conf.are_neighbors(u, x) && conf.are_neighbors(v, y))
        success = edge_swap(G, conf, u, v, x, y);
      else if(conf.are_neighbors(u, y) && conf.are_neighbors(v, x))
        success = edge_swap(G, conf, u, v, y, x);
      else
        assert(0);
      if(success)
        swapcount += 1;

      if(tries >= max_tries) {
        std::cerr << "Maximum number of swap trials (" << max_tries_per_swap
                  << ") exceeded";
        return false;
      }
      tries += 1;
    }
    return true;
  }
}
