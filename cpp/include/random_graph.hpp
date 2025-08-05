#pragma once

#include <vector>

#include "types.hpp"

inline double avg_deg_to_p(unsigned n, double avg_deg) {
  return avg_deg / (n - 1);
}

std::vector<Edge> gilbert(unsigned n, double p);

std::vector<Edge> erdos_renyi(unsigned n, unsigned m);

std::vector<Edge> chung_lu(unsigned n, double ple, double avg_deg,
                           double sigma = 1);

std::vector<double> power_law_weights(unsigned n, double ple);
