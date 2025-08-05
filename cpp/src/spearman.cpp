#include "spearman.hpp"

#include <algorithm>
#include <functional>
#include <numeric>

#include "pearson.hpp"

double spearman(const std::vector<Point>& points) {
  return pearson(ranks_times_two(points));
}

std::vector<Point> ranks_times_two(const std::vector<Point>& points) {
  unsigned n = points.size();

  auto ranks = [&](std::function<int(unsigned)> val) {
    std::vector<unsigned> permut(n);
    std::iota(permut.begin(), permut.end(), 0);
    std::sort(permut.begin(), permut.end(),
              [&](unsigned i, unsigned j) { return val(i) < val(j); });

    std::vector<unsigned> rank(n);
    auto fill_block_of_ties = [&](unsigned beg, unsigned end) {
      for (unsigned i = beg; i < end; ++i) {
        rank[permut[i]] = beg + end - 1;
      }
    };
    unsigned beg = 0;
    for (unsigned end = 1; end < n; ++end) {
      if (val(permut[beg]) == val(permut[end])) continue;
      fill_block_of_ties(beg, end);
      beg = end;
    }
    fill_block_of_ties(beg, n);

    return rank;
  };

  auto rank_x = ranks([&](unsigned i) { return points[i].x; });
  auto rank_y = ranks([&](unsigned i) { return points[i].y; });
  std::vector<Point> res(n);
  for (unsigned i = 0; i < n; ++i) {
    res[i] = Point{rank_x[i], rank_y[i]};
  }
  return res;  
}
