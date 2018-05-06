#pragma once

#include <vector>

#include "gg_defs.hpp"

namespace gg {

	class GraphConfig {
		std::vector<int> _dim;
		std::vector<int> _const;
		int _n;
		int _r;
	public:
		GraphConfig(const std::vector<int> &dim, const int &r=-1);
		int get_r() const { return _r; }
		void set_r(const int &r) { _r = r; }
		std::vector<int> get_dim() const { return _dim; }
		int get_n() const { return _n; }
		Vertex vid(const std::vector<int> &loc) const;
		std::vector<int> loc(const Vertex &vid) const;
		int dist(const Vertex &u, const Vertex &v) const;
		bool are_neighbors(const Vertex &u, const Vertex &v) const;
		std::vector<Vertex> neighbors(const Vertex &v) const;
	};

}
