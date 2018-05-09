
#include "edge_swap.hpp"

#include <cassert>
#include <iostream>

namespace gg {
  bool select_edges_nodim(Graph *G,
                          Vertex *u, Vertex *v, Vertex *x, Vertex *y) {
    typedef boost::graph_traits<Graph>::adjacency_iterator adj_it;
    adj_it neighbors, neighbors_end;
    Vertex _u = vertex(rand() % num_vertices(*G), *G);
    boost::tie(neighbors, neighbors_end) = adjacent_vertices(_u, *G);
    Vertex _v = *(neighbors + rand() % (neighbors_end - neighbors));
    Vertex _x = vertex(rand() % num_vertices(*G), *G);
    boost::tie(neighbors, neighbors_end) = adjacent_vertices(_x, *G);
    Vertex _y = *(neighbors + rand() % (neighbors_end - neighbors));
    if((_u == _x && _v == _y) || (_u == _y && _v == _x)) {
      return false;
    } else {
      *u = _u; *v = _v;
      *x = _x; *y = _y;
      return true;
    }
  }

  bool select_edges_dim(Graph *G, GraphConfig conf,
                        Vertex *u, Vertex *v, Vertex *x, Vertex *y) {
    typedef boost::graph_traits<Graph>::adjacency_iterator adj_it;
    typedef boost::graph_traits<Graph>::vertex_iterator vert_it;
    Vertex _u = vertex(rand() % num_vertices(*G), *G);
    adj_it neighbors, neighbors_end;
    boost::tie(neighbors, neighbors_end) = adjacent_vertices(_u, *G);
    Vertex _v = *(neighbors + rand() % (neighbors_end - neighbors));
    assert(conf.are_neighbors(_u, _v));

    std::vector<Vertex> xs;
    vert_it vi, vi_end;
    for(boost::tie(vi, vi_end) = vertices(*G); vi != vi_end; ++vi) {
      Vertex _x = *vi;
      if(_x != _u && _x != _v &&
         conf.are_neighbors(_x, _u) && conf.are_neighbors(_x, _v))
        xs.push_back(_x);
    }
    Vertex _x = xs[rand() % xs.size()];

    boost::tie(neighbors, neighbors_end) = adjacent_vertices(_x, *G);
    xs.clear();
    std::copy(neighbors, neighbors_end, std::back_inserter(xs));
    std::vector<Vertex> ys;
    if(conf.are_neighbors(_u, _x) && conf.are_neighbors(_v, _x)) {
      std::copy_if(xs.begin(), xs.end(), std::back_inserter(ys),
                   [_u, _v, conf](const Vertex &_y) {
                     return _y != _u && _y != _v &&
                       (conf.are_neighbors(_y, _u) || conf.are_neighbors(_y, _v));
                   });
    } else if(conf.are_neighbors(_u, _x)) {
      std::copy_if(xs.begin(), xs.end(), std::back_inserter(ys),
                   [_u, _v, conf](const Vertex &_y) {
                     return _y != _u && _y != _v && conf.are_neighbors(_y, _v);
                   });
    } else /* if(conf.are_neighbors(_v, _x)) */ {
      std::copy_if(xs.begin(), xs.end(), std::back_inserter(ys),
                   [_u, _v, conf](const Vertex &_y) {
                     return _y != _u && _y != _v && conf.are_neighbors(_y, _u);
                   });
    }
    if(ys.size() == 0) {
      return false;
    } else {
      Vertex _y = ys[rand() % ys.size()];
      *u = _u;
      *v = _v;
      if(conf.are_neighbors(_u, _x) && conf.are_neighbors(_v, _y)) {
        *x = _x;
        *y = _y;
      }
      else if(conf.are_neighbors(_u, _y) && conf.are_neighbors(_v, _x)) {
        *x = _y;
        *y = _x;
      }
      return true;
    }
  }

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
    int tries = 0, swapcount = 0, max_tries = nswap * max_tries_per_swap;
    while(swapcount < nswap) {
      tries += 1;
      if(tries > max_tries) {
        std::cerr << "Maximum number of swap trials (" << max_tries_per_swap
                  << ") exceeded";
        return false;
      }
      Vertex u, v, x, y;
      bool selected;
      if(conf.get_dim().size() == 1 || conf.get_r() < 0)
        selected = select_edges_nodim(G, &u, &v, &x, &y);
      else
        selected = select_edges_dim(G, conf, &u, &v, &x, &y);
      if(!selected) continue;
      if(edge_swap(G, conf, u, v, x, y))
        swapcount += 1;
    }
    return true;
  }
}
