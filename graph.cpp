#include <array>
#include "graph.hpp"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <ostream>

static size_t interest = 0;
static constexpr size_t kNodeAmount{ 7 };
static constexpr size_t kNodeAmountSecond{ 8 };
static constexpr size_t kEdgesAmount{ 9 };
namespace GraphFirst
{
template <size_t NodeAmount, bool isOriented, bool isWeighted,
	  typename value_type>
std::ostream &operator<<(
	std::ostream &out,
	const typename AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
				       value_type>::AdjacencyMatrixSelf &matrix);

template <size_t NodeAmount, bool isOriented, bool isWeighted,
	  typename value_type>
void AdjacencyMatrix<NodeAmount, isOriented, isWeighted, value_type>::add_edge(
	size_t firstNode, size_t secondNode, value_type value)
{
	matrix[firstNode][secondNode] = value;
	if constexpr (!isOriented) {
		matrix[secondNode][firstNode] = value;
	}
}

template <size_t NodeAmount, bool isOriented, bool isWeighted,
	  typename value_type>
void AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
		     value_type>::remove_edge(size_t firstNode,
					      size_t secondNode)
{
	matrix[firstNode][secondNode] = 0;
	if constexpr (!isOriented) {
		matrix[secondNode][firstNode] = 0;
	}
}

template <size_t NodeAmount, bool isOriented, bool isWeighted,
	  typename value_type>
void AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
		     value_type>::clear_matrix()
{
	size_t iSize = matrix.size();
	size_t jSize = matrix[0].size();

	for (size_t i = 0; i < iSize; ++i) {
		for (size_t j = 0; j < jSize; ++j) {
			matrix[i][j] = 0;
		}
	}
}

template <size_t NodeAmount, bool isOriented, bool isWeighted,
	  typename value_type>
void AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
		     value_type>::connect_all(value_type value)
{
	size_t iSize = matrix.size();
	size_t jSize = matrix[0].size();

	for (size_t i = 0; i < iSize; ++i) {
		for (size_t j = 0; j < jSize; ++j) {
			matrix[i][j] = value;
		}
	}
}

template <size_t NodeAmount, bool isOriented, bool isWeighted,
	  typename value_type>
typename AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
			 value_type>::IncidenceMatrix
AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
		value_type>::getIncidenceMatrix()
{
	size_t iSize = matrix.size();
	size_t jSize = matrix[0].size();

	IncidenceMatrix result_matrix;
	for (size_t i = 0; i < iSize; ++i) {
		for (size_t j = 0; j < jSize; ++j) {
			result_matrix[i][j] =
				matrix[i][j] == 0 && matrix[j][i] != 0 ?
					-matrix[j][i] :
					matrix[i][j];
		}
	}

	return result_matrix;
}

template <size_t NodeAmount, bool isOriented, bool isWeighted,
	  typename value_type>
typename AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
			 value_type>::DegreeMatrix
AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
		value_type>::getDegreeMatrix()
{
	size_t iSize = matrix.size();
	size_t jSize = matrix[0].size();

	DegreeMatrix result_matrix = { 0 };
	for (size_t i = 0; i < iSize; ++i) {
		for (size_t j = 0; j < jSize; ++j) {
			if (matrix[i][j] != 0 || matrix[j][i] != 0) {
				result_matrix[i][i] +=
					std::max(matrix[i][j], matrix[j][i]);
			}
		}
	}

	return result_matrix;
}

template <size_t NodeAmount, bool isOriented, bool isWeighted,
	  typename value_type>
typename AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
			 value_type>::ReachabilityMatrix
AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
		value_type>::getReachabilityMatrix()
{
	size_t iSize = matrix.size();
	size_t jSize = matrix[0].size();

	ReachabilityMatrix result_matrix = { 0 };
	for (size_t i = 0; i < iSize; ++i) {
		for (size_t j = 0; j < jSize; ++j) {
			result_matrix[i][j] = i == j ? 1 : isReachable(i, j);
		}
	}

	return result_matrix;
}

template <size_t NodeAmount, bool isOriented, bool isWeighted,
	  typename value_type>
typename AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
			 value_type>::DistanceMatrix
AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
		value_type>::getDistanceMatrix()
{
	size_t iSize = matrix.size();
	size_t jSize = matrix[0].size();

	DistanceMatrix result_matrix = { 0 };
	for (size_t i = 0; i < iSize; ++i) {
		for (size_t j = 0; j < jSize; ++j) {
			result_matrix[i][j] = i == j ? 0 : djkstra(i, j);
		}
	}

	return result_matrix;
}

template <size_t NodeAmount, bool isOriented, bool isWeighted,
	  typename value_type>
typename AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
			 value_type>::KirchoffMatrix
AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
		value_type>::getKirchoffMatrix()
{
	size_t iSize = matrix.size();
	size_t jSize = matrix[0].size();

	KirchoffMatrix result_matrix = { 0 };
	for (size_t i = 0; i < iSize; ++i) {
		for (size_t j = 0; j < jSize; ++j) {
			if (matrix[i][j] != 0 || matrix[j][i] != 0) {
				result_matrix[i][i] +=
					std::max(matrix[i][j], matrix[j][i]);
			}

			result_matrix[i][j] =
				matrix[i][j] == 0 && matrix[j][i] != 0 ?
					-matrix[j][i] :
					-matrix[i][j];
		}
	}

	return result_matrix;
}

template <size_t NodeAmount, bool isOriented, bool isWeighted,
	  typename value_type>
typename AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
			 value_type>::AdjacencyMatrixSelf
AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
		value_type>::GraphIntersection(const IntersectionMatrixRight
						       &matrix_second,
					       size_t removed_vertex)
{
	AdjacencyMatrixSelf result;

	auto &working_matrix = result.get_matrix();
	auto &working_matrix_second = matrix_second.cget_matrix();
	size_t iSmall = 0;
	size_t jSmall = 0;

	for (size_t i = 0; i < NodeAmount + 1; ++i) {
		if (i == removed_vertex)
			continue;
		for (size_t j = 0; j < NodeAmount + 1; ++j) {
			if (j == removed_vertex)
				continue;

			if (matrix[iSmall][jSmall] ==
				    working_matrix_second[i][j] &&
			    matrix[iSmall][jSmall] == 1) {
				working_matrix[iSmall][jSmall] = 1;
				working_matrix[jSmall][iSmall] = 1;
			}
			jSmall++;
		}
		jSmall = 0;
		iSmall++;
	}
	return result;
}

template <size_t NodeAmount, bool isOriented, bool isWeighted,
	  typename value_type>
AdjacencyMatrix<NodeAmount - 1, isOriented, isWeighted, value_type>
AdjacencyMatrix<NodeAmount, isOriented, isWeighted, value_type>::PullEdge(
	size_t first_node, size_t node_to_pull)
{
	AdjacencyMatrix<NodeAmount - 1, isOriented, isWeighted, value_type>
		result_matrix;
	auto &working_matrix = result_matrix.get_matrix();

	size_t iSmall = 0;
	size_t jSmall = 0;
	for (size_t i = 0; i < NodeAmount; ++i) {
		if (i == node_to_pull)
			continue;
		for (size_t j = 0; j < NodeAmount; ++j) {
			if (j == node_to_pull) {
				continue;
			}
			working_matrix[iSmall][jSmall++] = matrix[i][j];
		}
		iSmall++;
		jSmall = 0;
	}

	iSmall = 0;
	for (size_t i = 0; i < NodeAmount; ++i) {
		if (i == node_to_pull)
			continue;
		if (matrix[node_to_pull][i] == 1) {
			working_matrix[first_node][iSmall] = 1;
			working_matrix[iSmall][first_node] = 1;
		}
		++iSmall;
	}

	return result_matrix;
}

template <size_t NodeAmount, bool isOriented, bool isWeighted,
	  typename value_type>
value_type
AdjacencyMatrix<NodeAmount, isOriented, isWeighted, value_type>::djkstra(
	size_t first_node_index, size_t second_node_index)
{
	if (first_node_index == second_node_index) {
		return 0;
	}
	VisitedMatrix visited = { 0 };
	visited[first_node_index] = true;

	WeightsMatrix weights;
	for (auto &i : weights) {
		i = node_value_max;
	}
	weights[first_node_index] = 0;

	size_t current_node = first_node_index;

	while (current_node != second_node_index) {
		for (size_t i = 0; i < weights.size(); ++i) {
			if (visited[i] || matrix[current_node][i] == 0)
				continue;
			value_type new_value =
				weights[current_node] + matrix[current_node][i];

			weights[i] = std::min(weights[i], new_value);
		}
		int64_t min_weight_index = -1;
		value_type min_value = node_value_max;

		for (size_t i = 0; i < weights.size(); ++i) {
			if (min_value > weights[i] && !visited[i]) {
				min_value = weights[i];
				min_weight_index = i;
			}
		}

		if (min_value == node_value_max) {
			return node_value_max;
		}

		current_node = min_weight_index;
		visited[current_node] = true;
	}
	return weights[second_node_index];
}
template <size_t NodeAmount, bool isOriented, bool isWeighted,
	  typename value_type>
bool AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
		     value_type>::isReachable(size_t first_node,
					      size_t second_node)
{
	return djkstra(first_node, second_node) != node_value_max;
}
};

