#include "pearson.hpp"

#include <cmath>
#include <functional>
#include <numeric>

double pearson(const std::vector<Point>& points) {
  // https://www.johndcook.com/blog/2008/11/05/how-to-calculate-pearson-correlation-accurately/
  auto sum_points = [&](std::function<double(const Point&)> f) {
    return std::accumulate(
        points.begin(), points.end(), 0.0,
        [&](double sum, const Point& p) { return sum + f(p); });
  };

  unsigned n = points.size();

  double x_avg = sum_points([&](const Point& p) { return (double)p.x / n; });

  double y_avg = sum_points([&](const Point& p) { return (double)p.y / n; });

  double s_x = std::sqrt(sum_points([&](const Point& p) {
    return (double)std::pow(p.x - x_avg, 2) / (n - 1);
  }));

  double s_y = std::sqrt(sum_points([&](const Point& p) {
    return (double)std::pow(p.y - y_avg, 2) / (n - 1);
  }));

  double r = sum_points([&](const Point& p) {
               return ((p.x - x_avg) / s_x) * ((p.y - y_avg) / s_y);
             }) /
             (n - 1);
  return r;
}
