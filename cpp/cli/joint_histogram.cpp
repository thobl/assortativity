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
  App app{"todo"};

  std::filesystem::path input_file;
  app.arg(input_file, "input_file", "Name of the file containing the graph.");

  unsigned nr_buckets = 20;
  app.arg(nr_buckets, "--nr_buckets", "The number of buckets.");

  bool complementary_cumulative = false;
  app.arg(complementary_cumulative, "--complementary_cumulative",
          "Makes the histogram complementary cumulative, i.e., cell (x, y) "
          "contains the number of points with x-value and y-value at least x "
          "and y, respectively.");

  app.arg_header(
      "lb_x,ub_x,center_x,lb_y,ub_y,center_y,count,nr_buckets,compl_cumul");

  app.parse(argc, argv);

  std::ifstream f(input_file);
  EdgeList EL(f);

  // create histogram
  auto points = EL.type() != BIPARTITE ? EL.degree_points(SUM, SUM)
                                       : EL.degree_points(OUT, IN);

  Histogram2D H{points, nr_buckets, complementary_cumulative};

  for (unsigned x = 0; x < nr_buckets; ++x) {
    for (unsigned y = 0; y < nr_buckets; ++y) {
      print_csv_line(std::cout,                            //
                     H.lb_x(x), H.ub_x(x), H.center_x(x),  //
                     H.lb_y(y), H.ub_y(y), H.center_y(y),  //
                     H.count(x, y),                        //
                     nr_buckets, complementary_cumulative);
    }
  }

  return 0;
}
