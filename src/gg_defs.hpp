#pragma once

#include <boost/graph/adjacency_list.hpp>

namespace gg {
  typedef boost::adjacency_list<
    boost::vecS, boost::vecS, boost::undirectedS> Graph;
  typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
  typedef boost::graph_traits<Graph>::edge_descriptor Edge;
}
