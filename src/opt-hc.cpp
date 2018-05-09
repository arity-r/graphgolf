
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <boost/program_options.hpp>

#include "gg_util.hpp"
#include "graph_config.hpp"
#include "edge_swap.hpp"

using namespace std;
using namespace gg;
using namespace boost::program_options;

int main(int argc, char* argv[]) {
  string usage_doc =
    "usage: opt-hc.out [-h] --size SIZE [SIZE ...] -r R -i INPUT -o OUTPUT\n";
  usage_doc +=
    "                  [--seed SEED] --step STEP [--log LOG]";
  options_description desc("optional arguments");
  desc.add_options()
    ("help", "show this help message and exit")
    ("size", value<vector<int> >()->multitoken()->required(), "")
    (",r", value<int>()->default_value(-1), "")
    ("input", value<string>()->required(), "")
    ("output", value<string>()->required(), "")
    ("seed", value<unsigned long>(), "")
    ("step", value<int>()->default_value(1000), "")
    ("log", value<string>(), "");

  variables_map vm;
  try {
    store(command_line_parser(argc, argv)
          .options(desc)
          .style (command_line_style::default_style |
                  command_line_style::allow_guessing |
                  command_line_style::allow_long_disguise)
          .run(), vm);
    if(vm.count("help")) {
      cout << usage_doc << endl
           << desc << endl;
      return 0;
    }
    notify(vm);
  } catch(error& e) {
    cout << e.what() << endl
         << usage_doc << endl
         << desc << endl;
    return 1;
  }

  vector<int> size = vm["size"].as<vector<int> >();
  int r = vm["-r"].as<int>();
  GraphConfig conf(size, r);

  string input = vm["input"].as<string>();
  string output = vm["output"].as<string>();
  ofstream log_fs;
  if(vm.count("log"))
    log_fs.open(vm["log"].as<string>());
  if(vm.count("seed")) srand(vm["seed"].as<long int>());
  else srand(time(NULL));

  Graph G = read_edgelist(input);
  size_t n = num_vertices(G);

  pair<int, int> best_score = score(G);
  if(vm.count("log")) {
    log_fs << "t,diam,aspl" << endl;
    log_fs << "0," << best_score.first << ","
           << (double)best_score.second / (n*(n-1)) << endl;
  }
  for(int t = 1; t <= vm["step"].as<int>(); t++) {
    Graph H = G;
    random_swap(&H, conf);
    pair<int, int> curr_score = score(H);
    if(curr_score.first < best_score.first ||
       (curr_score.first == best_score.first &&
        curr_score.second < best_score.second)) {
      G = H;
      best_score = curr_score;
    }
    if(vm.count("log"))
      log_fs << t << "," << best_score.first << ","
             << (double)best_score.second / (n*(n-1)) << endl;
  }

  write_edgelist(G, output);
  if(vm.count("log"))
    log_fs.close();

  return 0;
}
