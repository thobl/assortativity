#include "edge_list.hpp"

#include <sstream>

#include "types.hpp"

EdgeList::EdgeList(std::istream& input)
    : m_deg(3), m_neighbors(3), m_graph_type(UNDIRECTED) {
  std::string line;
  unsigned offset = 0;
  // check whether it starts with a comment specifying the graph type
  // according to the KONECT format
  if (input.peek() == '%') {
    offset = 1;
    std::getline(input, line);
    if (line.find(" bip") != std::string::npos) {
      m_graph_type = BIPARTITE;
    } else if (line.find(" asym") != std::string::npos) {
      m_graph_type = DIRECTED;
    }
  }
  // skipping comments
  while (input.peek() == '%') {
    std::getline(input, line);
  }

  // read the edges and increase the degrees
  while (std::getline(input, line)) {
    std::stringstream line_ss(line);
    Node s, t;
    line_ss >> s;
    line_ss >> t;
    s -= offset;
    t -= offset;
    deg_mut(OUT, s)++;
    deg_mut(IN, t)++;
    deg_mut(SUM, s)++;
    deg_mut(SUM, t)++;
    m_edges.push_back({s, t});
    neighbors(OUT, s).push_back(t);
    neighbors(IN, t).push_back(s);
    if (m_graph_type != BIPARTITE) {
      neighbors(SUM, s).push_back(t);
      neighbors(SUM, t).push_back(s);
    }
  }
}

void EdgeList::check_configuration(DegType source_type,
                                   DegType target_type) const {
  if (m_graph_type == BIPARTITE && (source_type != OUT || target_type != IN)) {
    // for bipartite graphs only the IN-OUT variant makes any sense
    exit(1);
  }
  if (m_graph_type == UNDIRECTED &&
      (source_type != SUM || target_type != SUM)) {
    // for undirected graphs only the SUM-SUM variant makes any sense
    exit(1);
  }
}

unsigned& EdgeList::deg_mut(DegType type, Node v) {
  if (m_deg[type].size() <= v) {
    m_deg[type].resize(v + 1, 0);
  }
  return m_deg[type][v];
}

std::vector<Node>& EdgeList::neighbors(DegType type, Node v) {
  if (m_neighbors[type].size() <= v) {
    m_neighbors[type].resize(v + 1);
  }
  return m_neighbors[type][v];
}

unsigned EdgeList::deg(DegType type, Node v) const { return m_deg[type][v]; }

const std::vector<Node>& EdgeList::neigbors(DegType type, Node v) const {
  return m_neighbors[type][v];
}

std::vector<Point> EdgeList::degree_points(
    DegType sdeg, DegType tdeg, bool undirected_both_orientations) const {
  check_configuration(sdeg, tdeg);

  std::vector<Point> res;
  for (const Edge& e : m_edges) {
    Point p{deg(sdeg, e.s), deg(tdeg, e.t)};
    res.push_back(p);
    if (m_graph_type == UNDIRECTED && undirected_both_orientations) {
      Point q = {p.y, p.x};
      res.push_back(q);
    }
  }
  return res;
}

const std::vector<Edge>& EdgeList::edges() const {
  return m_edges;
}

unsigned EdgeList::n() const {
  if (m_graph_type == BIPARTITE) {
    return n1() + n2();
  }
  return m_deg[SUM].size();
}

unsigned EdgeList::n1() const {
  return m_graph_type == BIPARTITE ? m_deg[OUT].size() : 0;
}

unsigned EdgeList::n2() const {
  return m_graph_type == BIPARTITE ? m_deg[IN].size() : 0;
}
