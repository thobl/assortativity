#include "girg.hpp"

#include <omp.h>

#include <cassert>
#include <cmath>
#include <limits>
#include <vector>

#include "CLI11.hpp"
#include "app.hpp"
#include "random.hpp"
#include "types.hpp"

int main(int argc, char** argv) {
  App app{"Generating geometric inhomogeneous random graphs."};

  std::string output_file = "output_graph";
  app.arg(output_file, "output_file", "Name of the resulting graph file.");

  unsigned n = 100;
  app.arg(n, "--nr_nodes,-n",
          "Number of nodes of the generated graph (default: 100).");

  double deg = 15;
  app.arg(deg, "--deg", "Expected average degree (default: 15).");

  unsigned dim = 1;
  app.arg(dim, "--dim", "The Dimension of the ground space (default: 1).",
          CLI::PositiveNumber);

  double ple = 2.5;
  app.arg(ple, "--ple", "The power-law exponent (default: 2.5).",
          CLI::PositiveNumber & !CLI::Range(0.0, 2.0));

  double T = 0;
  app.arg(T, "--temperature,-T", "The temperature (default: 0).",
          CLI::Range(0.0, 1.0));

  double sigma = 1;
  app.arg(sigma, "--sigma",
          "The exponent controlling the assortativity (default: 1).",
          CLI::Range(0.0, 2.0));

  bool print_ipe = false;
  app.arg(print_ipe, "--ipe", "Set to print the coordinates.");  

  app.arg_header("n,deg,dim,ple,T,alpha,sigma,seed");

  app.arg_seed();

  app.parse(argc, argv);

  omp_set_num_threads(1);

  double correction_factor = 1.0;
  std::string ipe_output = print_ipe && sigma == 1.0 ? output_file + ".ipe" : "";
  auto edges = agirg(n, ple, deg, dim, T, sigma, 1.0// , ipe_output
                     );

  // if sigma != 1, the average degree is probably off quite a bit ->
  // correct it by running it again with a correction factor
  if (sigma != 1.0 || ipe_output != "") {
    // adjusting the average degree by a factor either under- or
    // overshoots the desired amount of change (depending on σ) -> fix
    // this by dampen (or amplify) the amount of change depending on σ
    auto dampen = [&sigma](double factor) {
      return std::pow(factor, 1.2 / sigma);
    };
    
    auto log = [&]() {
      // std::cout << "correction: " << correction_factor << std::endl;
      // std::cout << "avg deg: " << edges.size() * 2.0 / n << std::endl;
    };

    // adjusting the average degree three times yields graphs with
    // roughly the right degree in most cases
    for (int i = 0; i < 3; ++i) {
      if (i == 0) log();
      correction_factor *= dampen(0.5 * n * deg / edges.size());
      ipe_output = print_ipe && i == 2 ? output_file + ".ipe" : "";
      edges = agirg(n, ple, deg, dim, T, sigma, correction_factor, ipe_output);
      log();
    }
  }

  // subsample and write edges
  std::ofstream out{output_file};
  for (auto& e : edges) {
    auto [u, v] = e;
    out << u << ' ' << v << '\n';
  }

  // output parameters
  double alpha = T > 0 ? 1 / T : std::numeric_limits<double>::infinity();
  std::cout << n << "," << deg << "," << dim << "," << ple << "," << T << ","
            << alpha << "," << sigma << "," << Random::get_seed() << std::endl;

  return 0;
}
