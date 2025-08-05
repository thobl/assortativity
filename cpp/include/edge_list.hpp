#pragma once

#include <filesystem>
#include <fstream>
#include <istream>
#include <sstream>
#include <vector>

#include "doctest.h"
#include "types.hpp"

class EdgeList {
 public:
  EdgeList(std::istream& input);

  GraphType type() const { return m_graph_type; }

  // Returns a point for each edge with the specified combination of
  // degree types.  For UNDIRECTED, each edge yields two points (one
  // for each orientation of the edge), unless the last parameter is
  // set to false.
  std::vector<Point> degree_points(
      DegType source_type, DegType target_type,
      bool undirected_both_orientations = true) const;

  // number of vertices
  unsigned n() const;

  // number of vertices in the different partitions for bipartite graphs
  unsigned n1() const;
  unsigned n2() const;

  // number of edges
  unsigned m() const { return m_edges.size(); }

  // degree of a vertex
  unsigned deg(DegType type, Node v) const;

  // neighbors of a vertex
  const std::vector<Node>& neigbors(DegType type, Node v) const;

  // edges; note that for BIPARTITE, the same node id can be used for
  // vertices of different partitions
  const std::vector<Edge>& edges() const;

 private:
  void check_configuration(DegType source_type, DegType target_type) const;
  unsigned& deg_mut(DegType type, Node v);
  std::vector<Node>& neighbors(DegType type, Node v);

  GraphType m_graph_type;
  std::vector<Edge> m_edges;
  std::vector<std::vector<unsigned>> m_deg;
  std::vector<std::vector<std::vector<Node>>> m_neighbors;
};

TEST_CASE("[Edge List] Undirected Graphs") {
  // vertex degrees: 1: 4, 2-5: 2, 6-9: 1
  std::istringstream input_konect_format{
      "% sym unweighted\n"
      "%\n"
      "1 2 \n"
      "2 3 \n"
      "3 4 \n"
      "4 5 \n"
      "5 6 \n"
      "1 7 \n"
      "1 8 \n"
      "1 9 \n"};
  std::istringstream input_el_format{
      "0 1 \n"
      "1 2\n"
      "2 3\n"
      "3 4\n"
      "4 5 \n"
      "0 6 \n"
      "0 7\n"
      "0 8 \n"};

  auto check = [](std::istream& input) {
    EdgeList EL(input);
    auto points = EL.degree_points(SUM, SUM);

    CHECK(EL.type() == UNDIRECTED);

    CHECK(EL.n() == 9);
    CHECK(EL.m() == 8);

    CHECK(points.size() == 16);

    CHECK(points[0].x == 4);
    CHECK(points[0].y == 2);
    CHECK(points[1].x == 2);
    CHECK(points[1].y == 4);

    CHECK(points[2].x == 2);
    CHECK(points[2].y == 2);
    CHECK(points[3].x == 2);
    CHECK(points[3].y == 2);

    CHECK(points[8].x == 2);
    CHECK(points[8].y == 1);
    CHECK(points[9].x == 1);
    CHECK(points[9].y == 2);

    CHECK(points[10].x == 4);
    CHECK(points[10].y == 1);
    CHECK(points[11].x == 1);
    CHECK(points[11].y == 4);

    auto neighbors_of_0 = EL.neigbors(SUM, 0);
    CHECK(neighbors_of_0.size() == 4);
    CHECK(neighbors_of_0[0] == 1);
    CHECK(neighbors_of_0[1] == 6);
    CHECK(neighbors_of_0[2] == 7);
    CHECK(neighbors_of_0[3] == 8);
  };

  check(input_konect_format);
  check(input_el_format);
}

