#include "types.hpp"

std::ostream& operator<<(std::ostream& os, const Point& p) {
  return os << "(" << p.x << ", " << p.y << ")";
  return os;
}

std::string name(GraphType type) {
  switch (type) {
    case UNDIRECTED:
      return "undirected";
    case DIRECTED:
      return "directed";
    case BIPARTITE:
      return "bipartite";
  }
  return "";
}

std::string name(DegType type) {
  switch (type) {
    case OUT:
      return "out";
    case IN:
      return "in";
    case SUM:
      return "sum";
  }
  return "";
}

void print_edges(const std::vector<Edge>& edges, std::ostream& out) {
  for (const auto& e : edges) {
    out << e.s << " " << e.t << "\n";
  }
}