template <typename ContainerInside, size_t NodeAmount>
std::ostream &operator<<(std::ostream &out,
			 const std::array<ContainerInside, NodeAmount> &matrix)
{
	size_t iSize = matrix.size();
	size_t jSize = matrix[0].size();
	out << "\t|";
	for (size_t i = 0; i < jSize; ++i) {
		out << i + 1 << '\t';
	}

	out << '\n' << "\t|";
	for (size_t i = 0; i < jSize; ++i) {
		out << "=======";
	}

	out << '\n';

	for (size_t i = 0; i < iSize; ++i) {
		out << i + 1 << "\t|";
		for (size_t j = 0; j < jSize; ++j) {
			out << matrix[i][j] << '\t';
		}
		out << '\n';
	}
	return out;
}

template <size_t NodeAmount, bool isOriented, bool isWeighted,
	  typename value_type>
std::ostream &
operator<<(std::ostream &out,
	   const GraphFirst::AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
					     value_type> &matrix)
{
	size_t iSize = matrix.matrix.size();
	size_t jSize = matrix.matrix[0].size();
	out << "\t|";
	for (size_t i = 0; i < iSize; ++i) {
		out << i + 1 << '\t';
	}

	out << '\n' << "\t|";
	for (size_t i = 0; i < iSize; ++i) {
		out << "=======";
	}

	out << '\n';

	for (size_t i = 0; i < iSize; ++i) {
		out << i + 1 << "\t|";
		for (size_t j = 0; j < jSize; ++j) {
			out << matrix.matrix[i][j] << '\t';
		}
		out << '\n';
	}
	return out;
};
constexpr size_t kTabSignsAmount{ 8 };
void draw_name(std::string_view str, size_t value)
{
	std::string final_str(value, '=');

	auto it = final_str.begin() + (final_str.length() - str.length()) / 2;
	for (auto i = str.begin(); i != str.end(); ++i) {
		*it++ = *i;
	}
	std::cout << final_str << '\n';
}

