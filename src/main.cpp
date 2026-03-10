#include "graph.hpp"
#include "graph_generation.hpp"

static constexpr size_t kNodeAmount{7};
static constexpr size_t kNodeAmountSecond{8};
static constexpr size_t kEdgesAmount{9};
constexpr size_t kTabSignsAmount{8};
using GraphFirst::operator<<;
int
main()
{
  GraphFirst::AdjacencyMatrix<kNodeAmount> matrix_oriented;
  GraphFirst::ResizableAdjacencyMatrix<true, true, uint32_t> matrix_changable;
  matrix_changable.resize(kNodeAmount);

  matrix_changable.add_edge(0, 2, 12);
  matrix_changable.add_edge(0, 4, 19);
  matrix_changable.add_edge(0, 5, 15);
  matrix_changable.add_edge(0, 6, 7);

  matrix_changable.add_edge(1, 0, 21);

  matrix_changable.add_edge(2, 1, 4);

  matrix_changable.add_edge(3, 0, 25);
  matrix_changable.add_edge(3, 1, 23);

  matrix_changable.add_edge(6, 1, 42);

  matrix_changable.removeVertex(0);
  GraphFirst::draw_name("Adjacency Matrix", kNodeAmount * kTabSignsAmount + 2);
  std::cout << matrix_changable;
  GraphFirst::draw_name("Incidence Matrix", kEdgesAmount * kTabSignsAmount + 2);
  std::cout << matrix_changable.getIncidenceMatrix();

  GraphFirst::draw_name("Degree Matrix", kNodeAmount * kTabSignsAmount + 2);
  std::cout << matrix_changable.getDegreeMatrix();

  GraphFirst::draw_name("Reachability Matrix",
                        kNodeAmount * kTabSignsAmount + 2);
  std::cout << matrix_changable.getReachabilityMatrix();

  GraphFirst::draw_name("Distance Matrix", kNodeAmount * kTabSignsAmount + 2);
  std::cout << matrix_changable.getDistanceMatrix();

  GraphFirst::draw_name("Kirchoff Matrix", kNodeAmount * kTabSignsAmount + 2);
  std::cout << matrix_changable.getKirchoffMatrix();

  //============
  matrix_oriented.add_edge(0, 2, 12);
  matrix_oriented.add_edge(0, 4, 19);
  matrix_oriented.add_edge(0, 5, 15);
  matrix_oriented.add_edge(0, 6, 7);
  matrix_oriented.add_edge(1, 0, 21);

  matrix_oriented.add_edge(2, 1, 4);

  matrix_oriented.add_edge(3, 0, 25);
  matrix_oriented.add_edge(3, 1, 23);

  matrix_oriented.add_edge(6, 1, 42);

  GraphFirst::draw_name("Adjacency Matrix", kNodeAmount * kTabSignsAmount + 2);
  std::cout << matrix_oriented;

  GraphFirst::draw_name("Adjacency Matrix", kNodeAmount * kTabSignsAmount + 2);
  std::cout << matrix_oriented;

  GraphFirst::draw_name("Incidence Matrix", kEdgesAmount * kTabSignsAmount + 2);
  std::cout << matrix_oriented.getIncidenceMatrix();

  GraphFirst::draw_name("Degree Matrix", kNodeAmount * kTabSignsAmount + 2);
  std::cout << matrix_oriented.getDegreeMatrix();

  GraphFirst::draw_name("Reachability Matrix",
                        kNodeAmount * kTabSignsAmount + 2);
  std::cout << matrix_oriented.getReachabilityMatrix();

  GraphFirst::draw_name("Distance Matrix", kNodeAmount * kTabSignsAmount + 2);
  std::cout << matrix_oriented.getDistanceMatrix();

  GraphFirst::draw_name("Kirchoff Matrix", kNodeAmount * kTabSignsAmount + 2);
  std::cout << matrix_oriented.getKirchoffMatrix();

  //===
  GraphFirst::AdjacencyMatrix<kNodeAmountSecond, false> matrix_2_1;
  GraphFirst::AdjacencyMatrix<kNodeAmountSecond, false> matrix_2_2;
  GraphFirst::AdjacencyMatrix<4, false> matrix_2_3;

  matrix_2_3.add_edge(0, 1);
  matrix_2_3.add_edge(0, 2);
  matrix_2_3.add_edge(0, 3);
  matrix_2_3.add_edge(1, 2);

  matrix_2_1.add_edge(0, 1);
  matrix_2_1.add_edge(0, 2);
  matrix_2_1.add_edge(0, 5);
  matrix_2_1.add_edge(1, 2);
  matrix_2_1.add_edge(1, 3);
  matrix_2_1.add_edge(2, 2);
  matrix_2_1.add_edge(2, 4);
  matrix_2_1.add_edge(2, 5);
  matrix_2_1.add_edge(3, 5);
  matrix_2_1.add_edge(3, 6);
  matrix_2_1.add_edge(4, 6);
  matrix_2_1.add_edge(4, 7);
  matrix_2_1.add_edge(5, 7);
  matrix_2_1.add_edge(6, 7);
  GraphFirst::draw_name("First matrix", 4 * kTabSignsAmount + 2);
  std::cout << matrix_2_1;
  matrix_2_2.add_edge(0, 1);
  matrix_2_2.add_edge(0, 3);
  matrix_2_2.add_edge(0, 4);
  matrix_2_2.add_edge(0, 6);
  matrix_2_2.add_edge(1, 2);
  matrix_2_2.add_edge(1, 5);
  matrix_2_2.add_edge(1, 7);
  matrix_2_2.add_edge(2, 3);
  matrix_2_2.add_edge(2, 4);
  matrix_2_2.add_edge(2, 5);
  matrix_2_2.add_edge(3, 5);
  matrix_2_2.add_edge(3, 6);
  matrix_2_2.add_edge(3, 7);
  matrix_2_2.add_edge(4, 6);
  matrix_2_2.add_edge(4, 7);
  matrix_2_2.add_edge(5, 6);
  matrix_2_2.add_edge(6, 7);

  GraphFirst::draw_name("Second matrix",
                        kNodeAmountSecond * kTabSignsAmount + 2);
  std::cout << matrix_2_2;
  GraphFirst::draw_name("First pulled matrix",
                        (kNodeAmountSecond - 1) * kTabSignsAmount + 2);

  auto matrix_pulled = matrix_2_1.PullEdge(4, 2);
  std::cout << matrix_pulled;
}
