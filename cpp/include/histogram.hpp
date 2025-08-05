#pragma once

#include <ostream>
#include <vector>

#include "doctest.h"
#include "types.hpp"

// a histogram of natural number measurement
class Histogram {
 public:
  Histogram();

  // add a measurement to the histogram
  void add(unsigned measurement);

  // largest observed measurement
  unsigned maximum() const;

  // number of observations of given measurement
  unsigned frequency(unsigned measurement) const;

 private:
  unsigned m_max;
  std::vector<unsigned> m_frequency_by_measurement;
};

// create a sequence of breaks between buckets starting with min and
// ending with max such that each two consecutive breaks are the same
// constant factor apart
std::vector<double> log_breaks(double min, double max, unsigned nr_buckets);

// 2D histogram aggregating 2D data into buckets
class Histogram2D {
 public:
  Histogram2D(const std::vector<Point>& points, unsigned nr_buckets,
              bool complementary_cumulative = false);

  unsigned count(unsigned bucket_x, unsigned bucket_y) const {
    return m_histogram[bucket_x][bucket_y];
  };

  // lower bound, upper bound and center (logarithmic) of a bucket
  double lb_x(unsigned bucket) const { return m_breaks_x[bucket]; }
  double ub_x(unsigned bucket) const { return m_breaks_x[bucket + 1]; }
  double center_x(unsigned bucket) const { return m_bucket_center_x[bucket]; }

  double lb_y(unsigned bucket) const { return m_breaks_y[bucket]; }
  double ub_y(unsigned bucket) const { return m_breaks_y[bucket + 1]; }
  double center_y(unsigned bucket) const { return m_bucket_center_y[bucket]; }

 private:
  std::vector<std::vector<unsigned>> m_histogram;
  std::vector<double> m_breaks_x;
  std::vector<double> m_breaks_y;
  std::vector<double> m_bucket_center_x;
  std::vector<double> m_bucket_center_y;
};

TEST_CASE("[log breaks]") {
  // 4 buckets: 1-2, 2-4, 4-8, 8-16
  auto breaks = log_breaks(1.0, 16.0, 4);
  std::cout << std::endl;
  CHECK(breaks.size() == 5);
  CHECK(breaks[0] == 1.0);
  CHECK(breaks[4] == 16.0);
}
