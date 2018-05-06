#pragma once

#include "gg_defs.hpp"

#include <boost/graph/breadth_first_search.hpp>
#include <string>

namespace gg {

  class bfs_visitor : public boost::default_bfs_visitor {
  public:
    std::map<Vertex, int> *_dist;
    int *_max_dist;
    int *_n_v_visited;

    bfs_visitor(std::map<Vertex, int> *dist, int *max_dist, int *n_v_visited);
    void initialize_vertex(const Vertex &s, const Graph &G);
    void discover_vertex(const Vertex &u, const Graph &G);
    void tree_edge(const Edge &e, const Graph &G);
    bool all_visited() const;
    int sum_dist() const;
    int max_dist() const;
  };

  std::pair<int, int> score(const Graph &G);

  Graph read_edgelist(std::string filename);
  void write_edgelist(Graph G, std::string filename);

}
