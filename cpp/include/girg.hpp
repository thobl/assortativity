#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "doctest.h"
#include "girgs/Generator.h"
#include "ipe.hpp"
#include "random.hpp"
#include "random_graph.hpp"
#include "types.hpp"

inline void print_graph(const std::vector<Edge>& E,
                        const std::vector<std::vector<double>>& p,
                        const std::vector<double>& w,
                        const std::string& filename, double sigma) {
  IpeFile ipe(filename, 400, 40);

  auto sqr_dist = [](std::vector<double> p, std::vector<double> q) {
    return (p[0] - q[0]) * (p[0] - q[0]) + (p[1] - q[1]) * (p[1] - q[1]);
  };

  ipe.start_group_with_clipping(0, 0, 1, 1);
  for (auto& e : E) {
    std::vector<double> s_pos = p[e.s];
    std::vector<std::vector<double>> t_cand_pos(0);
    std::vector<std::vector<double>> offsets;
    for (auto x : {-1.0, 0.0, 1.0}) {
      for (auto y : {-1.0, 0.0, 1.0}) {
        offsets.push_back({x, y});
        t_cand_pos.push_back({p[e.t][0] + x, p[e.t][1] + y});
      }
    }

    std::vector<double> offset = *std::min_element(
        offsets.begin(), offsets.end(), [&](auto& o1, auto& o2) {
          return sqr_dist(s_pos, {p[e.t][0] + o1[0], p[e.t][1] + o1[1]}) <
                 sqr_dist(s_pos, {p[e.t][0] + o2[0], p[e.t][1] + o2[1]});
        });

    std::vector<double> t_pos = {p[e.t][0] + offset[0], p[e.t][1] + offset[1]};

    for (auto& o : offsets) {
      ipe.line(s_pos[0] + o[0], s_pos[1] + o[1], t_pos[0] + o[0],
               t_pos[1] + o[1]);
    }
    // ipe.line(p[e.s][0], p[e.s][1], p[e.t][0], p[e.t][1]);
  }
  ipe.end_group();

  ipe.start_group();
  for (unsigned i = 0; i < p.size(); ++i) {
    ipe.disk(p[i][0], p[i][1], 1.5 * std::sqrt(w[i]));
  }
  ipe.end_group();
  ipe.box(0, 0, 1, 1);
  ipe.box(-0.1, -0.1, 1.1, 1.1);
  ipe.label("$\\sigma = " + std::to_string(sigma) + "$", 0.5, 1.05);
}

inline double distance(const std::vector<double>& a,
                       const std::vector<double>& b) {
  auto result = 0.0;
  for (auto d = 0u; d < a.size(); ++d) {
    auto dist = std::abs(a[d] - b[d]);
    dist = std::min(dist, 1.0 - dist);
    result = std::max(result, dist);
  }
  return result;
}

inline void scale_weights(std::vector<double>& weights, double factor) {
  for (auto& w : weights) {
    w *= factor;
  }
}

