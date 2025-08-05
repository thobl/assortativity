#include "histogram.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <vector>

Histogram::Histogram() : m_max(0), m_frequency_by_measurement(1, 0) {}

void Histogram::add(unsigned measurement) {
  if (measurement > m_max) {
    m_max = measurement;
    m_frequency_by_measurement.resize(m_max + 1, 0);
  }
  m_frequency_by_measurement[measurement]++;
}

unsigned Histogram::maximum() const { return m_max; }

unsigned Histogram::frequency(unsigned measurement) const {
  return measurement > m_max ? 0 : m_frequency_by_measurement[measurement];
}

////////////////////////////////////////////////////////////////////////////////

std::vector<double> log_breaks(double min, double max, unsigned nr_buckets) {
  std::vector<double> breaks;
  double base = std::pow((max / min), 1.0 / nr_buckets);
  double curr_break = 1.0;
  while (curr_break < max + eps) {
    breaks.push_back(curr_break);
    curr_break *= base;
  }
  return breaks;
}

Histogram2D::Histogram2D(const std::vector<Point>& points, unsigned nr_buckets,
                         bool complementary_cumulative) {
  // creating the breaks
  unsigned max_x =
      std::max_element(points.begin(), points.end(), [](auto& p1, auto& p2) {
        return p1.x < p2.x;
      })->x;

  unsigned max_y =
      std::max_element(points.begin(), points.end(), [](auto& p1, auto& p2) {
        return p1.y < p2.y;
      })->y;

  m_breaks_x = log_breaks(1, max_x + 1, nr_buckets);
  m_breaks_y = log_breaks(1, max_y + 1, nr_buckets);
  
  // precompute bucket centers
  m_bucket_center_x.resize(nr_buckets);
  m_bucket_center_y.resize(nr_buckets);
  for (unsigned bucket = 0; bucket < nr_buckets; ++bucket) {
    m_bucket_center_x[bucket] = std::sqrt(lb_x(bucket) * ub_x(bucket));
    m_bucket_center_y[bucket] = std::sqrt(lb_y(bucket) * ub_y(bucket));
  }

  // std::cout << "max: " << max_x << " " << max_y << std::endl;
  // std::cout << "breaks x" << std::endl;
  // for (auto b : m_breaks_x) {
  //   std::cout << b << "\n";
  // }
  // std::cout << "breaks y" << std::endl;
  // for (auto b : m_breaks_y) {
  //   std::cout << b << "\n";
  // }

  // assigning each point to the correct cell
  m_histogram.resize(nr_buckets);
  for (auto& col : m_histogram) {
    col.resize(nr_buckets, 0);
  }

  auto index = [](std::vector<double> vec, unsigned val) {
    auto ub = std::upper_bound(vec.begin(), vec.end(), val);
    return std::distance(vec.begin(), ub) - 1;
  };

  for (const auto& p : points) {
    unsigned cell_x = index(m_breaks_x, p.x);
    unsigned cell_y = index(m_breaks_y, p.y);
    m_histogram[cell_x][cell_y]++;
  }

  // auto print_histogram = [&]() {
  //   std::cout << "y\\x\t" << std::fixed << std::setprecision(2);
  //   for (unsigned x = 0; x < nr_buckets; ++x) {
  //     std::cout << m_breaks_x[x] << "\t";
  //   }
  //   std::cout << "\n";
  //   for (int y = nr_buckets - 1; y >= 0; --y) {
  //     std::cout << m_breaks_y[y] << ":\t";
  //     for (unsigned x = 0; x < nr_buckets; ++x) {
  //       std::cout << m_histogram[x][y] << "\t";
  //     }
  //     std::cout << "\n";
  //   }
  // };

  // print_histogram();

  if (!complementary_cumulative) return;

  // making it cumulative
  std::vector<std::vector<bool>> done(nr_buckets,
                                      std::vector<bool>(nr_buckets, false));

  std::function<unsigned(unsigned, unsigned)> cum_val_rec =
      [&](unsigned x, unsigned y) -> unsigned {
    if (x >= nr_buckets || y >= nr_buckets) {
      return 0;
    } else if (!done[x][y]) {
      m_histogram[x][y] += cum_val_rec(x + 1, y) + cum_val_rec(x, y + 1) -
                           cum_val_rec(x + 1, y + 1);
      done[x][y] = true;
    }
    return m_histogram[x][y];
  };

  cum_val_rec(0, 0);
  // std::cout << std::endl;
  // print_histogram();
}
