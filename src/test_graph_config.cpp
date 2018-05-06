
#include <iostream>
#include <iterator>
#include "graph_config.hpp"

using namespace std;
using namespace gg;

template<typename T>
ostream & operator<<(ostream & os, vector<T> vec) {
				os<<"[";
				if(vec.size()!=0) {
								std::copy(vec.begin(), vec.end()-1,
													std::ostream_iterator<T>(os, " "));
								os<<vec.back();
				}
				os<<"]";
				return os;
}

int main() {
				/*
					 0  1  2  3  4  5  6  7
					 8  9 10 11 12 13 14 15
					16 17 18 19 20 21 22 23
					24 25 26 27 28 29 30 31
				*/
				int r = 2;
				vector<int> dimen, loc;
				dimen.push_back(4); dimen.push_back(8);
				loc.push_back(1); loc.push_back(2);
				GraphConfig conf1 = GraphConfig(dimen, r);
				cout << "conf1.vid(1, 2) = " << conf1.vid(loc) << endl;
				cout << "conf1.loc(19) = " << conf1.loc(19) << endl;
				cout << "conf1.dist(10, 20) = " << conf1.dist(10, 20) << endl;
				cout << "conf1.are_neighbors(11, 17) = " <<
								conf1.are_neighbors(11, 17) << endl;
				cout << "conf1.neighbors(11) = " << conf1.neighbors(11) << endl;

				return 0;
}
