#pragma once

#include <algorithm>
#include <cmath>
#include <vector>

#include "doctest.h"
#include "types.hpp"

class Kendall {
 public:
  // If symmetric is true, it is assumed that the points come in
  // symmetric pairs, i.e., every point (x, y) has (y, x) as a
  // counterpart, and that a point should not be compared with its
  // counterpart.
  Kendall(std::vector<Point>& points, bool symmetric = false);

  sll concordant() const;
  sll discordant() const;
  sll ties_x() const;
  sll ties_y() const;
  sll ties_both() const;

  // https://en.wikipedia.org/wiki/Kendall_rank_correlation_coefficient#Tau-a
  double tau_a() const;

  // https://online.stat.psu.edu/stat509/lesson/18/18.3
  double tau_b() const;

  // as tau_b but without the ties in the denominator
  double tau_simple() const;

 private:
  sll m_concordant;
  sll m_discordant;
  sll m_ties_x;
  sll m_ties_y;
  sll m_ties_both;
  sll m_asymmetric_points;
  sll m_nr_points;

  bool m_symmetric;
};

TEST_CASE("[Kendall]") {
  sll concordant = 0;
  sll discordant = 0;
  sll ties_x = 0;
  sll ties_y = 0;
  sll ties_both = 0;
  auto count_pair = [&](const Point& p, const Point& q) {
    if (p.x == q.x && p.y == q.y) {
      ties_both++;
    } else if (p.x == q.x) {
      ties_x++;
    } else if (p.y == q.y) {
      ties_y++;
    } else if ((p.x < q.x) == (p.y < q.y)) {
      concordant++;
    } else {
      discordant++;
    }
  };

  SUBCASE("[Kendall] Symmetric") {
    auto check = [&](const std::vector<Point>& points) {
      std::vector<Point> points_sym;
      for (const auto& p : points) {
        points_sym.push_back(p);
        points_sym.push_back({p.y, p.x});
      }

      Kendall K(points_sym, true);
      for (unsigned i = 0; i < points.size(); ++i) {
        for (unsigned j = i + 1; j < points.size(); ++j) {
          Point p1 = points[i];
          Point q1 = points[j];
          Point p2{p1.y, p1.x};
          Point q2{q1.y, q1.x};
          count_pair(p1, q1);
          count_pair(p1, q2);
          count_pair(p2, q1);
          count_pair(p2, q2);
        }
      }

      CHECK(K.concordant() == concordant);
      CHECK(K.discordant() == discordant);
      CHECK(K.ties_x() == ties_x);
      CHECK(K.ties_y() == ties_y);
      CHECK(K.ties_both() == ties_both);
    };

    std::vector<Point> points{{1, 3}, {2, 2}, {4, 4}, {2, 4}, {0, 4}};
    check(points);
  }

  SUBCASE("[Kendall] Asymmetric") {
    auto check = [&](std::vector<Point>& points) {
      Kendall K(points, false);
      for (unsigned i = 0; i < points.size(); ++i) {
        for (unsigned j = i + 1; j < points.size(); ++j) {
          count_pair(points[i], points[j]);
        }
      }

      CHECK(K.concordant() == concordant);
      CHECK(K.discordant() == discordant);
      CHECK(K.ties_x() == ties_x);
      CHECK(K.ties_y() == ties_y);
      CHECK(K.ties_both() == ties_both);
    };

    std::vector<Point> points{{1, 3}, {2, 2}, {4, 4}, {2, 4}, {0, 4}, {0, 4}};
    check(points);
  }
}

