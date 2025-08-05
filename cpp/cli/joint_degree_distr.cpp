#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <utility>

#include "CLI11.hpp"
#include "app.hpp"
#include "edge_list.hpp"
#include "histogram.hpp"
#include "types.hpp"

int main(int argc, char** argv) {
  App app{
      "Output the joint degree distribution of the two endpoints of an edge, "
      "i.e., for each pair of degrees the frequency with which this pair "
      "appears. For undirected graphs, each edge contributes two points (one "
      "for each possible direction). For directed and bipartite graphs, each "
      "edge contributes one point. For directed graphs, the degree of the "
      "vertices is the sum of in and out degree."};

  std::filesystem::path input_file;
  app.arg(input_file, "input_file", "Name of the file containing the graph.");

  app.arg_header("degree_s,degree_t,frequency");

  app.parse(argc, argv);

  std::ifstream f(input_file);
  EdgeList EL(f);

  // create histogram
  auto points = EL.type() != BIPARTITE ? EL.degree_points(SUM, SUM)
                                       : EL.degree_points(OUT, IN);
  std::sort(points.begin(), points.end(), [](Point p1, Point p2) {
    if (p1.x == p2.x) return p1.y < p2.y;
    return p1.x < p2.x;
  });

  // sentinel element at the end
  points.push_back({0, 0});

  Point prev_point = points[0];
  unsigned count = 1;

  for (unsigned i = 1; i < points.size(); ++i) {
    if (points[i].x == prev_point.x && points[i].y == prev_point.y) {
      count++;
      continue;
    }
    std::cout << prev_point.x << "," << prev_point.y << "," << count << "\n";
    prev_point = points[i];
    count = 1;
  }

  return 0;
}
