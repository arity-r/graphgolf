#pragma once

#include "gg_defs.hpp"

#include <boost/graph/breadth_first_search.hpp>
#include <string>

namespace gg {

  class bfs_visitor : public boost::default_bfs_visitor {
  public:
    std::map<Vertex, unsigned long> *_dist;
    unsigned long *_max_dist;
    unsigned long *_n_v_visited;

    bfs_visitor(std::map<Vertex, unsigned long> *dist,
                unsigned long *max_dist, unsigned long *n_v_visited);
    void initialize_vertex(const Vertex &s, const Graph &G);
    void discover_vertex(const Vertex &u, const Graph &G);
    void tree_edge(const Edge &e, const Graph &G);
    bool all_visited() const;
    unsigned long sum_dist() const;
    unsigned long max_dist() const;
  };

  std::pair<unsigned long, unsigned long> score(const Graph &G);

  Graph read_edgelist(std::string filename);
  void write_edgelist(Graph G, std::string filename);

}
