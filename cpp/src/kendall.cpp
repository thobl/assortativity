#include "kendall.hpp"

#include <functional>

Kendall::Kendall(std::vector<Point>& points, bool symmetric)
    : m_nr_points(points.size()), m_symmetric(symmetric) {
  // check if number of pairs is maybe too big: at most n² pairs; 4 bits of
  // buffer, just to be safe..
  if (2 * std::log2(m_nr_points) > std::log2(infty) - 4) {
    exit(1);
  }

  // sort lexicographically by (x, y)
  std::sort(points.begin(), points.end(), [](const Point& p1, const Point& p2) {
    return p1.x == p2.x ? p1.y < p2.y : p1.x < p2.x;
  });

  // helper function to count ties (assuming the points are sorted)
  auto count_ties =
      [&](std::function<bool(const Point&, const Point& p2)> equal) {
        sll ties = 0;
        sll beg = 0;
        sll len = 1;
        while (beg + len < points.size()) {
          if (!equal(points[beg + len - 1], points[beg + len])) {
            // new bucket -> count number of pairs in prev bucket
            ties += len * (len - 1) / 2;
            beg += len;
            len = 1;
            continue;
          }
          ++len;
        }
        ties += len * (len - 1) / 2;
        return ties;
      };

  // count ties for both and for x
  m_ties_both = count_ties([](const Point& p1, const Point& p2) {
    return p1.x == p2.x && p1.y == p2.y;
  });

  m_ties_x =
      count_ties([](const Point& p1, const Point& p2) { return p1.x == p2.x; });
  m_ties_x -= m_ties_both;

  // merge ranges [beg, (beg + end) / 2) and [(beg + end) / 2, end)
  auto merge = [&](unsigned beg, unsigned end) {
    unsigned mid = (beg + end) / 2;
    end = std::min<unsigned>(end, points.size());
    std::vector<Point> frst(points.begin() + beg, points.begin() + mid);
    std::vector<Point> scnd(points.begin() + mid, points.begin() + end);

    unsigned curr1 = 0;
    unsigned curr2 = 0;

    for (unsigned curr = beg; curr < end; ++curr) {
      if (curr2 == scnd.size() ||
          (curr1 < frst.size() && frst[curr1].y <= scnd[curr2].y)) {
        // can take first
        points[curr] = frst[curr1];
        curr1++;
      } else {
        // have to take second -> count new inversions
        points[curr] = scnd[curr2];
        curr2++;
        m_discordant += frst.size() - curr1;
      }
    }
  };

  // merge sort with respect to y and count inversions
  m_discordant = 0;
  for (unsigned len = 2; len / 2 < points.size(); len *= 2) {
    for (unsigned beg = 0; beg + len / 2 < points.size(); beg += len) {
      merge(beg, beg + len);
    }
  }

  // ties with respect to y
  m_ties_y =
      count_ties([](const Point& p1, const Point& p2) { return p1.y == p2.y; });
  m_ties_y -= m_ties_both;

  // concordant pairs
  m_concordant = m_nr_points * (m_nr_points - 1) / 2 - m_discordant - m_ties_x -
                 m_ties_y - m_ties_both;

  // additionally count asymmetric points
  m_asymmetric_points = 0;
  for (auto& p : points) {
    if (p.x != p.y) {
      m_asymmetric_points++;
    }
  }
}

sll Kendall::concordant() const { return m_concordant; }
sll Kendall::discordant() const {
  return m_symmetric ? m_discordant - m_asymmetric_points / 2 : m_discordant;
}
sll Kendall::ties_x() const { return m_ties_x; }
sll Kendall::ties_y() const { return m_ties_y; }
sll Kendall::ties_both() const {
  return m_symmetric ? m_ties_both - (m_nr_points - m_asymmetric_points) / 2
                     : m_ties_both;
}

double Kendall::tau_a() const {
  sll C = concordant();
  sll D = discordant();
  sll T1 = ties_x();
  sll T2 = ties_y();
  sll T = ties_both();
  return (double)(C - D) / (C + D + T1 + T2 + T);
}

double Kendall::tau_b() const {
  sll C = concordant();
  sll D = discordant();
  sll T1 = ties_x();
  sll T2 = ties_y();
  return (C - D) / std::sqrt((double)(C + D + T1) * (C + D + T2));
}

double Kendall::tau_simple() const {
  sll C = concordant();
  sll D = discordant();
  return (double)(C - D) / (C + D);
}