inline std::vector<Edge> agirg(unsigned n, double ple, double deg, unsigned dim,
                               double T, double sigma,
                               double weight_correction_factor = 1.0,
                               std::string ipe_output = "") {
  // alpha = inverse temperature
  double alpha = T > 0 ? 1 / T : std::numeric_limits<double>::infinity();

  // We want to use the GIRG generator as black box to generate
  // AGIRGs.  For this, we have to choose GIRG weights such that the
  // resulting GIRG is a supergraph of the desired AGIRG and then do
  // an additional coin flip for each edge correcting the probability.
  // See the latex notes for more details on how get feasible GIRG
  // weights.

  // Note: Important that we pass a copy of the wights here, as we
  // potentially scale them but we do not want to actually scale the
  // AGIRG weights.
  auto girg_supergraph_weights = [&n, &sigma](std::vector<double> weights_agirg,
                                              double W_agirg) {
    // nothing to do for sigma = 1
    if (sigma == 1) return weights_agirg;

    // copy weights and potentially scale them to have minimum weight >= 1
    double min_weight = weights_agirg[n - 1];
    if (min_weight < 1) {
      scale_weights(weights_agirg, 1 / min_weight);
      W_agirg /= min_weight;
    }

    // scaling with min_weight^{sigma - 1} should not decrease the
    // weights by too much
    if (sigma < 1) {
      min_weight = weights_agirg[n - 1];
      scale_weights(weights_agirg, std::pow(min_weight, sigma - 1));
      return weights_agirg;
    }

    // for σ > 1, first define auxiliary weights (w'' in the notes)
    std::vector<double> weights_girg(n);
    double W_agirg_pow = std::pow(W_agirg, 1 / (sigma + 1));
    for (unsigned v = 0; v < weights_agirg.size(); ++v) {
      weights_girg[v] =
          weights_agirg[v] *
          std::pow(std::min(W_agirg_pow, weights_agirg[v]), (sigma - 1) / 2);
    }

    // additionally scale by W'' / W
    double W_aux =
        std::accumulate(weights_girg.begin(), weights_girg.end(), 0.0);
    scale_weights(weights_girg, W_aux / W_agirg);

    return weights_girg;
  };

  // Start with decreasingly sorted power-law weights for the AGIRG.
  auto weights_agirg = power_law_weights(n, ple);
  std::sort(weights_agirg.begin(), weights_agirg.end(), std::greater<double>());

  // scale weights according to girg degree estimation
  girgs::scaleWeights(weights_agirg, deg, dim, alpha);
  std::vector<double> initial_weights = weights_agirg;

  // scale weights by a factor anticipating some weight change
  // depending on sigma and by the correction factor
  double min_weight = weights_agirg[n - 1];
  double heuristic_factor = std::pow(min_weight, 1 - sigma);
  // double heuristic_factor = 1.0;
  scale_weights(weights_agirg, heuristic_factor * weight_correction_factor);

  double W_agirg =
      std::accumulate(weights_agirg.begin(), weights_agirg.end(), 0.0);

  // generate the GIRG supergraph
  auto weights_girg = girg_supergraph_weights(weights_agirg, W_agirg);
  unsigned pseed = Random::natural_number();
  unsigned sseed = Random::natural_number();
  auto positions = girgs::generatePositions(n, dim, pseed);
  auto edges_girg = girgs::generateEdges(weights_girg, positions, alpha, sseed);

  // filter GIRG edges to get the AGIRG
  std::vector<Edge> edges_agirg;
  double W_girg =
      std::accumulate(weights_girg.begin(), weights_girg.end(), 0.0);
  for (auto e : edges_girg) {
    Node u = std::min(e.first, e.second);
    Node v = std::max(e.first, e.second);

    // weight part of the GIRG probability
    double w_part_girg = weights_girg[u] * weights_girg[v] / W_girg;

    // weight part of the AGIRG probability; note that u < v and thus w_u > w_v
    double w_part_agirg =
        std::pow(weights_agirg[v], sigma) *
        std::pow(weights_agirg[u], std::min(1.0, ple - sigma)) / W_agirg;

    if (w_part_girg < 1 && w_part_girg < w_part_agirg - 0.00001) {
      std::cout << "GIRG probability too small" << std::endl;
      std::cout << "w_girg[u] = " << weights_girg[u]
                << ", w_girg[v] = " << weights_girg[v]
                << ", W_girg = " << W_girg << std::endl;
      std::cout << "w_agirg[u] = " << weights_agirg[u]
                << ", w_agirg[v] = " << weights_agirg[v]
                << ", W_agirg = " << W_agirg << std::endl;
      std::cout << "w_part_girg: " << w_part_girg << std::endl;
      std::cout << "w_part_agirg: " << w_part_agirg << std::endl;
    }
    assert(w_part_girg >= 1 || w_part_girg >= w_part_agirg - 0.00001);

    // we also need the distance part of the probability (due to
    // probabilities being capped at 1)
    double dist = distance(positions[u], positions[v]);
    double dist_part = std::pow(dist, dim);

    // special handling for the temperature 0 case
    if (T == 0) {
      if (dist_part < w_part_agirg) {
        edges_agirg.push_back({u, v});
      }
      continue;
    }

    // correcting the connection probability
    double p_girg = std::min(1.0, std::pow(w_part_girg / dist_part, alpha));
    double p_agirg = std::min(1.0, std::pow(w_part_agirg / dist_part, alpha));
    if (Random::coin_flip(p_agirg / p_girg)) {
      edges_agirg.push_back({u, v});
    }
  }

  // std::cout << "edges in the GIRG: " << edges_girg.size() << std::endl;
  // std::cout << "edges in the AGIRG: " << edges_agirg.size() << std::endl;

  if (ipe_output != "")
    print_graph(edges_agirg, positions, initial_weights, ipe_output, sigma);

  return edges_agirg;
}

TEST_CASE("[GIRG] agirg correct graph") {
  auto check = [](unsigned n, double ple, double deg, unsigned dim, double T,
                  double sigma) {
    double alpha = T > 0 ? 1 / T : std::numeric_limits<double>::infinity();

    // generate graph with fixed seed
    Random::set_seed(1234);
    auto edges = agirg(n, ple, deg, dim, T, sigma);

    // positions with the same seed
    Random::set_seed(1234);
    unsigned pseed = Random::natural_number();
    auto positions = girgs::generatePositions(n, dim, pseed);

    // weights
    auto weights = power_law_weights(n, ple);
    std::sort(weights.begin(), weights.end(), std::greater<double>());
    girgs::scaleWeights(weights, deg, dim, alpha);
    double min_weight = weights[n - 1];
    double heuristic_factor = std::pow(min_weight, 1 - sigma);
    scale_weights(weights, heuristic_factor);
    double W = std::accumulate(weights.begin(), weights.end(), 0.0);

    // hash set of edges
    std::unordered_set<std::string> is_connected;
    auto key = [](Node u, Node v) {
      return std::to_string(u) + " -> " + std::to_string(v);
    };
    for (auto& e : edges) {
      is_connected.insert(key(e.s, e.t));
      is_connected.insert(key(e.t, e.s));
    }

    // check pairwise connections
    for (unsigned u = 0; u < n; ++u) {
      for (unsigned v = u + 1; v < n; ++v) {
        REQUIRE(weights[u] >= weights[v]);
        double w_u_part = std::pow(weights[u], std::min(1.0, ple - sigma));
        double w_v_part = std::pow(weights[v], sigma);
        double weights_part = w_u_part * w_v_part / W;
        double dist_part = std::pow(distance(positions[u], positions[v]), dim);

        bool connected = weights_part >= dist_part;
        std::string err_msg =
            connected ? "missing edge" : "edge should not be here";
        REQUIRE_MESSAGE(connected == is_connected.contains(key(u, v)), err_msg,
                        "\nparameters: τ = ", ple, " σ = ", sigma,
                        "\nweights_part = ", weights_part,
                        "\ndist_part = ", dist_part);
      }
    }
  };

  unsigned n = 100, dim = 2;
  double deg = 15, T = 0;

  std::vector<double> ples = {2.2, 2.5, 2.7, 3.0};
  std::vector<double> sigmas = {0.4, 0.7, 1.0, 1.3, 1.6};

  for (double ple : ples) {
    for (double sigma : sigmas) {
      check(n, ple, deg, dim, T, sigma);
    }
  }
}
