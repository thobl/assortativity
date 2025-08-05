#include <filesystem>
#include <fstream>
#include <iostream>

#include "app.hpp"
#include "types.hpp"
#include "edge_list.hpp"

int main(int argc, char** argv) {
  App app{"Output the degrees of the endpoints for each edge."};

  std::filesystem::path input_file;
  app.arg(input_file, "input_file", "Name of the file containing the graph.");

  app.arg_header("source,target");

  app.parse(argc, argv);

  std::ifstream f(input_file);
  EdgeList EL(f);
  auto points = EL.degree_points(SUM, SUM);
  for (auto& p : points) {
    std::cout << p.x << "," << p.y << "\n";
  }

  return 0;
}