int main()
{
	GraphFirst::AdjacencyMatrix<kNodeAmount, true, false> matrix_oriented;

	matrix_oriented.add_edge(0, 2, 12);
	matrix_oriented.add_edge(0, 4, 19);
	matrix_oriented.add_edge(0, 5, 15);
	matrix_oriented.add_edge(0, 6, 7);

	matrix_oriented.add_edge(1, 0, 21);

	matrix_oriented.add_edge(2, 1, 4);

	matrix_oriented.add_edge(3, 0, 25);
	matrix_oriented.add_edge(3, 1, 23);

	matrix_oriented.add_edge(6, 1, 42);

	draw_name("Adjacency Matrix", kNodeAmount * kTabSignsAmount + 2);
	std::cout << matrix_oriented;

	draw_name("Incidence Matrix", kEdgesAmount * kTabSignsAmount + 2);
	std::cout << matrix_oriented.getIncidenceMatrix();

	draw_name("Degree Matrix", kNodeAmount * kTabSignsAmount + 2);
	std::cout << matrix_oriented.getDegreeMatrix();

	draw_name("Reachability Matrix", kNodeAmount * kTabSignsAmount + 2);
	std::cout << matrix_oriented.getReachabilityMatrix();

	draw_name("Distance Matrix", kNodeAmount * kTabSignsAmount + 2);
	std::cout << matrix_oriented.getDistanceMatrix();

	draw_name("Kirchoff Matrix", kNodeAmount * kTabSignsAmount + 2);
	std::cout << matrix_oriented.getKirchoffMatrix();
}
/*
int main()
{
	AdjacencyMatrix<kNodeAmount> matrix_not_oriented;
	AdjacencyMatrix<kNodeAmount, true> matrix_oriented;

	std::cout << matrix_not_oriented << '\n';
	matrix_not_oriented.connect_all();
	std::cout << matrix_not_oriented << '\n';
	matrix_not_oriented.clear_matrix();
	std::cout << matrix_not_oriented << '\n';
	matrix_not_oriented.add_edge(1, 2);
	std::cout << matrix_not_oriented << '\n';
	matrix_not_oriented.remove_edge(1, 2);
	std::cout << matrix_not_oriented << '\n';

	matrix_oriented.add_edge(1, 2);
	std::cout << matrix_oriented << '\n';
	matrix_oriented.remove_edge(1, 2);
	std::cout << matrix_oriented << '\n';
}*/
