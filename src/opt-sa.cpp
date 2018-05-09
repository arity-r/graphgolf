
#include <cmath>
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
    "usage: opt-sa.py [-h] --size SIZE [SIZE ...] [-r R] [-T T]";
  usage_doc +=
    "[--heat-loss HEAT_LOSS] [--iteration ITERATION] -i INPUT -o";
  usage_doc +=
    "OUTPUT [--log LOG] [--seed SEED]";
  options_description desc("optional arguments");
  desc.add_options()
    ("help", "show this help message and exit")
    ("size", value<vector<int> >()->multitoken(), "")
    (",r", value<int>()->default_value(-1), "")
    (",T", value<double>()->default_value(10), "")
    ("heat-loss", value<double>()->default_value(1e-3), "")
    //("iteration", value<int>()->default_value(1), "")
    ("input", value<string>()->required(), "")
    ("output", value<string>()->required(), "")
    ("seed", value<unsigned long>(), "")
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


  string input = vm["input"].as<string>();
  string output = vm["output"].as<string>();
  ofstream log_fs;
  if(vm.count("log"))
    log_fs.open(vm["log"].as<string>());
  if(vm.count("seed")) srand(vm["seed"].as<long int>());
  else srand(time(NULL));

  Graph G = read_edgelist(input);
  size_t n = num_vertices(G);

  vector<int> size;
  if(!vm.count("size")) size.push_back(n);
  else size = vm["size"].as<vector<int> >();
  int r = vm["-r"].as<int>();
  GraphConfig conf(size, r);

  double T = vm["-T"].as<double>();
  int iteration = 1;
  double heat_loss = vm["heat-loss"].as<double>();
  double cooling_rate = 1 - heat_loss;
  pair<unsigned long, unsigned long> best_score_pair = score(G);
  double best_score = best_score_pair.first*n*n*n + best_score_pair.second;
  if(vm.count("log")) {
    log_fs << "T,heat-loss,t,diam,aspl" << endl;
    log_fs << T << "," << heat_loss << "," << T << ","
           << best_score_pair.first << ","
           << (double)best_score_pair.second / (n*(n-1)) << endl;
  }

  unsigned long long total_iteration =
    (unsigned long long)(-log(T)/log(cooling_rate)*iteration);
  double t = T;
  int iprint = 1, nprint = 1000;
  for(unsigned long i = 1; i < total_iteration; ++i) {
    Graph H = G;
    random_swap(&H, conf);
    pair<unsigned long, unsigned long> curr_score_pair = score(H);
    double curr_score = curr_score_pair.first*n*n*n + curr_score_pair.second;
    if(curr_score < best_score ||
       (double)rand() / RAND_MAX < exp((best_score - curr_score) / t)) {
      G = H;
      best_score_pair = curr_score_pair;
      best_score = curr_score;
    }
    if(i % iteration == 0)
      t *= cooling_rate;
    if(i == (unsigned long long)((double)iprint * total_iteration / nprint)) {
      iprint++;
      if(vm.count("log"))
        log_fs << T << "," << heat_loss << "," << t << ","
               << best_score_pair.first << ","
               << (double)best_score_pair.second / (n*(n-1)) << endl;
    }
  }

  write_edgelist(G, output);
  if(vm.count("log"))
    log_fs.close();

  return 0;
}
