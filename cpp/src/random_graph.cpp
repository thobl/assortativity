#include "random_graph.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>
#include <numeric>
#include <string>
#include <unordered_set>
#include <vector>

#include "random.hpp"

std::vector<Edge> gilbert(unsigned n, double p) {
  std::vector<Edge> edges;

  auto next_pair = [](Node u, Node v, unsigned skip) {
    while (skip + v >= u) {
      skip -= u - v;
      u++;
      v = 0;
    }
    v += skip;
    return Edge{u, v};
  };

  auto edge = next_pair(1, 0, Random::geometric_skip(p));
  auto [u, v] = edge;
  while (u < n) {
    edges.push_back(edge);
    edge = next_pair(u, v, 1 + Random::geometric_skip(p));
    u = edge.s;
    v = edge.t;
  }
  return edges;
}

std::vector<Edge> erdos_renyi(unsigned n, unsigned m) {
  std::vector<Edge> edges;
  std::unordered_set<std::string> edges_seen;
  auto edge_name = [](Node u, Node v) {
    return std::to_string(std::min(u, v)) + "-" +
           std::to_string(std::max(u, v));
  };
  for (unsigned i = 0; i < m; ++i) {
    Node u, v;
    do {  // rejection sampling
      u = Random::natural_number(n - 1);
      v = Random::natural_number(n - 1);
    } while (u == v || edges_seen.count(edge_name(u, v)));
    edges.push_back(Edge{u, v});
    edges_seen.insert(edge_name(u, v));
  }
  return edges;
}

std::vector<Edge> chung_lu(unsigned n, double ple, double avg_deg, double sigma) {
  // decreasingly sorted power-law weights
  auto weights = power_law_weights(n, ple);
  std::sort(weights.begin(), weights.end(), std::greater<double>());

  // sum of all weights
  double weight_sum = std::accumulate(weights.begin(), weights.end(), 0.0);

  // constant factor increasing the connection probability for
  // adjusting the average degree; initial value is a rough
  // estimation: for σ=1 it yields the correct average degree in the
  // mulit-graph model (i.e., when allowing "probabilities" greater 1)
  double deg_correction = n * avg_deg / weight_sum;
  
  // compute expected number of edges for the weights and the given
  // degree correction factor
  auto expected_nr_edges = [&]() {
    double sum_of_smaller_non_min = 0.0;
    unsigned num_nodes_with_min = 0;
    double exp_nr_edges = 0.0;
    for (Node u = n - 1; u + 1 > 0; --u) {
      // normalized weight of u: w_u^min(1, τ - σ) / W
      double w_u_normal = deg_correction *
                          std::pow(weights[u], std::min(1.0, ple - sigma)) /
                          weight_sum;

      // sum of all smaller weights to the power of σ
      double w_v_sigma = std::pow(weights[u], sigma);
      if (w_u_normal * w_v_sigma <= 1.0) {
        // not yet in the regime where probabilities are > 1
        sum_of_smaller_non_min += w_v_sigma;
      } else {
        // in the regime where probabilities are > 1 -> correct the sum
        num_nodes_with_min++;
        for (Node v = u + num_nodes_with_min; v < n; ++v) {
          double w_v_sigma = std::pow(weights[v], sigma);
          if (w_u_normal * w_v_sigma <= 1.0) {
            break;
          }
          num_nodes_with_min++;
          sum_of_smaller_non_min -= w_v_sigma;
        }
      }

      // add part to the sum where the minimum applies
      double sum_of_smaller =
          sum_of_smaller_non_min + num_nodes_with_min / w_u_normal;

      // update expected number of edges
      exp_nr_edges += w_u_normal * sum_of_smaller;
    }
    return exp_nr_edges;
  };
  
  // adjust the degree correction factor to yield a better estimate
  // for the expected average degree; this can be repeated to yield
  // slightly better results (more than twice seems unnecessary)
  deg_correction *= 0.5 * avg_deg * n / expected_nr_edges();
  deg_correction *= 0.5 * avg_deg * n / expected_nr_edges();
  
  // connection probability
  auto p = [&](Node u, Node v) {
    // we only call this for u < v and thus weights(u) > weights(v)
    assert(weights[v] <= weights[u]);
    double w_min = std::pow(weights[v], sigma);
    double w_max = std::pow(weights[u], std::min(1.0, ple - sigma));
    return std::min(deg_correction * w_min * w_max / weight_sum, 1.0);
  };

  // generate
  std::vector<Edge> edges;

  for (Node u = 0; u < n - 1; ++u) {
    Node v = u;
    while (true) {
      // skip some vertices using an upper bound on the connection probability
      v++;
      double p_uv_upper = p(u, v);
      v += Random::geometric_skip(p_uv_upper);
      if (v >= n) {
        break;
      }

      // correct for the error made by using only an upper bound
      if (Random::coin_flip(p(u, v) / p_uv_upper)) {
        edges.push_back(Edge{u, v});
      }
    }
  }
  return edges;
}

std::vector<double> power_law_weights(unsigned n, double ple) {
  std::vector<double> weights(n);
  double exponent = -1 / (ple - 1);
  for (unsigned i = 0; i < n; ++i) {
    weights[i] = std::pow(i + 1, exponent);
  }
  return weights;
}
