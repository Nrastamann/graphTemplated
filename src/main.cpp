#include "graph.hpp"
#include <array>
static constexpr size_t kNodeAmount{ 7 };
static constexpr size_t kNodeAmountSecond{ 8 };
static constexpr size_t kEdgesAmount{ 9 };
constexpr size_t kTabSignsAmount{ 8 };
using GraphFirst::operator<<;
int main()
{
	GraphFirst::AdjacencyMatrix<kNodeAmount> matrix_oriented;
	GraphFirst::ResizableAdjacencyMatrix<true, true, uint32_t>
		matrix_changable;
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

	GraphFirst::draw_name("Adjacency Matrix",
			      kNodeAmount * kTabSignsAmount + 2);
	std::cout << matrix_changable;

	GraphFirst::draw_name("Incidence Matrix",
			      kEdgesAmount * kTabSignsAmount + 2);
	std::cout << matrix_changable.getIncidenceMatrix();

	GraphFirst::draw_name("Degree Matrix",
			      kNodeAmount * kTabSignsAmount + 2);
	std::cout << matrix_changable.getDegreeMatrix();

	GraphFirst::draw_name("Reachability Matrix",
			      kNodeAmount * kTabSignsAmount + 2);
	std::cout << matrix_changable.getReachabilityMatrix();

	GraphFirst::draw_name("Distance Matrix",
			      kNodeAmount * kTabSignsAmount + 2);
	std::cout << matrix_changable.getDistanceMatrix();

	GraphFirst::draw_name("Kirchoff Matrix",
			      kNodeAmount * kTabSignsAmount + 2);
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

	GraphFirst::draw_name("Adjacency Matrix",
			      kNodeAmount * kTabSignsAmount + 2);
	std::cout << matrix_oriented;

	GraphFirst::draw_name("Incidence Matrix",
			      kEdgesAmount * kTabSignsAmount + 2);
	std::cout << matrix_oriented.getIncidenceMatrix();

	GraphFirst::draw_name("Degree Matrix",
			      kNodeAmount * kTabSignsAmount + 2);
	std::cout << matrix_oriented.getDegreeMatrix();

	GraphFirst::draw_name("Reachability Matrix",
			      kNodeAmount * kTabSignsAmount + 2);
	std::cout << matrix_oriented.getReachabilityMatrix();

	GraphFirst::draw_name("Distance Matrix",
			      kNodeAmount * kTabSignsAmount + 2);
	std::cout << matrix_oriented.getDistanceMatrix();

	GraphFirst::draw_name("Kirchoff Matrix",
			      kNodeAmount * kTabSignsAmount + 2);
	std::cout << matrix_oriented.getKirchoffMatrix();
}
