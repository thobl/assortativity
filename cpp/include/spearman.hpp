#pragma once

#include "doctest.h"
#include "types.hpp"

double spearman(const std::vector<Point>& points);

std::vector<Point> ranks_times_two(const std::vector<Point>& points);

TEST_CASE("[Spearman] Ranks") {
  std::vector<Point> points{{0, 6}, {0, 6}, {2, 5}, {2, 2},
                            {2, 2}, {5, 2}, {6, 0}, {6, 0}};
  auto ranks = ranks_times_two(points);
  
  // 0, 1 -> rank 0.5
  CHECK(ranks[0].x == 1);
  CHECK(ranks[1].x == 1);

  // 2, 3, 4 -> rank 3
  CHECK(ranks[2].x == 6);
  CHECK(ranks[3].x == 6);
  CHECK(ranks[4].x == 6);

  // 5 -> rank 5
  CHECK(ranks[5].x == 10);

  // 6, 7 -> rank 6.5
  CHECK(ranks[6].x == 13);
  CHECK(ranks[7].x == 13);

  // opposite for y
  CHECK(ranks[0].y == 13);
  CHECK(ranks[1].y == 13);

  CHECK(ranks[2].y == 10);
  
  CHECK(ranks[3].y == 6);
  CHECK(ranks[4].y == 6);
  CHECK(ranks[5].y == 6);

  CHECK(ranks[6].y == 1);
  CHECK(ranks[7].y == 1);
}

TEST_CASE("[Spearman]") {
  //points generated and tested for correlation in R
  std::vector<Point> points{{46, 57}, {5, 75},  {47, 74}, {54, 21}, {79, 76},
                            {17, 48}, {10, 43}, {21, 49}, {23, 88}, {10, 86},
                            {14, 69}, {87, 17}, {62, 24}, {47, 49}, {96, 51},
                            {16, 13}, {94, 30}, {58, 51}, {77, 71}, {89, 61}};
  CHECK(spearman(points) == doctest::Approx(-0.1856175));
}