TEST_CASE("[Edge Lists] Directed Graphs") {
  std::istringstream input{
      "% asym unweighted\n"
      "1 2 \n"
      "1 4 \n"
      "2 3 \n"
      "3 2\n"
      "3 4\n"
      "3 5\n"
      "5 1 \n"};
  // node out in
  // 1    2   1
  // 2    1   2
  // 3    3   1
  // 4    0   2
  // 5    1   1

  EdgeList EL(input);
  CHECK(EL.type() == DIRECTED);
  CHECK(EL.n() == 5);
  CHECK(EL.m() == 7);

  SUBCASE("SUM-SUM variant") {
    auto points = EL.degree_points(SUM, SUM);

    CHECK(points.size() == 7);

    CHECK(points[0].x == 3);
    CHECK(points[0].y == 3);

    CHECK(points[1].x == 3);
    CHECK(points[1].y == 2);

    CHECK(points[2].x == 3);
    CHECK(points[2].y == 4);

    CHECK(points[3].x == 4);
    CHECK(points[3].y == 3);

    CHECK(points[4].x == 4);
    CHECK(points[4].y == 2);

    CHECK(points[5].x == 4);
    CHECK(points[5].y == 2);

    CHECK(points[6].x == 2);
    CHECK(points[6].y == 3);
  }

  SUBCASE("OUT-IN variant") {
    auto points = EL.degree_points(OUT, IN);

    CHECK(points.size() == 7);

    CHECK(points[0].x == 2);
    CHECK(points[0].y == 2);

    CHECK(points[1].x == 2);
    CHECK(points[1].y == 2);

    CHECK(points[2].x == 1);
    CHECK(points[2].y == 1);

    CHECK(points[3].x == 3);
    CHECK(points[3].y == 2);

    CHECK(points[4].x == 3);
    CHECK(points[4].y == 2);

    CHECK(points[5].x == 3);
    CHECK(points[5].y == 1);

    CHECK(points[6].x == 1);
    CHECK(points[6].y == 1);
  }

  SUBCASE("OUT-OUT variant") {
    auto points = EL.degree_points(OUT, OUT);

    CHECK(points.size() == 7);

    CHECK(points[0].x == 2);
    CHECK(points[0].y == 1);

    CHECK(points[1].x == 2);
    CHECK(points[1].y == 0);

    CHECK(points[2].x == 1);
    CHECK(points[2].y == 3);

    CHECK(points[3].x == 3);
    CHECK(points[3].y == 1);

    CHECK(points[4].x == 3);
    CHECK(points[4].y == 0);

    CHECK(points[5].x == 3);
    CHECK(points[5].y == 1);

    CHECK(points[6].x == 1);
    CHECK(points[6].y == 2);
  }

  SUBCASE("neighborhoods") {
    auto out_neighbors_of_0 = EL.neigbors(OUT, 0);
    auto in_neighbors_of_0 = EL.neigbors(IN, 0);
    auto sum_neighbors_of_0 = EL.neigbors(SUM, 0);

    CHECK(out_neighbors_of_0.size() == 2);
    CHECK(out_neighbors_of_0[0] == 1);
    CHECK(out_neighbors_of_0[1] == 3);

    CHECK(in_neighbors_of_0.size() == 1);
    CHECK(in_neighbors_of_0[0] == 4);

    CHECK(sum_neighbors_of_0.size() == 3);
    CHECK(sum_neighbors_of_0[0] == 1);
    CHECK(sum_neighbors_of_0[1] == 3);
    CHECK(sum_neighbors_of_0[2] == 4);
  }
}

TEST_CASE("[Edge Lists] Bipartite Graphs") {
  std::istringstream input{
      "% bip unweighted\n"
      "1 1 \n"
      "1 2 \n"
      "1 3 \n"
      "2 1 \n"
      "2 2 \n"};
  EdgeList EL(input);
  CHECK(EL.type() == BIPARTITE);
  CHECK(EL.n() == 5);
  CHECK(EL.n1() == 2);
  CHECK(EL.n2() == 3);
  CHECK(EL.m() == 5);

  SUBCASE("OUT-IN variant") {
    auto points = EL.degree_points(OUT, IN);

    CHECK(points.size() == 5);

    CHECK(points[0].x == 3);
    CHECK(points[0].y == 2);

    CHECK(points[1].x == 3);
    CHECK(points[1].y == 2);

    CHECK(points[2].x == 3);
    CHECK(points[2].y == 1);

    CHECK(points[3].x == 2);
    CHECK(points[3].y == 2);

    CHECK(points[4].x == 2);
    CHECK(points[4].y == 2);
  }

  SUBCASE("neighborhoods") {
    auto out_neighbors_of_0 = EL.neigbors(OUT, 0);
    auto in_neighbors_of_0 = EL.neigbors(IN, 0);

    CHECK(out_neighbors_of_0.size() == 3);
    CHECK(out_neighbors_of_0[0] == 0);
    CHECK(out_neighbors_of_0[1] == 1);
    CHECK(out_neighbors_of_0[2] == 2);

    CHECK(in_neighbors_of_0.size() == 2);
    CHECK(in_neighbors_of_0[0] == 0);
    CHECK(in_neighbors_of_0[1] == 1);
  }
}
