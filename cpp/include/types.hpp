#pragma once

#include <limits>
#include <iostream>
#include <vector>

typedef signed long long sll;
constexpr sll infty = std::numeric_limits<sll>::max();
constexpr double eps = 0.00001;

struct Point {
  unsigned x;
  unsigned y;
};

std::ostream& operator<<(std::ostream& os, const Point& p);

enum GraphType { UNDIRECTED, DIRECTED, BIPARTITE };
std::string name(GraphType type);
enum DegType { OUT, IN, SUM };
std::string name(DegType type);

typedef unsigned Node;

struct Edge {
  Node s;
  Node t;
};

void print_edges(const std::vector<Edge>& edges, std::ostream& out);

template <typename T>
void print_csv_line_rec(std::ostream& out, const T& val) {
  out << val;
}

template <typename T, typename... Args>
void print_csv_line_rec(std::ostream& out, const T& first,
                        const Args&... rest) {
  out << first << ",";
  print_csv_line_rec(out, rest...);
}

template <typename... Args>
void print_csv_line(std::ostream& out, const Args&... args) {
  print_csv_line_rec(out, args...);
  out << "\n";
}
