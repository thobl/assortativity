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
      "Output different variants of the degree distribution as pairs of degree "
      "and frequency. Default is just the normal degree distribution "
      "indicating how many vertices of which degree exist."};

  std::filesystem::path input_file;
  app.arg(input_file, "input_file", "Name of the file containing the graph.");

  bool edge_endpoints = false;
  app.arg(
      edge_endpoints, "--edge_endpoints",
      "Instead of counting each vertex once, each vertex is counted once for "
      "each occurrence as an endpoint of an edge. Thus, the resulting "
      "frequency is proportional to the probability to observe the "
      "corresponding degree when drawing a random endpoint of a random edge. "
      "This can be restricted using `nr_buckets` and `bucket`, which "
      "subdivides the degree endpoints into buckets (with logarithmic axis) "
      "and only counts an endpoint of an edge if the other endpoint is in a "
      "certain degree range.");

  unsigned nr_buckets = 1;
  app.arg(nr_buckets, "--nr_buckets", "The number of buckets.");

  unsigned bucket = 0;
  app.arg(bucket, "--bucket", "The selected bucket.");

  app.arg_header(
      "degree,frequency,edge_endpoints,nr_buckets,bucket,neighbor_deg_lb,"
      "neighbor_deg_ub");

  app.parse(argc, argv);

  double ndeg_lb = 0, ndeg_ub = 0;

  auto write_histogram = [&](const Histogram& H) {
    for (unsigned deg = 0; deg <= H.maximum(); ++deg) {
      if (H.frequency(deg) > 0) {
        print_csv_line(std::cout, deg, H.frequency(deg), edge_endpoints,
                       nr_buckets, bucket, ndeg_lb, ndeg_ub);
      }
    }
  };

  std::ifstream f(input_file);
  EdgeList EL(f);

  // degree distribution (random vertex)
  Histogram H_deg;
  if (EL.type() == BIPARTITE) {
    for (Node v = 0; v < EL.n1(); ++v) H_deg.add(EL.deg(OUT, v));
    for (Node v = 0; v < EL.n2(); ++v) H_deg.add(EL.deg(IN, v));
  } else {
    for (Node v = 0; v < EL.n(); ++v) H_deg.add(EL.deg(SUM, v));
  }

  if (!edge_endpoints) {
    write_histogram(H_deg);
    return 0;
  }

  // edge endpoints
  Histogram H;
  
  // logarithmic breaks -> upper and lower bounds for neighbor degree
  auto breaks = log_breaks(1, H_deg.maximum() + 1, nr_buckets);
  ndeg_lb = breaks[bucket];
  ndeg_ub = breaks[bucket + 1];

  for (auto& e : EL.edges()) {
    unsigned deg_s =
        EL.type() != BIPARTITE ? EL.deg(SUM, e.s) : EL.deg(OUT, e.s);
    unsigned deg_t =
        EL.type() != BIPARTITE ? EL.deg(SUM, e.t) : EL.deg(IN, e.t);
    if (ndeg_lb <= deg_s && deg_s <= ndeg_ub) H.add(deg_t);
    if (ndeg_lb <= deg_t && deg_t <= ndeg_ub) H.add(deg_s);
  }
  write_histogram(H);

  return 0;
}
