#include <filesystem>
#include <functional>
#include <iostream>
#include <istream>
#include <tuple>

#include "app.hpp"
#include "edge_list.hpp"
#include "kendall.hpp"
#include "pearson.hpp"
#include "spearman.hpp"

void print_stats(EdgeList& EL) {
  std::vector<std::pair<DegType, DegType>> variants;
  switch (EL.type()) {
    case UNDIRECTED:
      variants = {{SUM, SUM}};
      break;
    case BIPARTITE:
      variants = {{OUT, IN}};
      break;
    case DIRECTED:
      variants = {{OUT, IN}, {IN, OUT}, {OUT, OUT}, {IN, IN}, {SUM, SUM}};
  }

  for (auto [source_type, target_type] : variants) {
    // basic stats
    std::cout << name(EL.type()) << "," << EL.n() << "," << EL.n1() << ","
              << EL.n2() << "," << EL.m() << ",";

    // assortativity
    auto points = EL.degree_points(source_type, target_type);
    std::cout << name(source_type) << "," << name(target_type) << ",";

    std::cout << pearson(points) << ",";
    std::cout << spearman(points) << ",";

    Kendall K(points, EL.type() == UNDIRECTED);
    std::cout << K.tau_a() << "," << K.tau_b() << "," << K.tau_simple() << ","
              << K.concordant() << "," << K.discordant() << "," << K.ties_x()
              << "," << K.ties_y() << "," << K.ties_both() << std::endl;
  }
}

int main(int argc, char** argv) {
  App app{"Compute several stats of a graph."};

  std::filesystem::path input_file;
  app.arg(input_file, "input_file", "Name of the file containing the graph.");

  app.arg_header(
      "type,n,n1,n2,m,source_type,target_type,pearson,spearman,kendall_tau_a,"
      "kendall_tau_b,kendall_tau_simple,concordant,discordant,ties_x,ties_y,"
      "ties_both");

  app.parse(argc, argv);

  std::ifstream input(input_file);
  EdgeList EL(input);

  print_stats(EL);
  return 0;
}
