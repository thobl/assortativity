#include <fstream>

#include "app.hpp"
#include "edge_list.hpp"
#include "random_graph.hpp"

int main(int argc, char** argv) {
  App app{"Generating Chung-Lu random graphs with power-law distribution."};

  std::string output_file = "output_graph";
  app.arg(output_file, "output_file", "Name of the resulting graph file.");

  unsigned n = 100;
  app.arg(n, "--nr_nodes,-n", "Number of nodes of the generated graph.");

  double deg = 15;
  app.arg(deg, "--deg", "Expected average degree (default: 15).");

  double ple = 2.5;
  app.arg(ple, "--ple", "The power-law exponent (default: 2.5).",
          CLI::PositiveNumber & !CLI::Range(0.0, 2.0));

  double sigma = 1.0;
  app.arg(sigma, "--sigma",
          "The exponent controlling the assortativity (default: 1).");

  app.arg_header("n,deg,ple,sigma,seed");

  app.arg_seed();
  app.parse(argc, argv);

  auto edges = chung_lu(n, ple, deg, sigma);

  std::ofstream out(output_file);
  print_edges(edges, out);

  std::cout << n << "," << deg << "," << ple << "," << sigma << ","
            << Random::get_seed() << std::endl;

  return 0;
}
