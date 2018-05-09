#include "graph_config.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <numeric>
#include <utility>

#include <iostream>

namespace gg {

	GraphConfig::GraphConfig(const std::vector<int> &dim, const int &r) :
		_dim(dim), _r(r) {
		_const = std::vector<int>(dim.size());
		_const[dim.size()-1] = 1;
		for(int i = dim.size()-2; i >= 0; i--)
			_const[i] = _const[i+1] * dim[i+1];
		_n = _const[0] * dim[0];
		_r = r;
	}

	Vertex GraphConfig::vid(const std::vector<int> &loc) const {
		assert(_dim.size() == loc.size());
		for(std::size_t i = 0; i < loc.size(); i++)
			if(loc[i] < 0 || loc[i] >= _dim[i])
				return -1;
		Vertex _vid = 0;
		for(std::size_t i = 0; i < loc.size(); i++)
			_vid += loc[i] * _const[i];
		return _vid;
	}

	std::vector<int> GraphConfig::loc(const Vertex &vid) const {
		std::vector<int> _loc(_dim.size());
		for(std::size_t i = 0; i < _dim.size(); i++)
			_loc[i] = vid / _const[i] % _dim[i];
		return _loc;
	}

	int GraphConfig::dist(const Vertex &u, const Vertex &v) const {
		std::vector<int> loc_u = loc(u);
		std::vector<int> loc_v = loc(v);
		int _dist = 0;
		for(std::size_t i = 0; i < loc_u.size(); i++)
			_dist += abs(loc_u[i] - loc_v[i]);
		return _dist;
	}

	bool GraphConfig::are_neighbors(const Vertex &u, const Vertex &v) const {
		if(_r < 0) return true;
		else return u != v && dist(u, v) <= _r;
	}

	std::vector<Vertex> GraphConfig::neighbors(const Vertex &v) const {
		std::vector<Vertex> nei;
    if(_dim.size() == 1 || _r < 0) {
      nei.resize(_n - 1);
      std::generate(nei.begin(), nei.begin()+(v-1),
                    [n = 0] () mutable { return n++; });
      std::generate(nei.begin()+(v+1), nei.end(),
                    [n = v+1] () mutable { return n++; });
    } else {
      std::vector<int> minidim(_dim.size());
      std::fill(minidim.begin(), minidim.end(), 2*_r+1);
      GraphConfig miniconf(minidim);
      int maxidx = std::accumulate(minidim.begin(), minidim.end(), 1,
                                   std::multiplies<int>());
      std::vector<int> v_loc = this->loc(v);
      for(int di = 1; di <= maxidx; di++) {
        std::vector<int> loc(_dim.size());
        bool valid_loc = true;
        std::vector<int> diff = miniconf.loc(di);
        std::transform(diff.begin(), diff.end(), diff.begin(),
                       [r = _r](int d) -> int { return d - r; });
        int sumdiff = 0;
        for(size_t i = 0; i < loc.size(); i++) {
          loc[i] = v_loc[i] + diff[i];
          sumdiff += abs(diff[i]);
          std::cout << loc[i] << " ";
          if(loc[i] < 0 || loc[i] >= _dim[i] || sumdiff > _r) {
            valid_loc = false;
            break;
          }
        }
        std::cout << std::endl;
        if(!valid_loc || sumdiff == 0) continue;
        Vertex w = vid(loc);
        if(w >= 0) nei.push_back(w);
      }
      std::cout << std::endl;
    }
		return nei;
	}

}
