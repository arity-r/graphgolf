
#include "gg_util.hpp"

#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/spirit/include/qi.hpp>

#include <boost/graph/edge_list.hpp>
#include <fstream>
#include <map>
#include <numeric>
#include <vector>

namespace gg {

  bfs_visitor::bfs_visitor(std::map<Vertex, int> *dist,
                           int *max_dist, int *n_v_visited):
    _dist(dist), _max_dist(max_dist), _n_v_visited(n_v_visited) {
  }

  void bfs_visitor::initialize_vertex(const Vertex &s, const Graph &G) {
    _dist->insert(std::pair<Vertex, int>(s, 0));
  }

  void bfs_visitor::discover_vertex(const Vertex &u, const Graph &G) {
    (*_n_v_visited)++;
  }

  void bfs_visitor::tree_edge(const Edge &e, const Graph &G) {
    _dist->at(target(e, G)) = _dist->at(source(e, G)) + 1;
    if(*_max_dist < _dist->at(target(e, G)))
      *_max_dist = _dist->at(target(e, G));
  }

  bool bfs_visitor::all_visited() const {
    return *_n_v_visited == _dist->size();
  }

  int bfs_visitor::sum_dist() const {
    int dist = 0;
    for(std::map<Vertex, int>::const_iterator it = _dist->begin();
        it != _dist->end(); ++it) {
      dist += it->second;
    }
    return dist;
  }

  int bfs_visitor::max_dist() const {
    return *_max_dist;
  }

  std::pair<int, int> score(const Graph &G) {
    std::map<Vertex, int> dist;
    int n_visited, diam = 0, sspl = 0;
    typedef boost::graph_traits<Graph>::vertex_iterator vert_it;
    vert_it vit, vit_end;
    for(boost::tie(vit, vit_end) = vertices(G); vit != vit_end; ++vit) {
      Vertex s = *vit;
      n_visited = 0;
      dist.clear();
      bfs_visitor vis(&dist, &diam, &n_visited);
      boost::breadth_first_search(G, s, boost::visitor(vis));
      if(!vis.all_visited()) {
        diam = sspl = -1;
        break;
      } else {
        diam = vis.max_dist();
        sspl += vis.sum_dist();
      }
    }
    return std::pair<int, int>(diam, sspl);
  }

  Graph read_edgelist(std::string filename) {
    namespace qi = boost::spirit::qi;

    std::ifstream ifs(filename);
    ifs >> std::noskipws;

    boost::spirit::istream_iterator f(ifs), l;
    std::vector<std::pair<int, int> > edges;
    qi::phrase_parse(f, l, (qi::int_ >> qi::int_) % qi::eol, qi::blank, edges);

    Graph G;
    for(std::vector<std::pair<int, int> >::iterator it = edges.begin();
        it != edges.end(); ++it) {
      Vertex u = (*it).first, v = (*it).second;
      add_edge(u, v, G);
    }

    return G;
  }

  void write_edgelist(Graph G, std::string filename) {
    typedef boost::graph_traits<Graph>::edge_iterator edge_it;

    std::ofstream ofs(filename);
    edge_it ei, ei_end;
    for(boost::tie(ei, ei_end) = edges(G); ei != ei_end; ++ei)
      ofs << source(*ei, G) << " " << target(*ei, G) << std::endl;
    ofs.close();
  }
}
