#pragma once

#include <vector>

#include "doctest.h"
#include "types.hpp"

double pearson(const std::vector<Point>& points);

TEST_CASE("[Pearson]") {
  // points generated and tested for correlation in R
  std::vector<Point> points{{85, 76}, {37, 100}, {21, 91}, {65, 6},  {35, 69},
                            {16, 55}, {69, 47},  {38, 51}, {39, 82}, {73, 15},
                            {18, 55}, {57, 32},  {15, 72}, {52, 17}, {87, 69},
                            {51, 20}, {40, 49},  {98, 54}, {30, 25}, {7, 75}};
  SUBCASE("[Pearson] Symmetric") {
    std::vector<Point> points_sym;
    for (const auto& p : points) {
      points_sym.push_back(p);
      points_sym.push_back({p.y, p.x});
    }
    CHECK(pearson(points_sym) == doctest::Approx(-0.3008414));
  }
  SUBCASE("[Pearson] Asymmetric") {
    CHECK(pearson(points) == doctest::Approx(-0.2903994));
  }
  SUBCASE("[Pearson] Large numbers") {
    for (auto& p : points) {
      p.x += 100000000;
      p.y += 100000000;
    }
    CHECK(pearson(points) == doctest::Approx(-0.2903994));
  }
}
