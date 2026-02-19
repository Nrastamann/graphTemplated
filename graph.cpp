#include <array>
#include "graph.hpp"
#include <cstddef>
#include <iostream>
#include <ostream>
#include <utility>
#include <vector>

static size_t interest = 0;
static constexpr size_t kNodeAmount{ 7 };
static constexpr size_t kNodeAmountSecond{ 8 };
static constexpr size_t kEdgesAmount{ 9 };
namespace GraphFirst
{

template <size_t NodeAmount_2, bool isOriented, bool isWeighted,
	  typename value_type_2>
std::ostream &
operator<<(std::ostream &out,
	   const GraphFirst::AdjacencyMatrix<NodeAmount_2, isOriented,
					     isWeighted, value_type_2> &matrix)
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
std::vector<std::pair<size_t, size_t> >
AdjacencyMatrix<NodeAmount, isOriented, isWeighted, value_type>::getEdges() const
{
	std::vector<std::pair<size_t, size_t> > edges{};
	for (size_t i = 0; i != matrix.size(); ++i) {
		size_t j = i;
		if constexpr (isOriented) {
			j = 0;
		}
		for (; j != matrix[0].size(); ++j) {
			if (matrix[i][j] != 0) {
				edges.push_back(std::make_pair(i, j));
			}
		}
	}
	return edges;
}

template <size_t NodeAmount, bool isOriented, bool isWeighted,
	  typename value_type>
typename AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
			 value_type>::IncidenceMatrix
AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
		value_type>::getIncidenceMatrix() const
{
	size_t iSize = matrix.size();
	std::vector<std::pair<size_t, size_t> > edges = getEdges();
	size_t jSize = edges.size();
	IncidenceMatrix result_matrix{};
	if constexpr (NodeAmount == kNodeAmountResizable) {
		result_matrix.resize(iSize);
	}

	for (size_t i = 0; i < iSize; ++i) {
		result_matrix[i].resize(jSize);
		for (size_t j = 0; j < jSize; ++j) {
			if (edges[j].first == i) {
				result_matrix[i][j] = -1;
			} else if (edges[j].second == i) {
				result_matrix[i][j] = 1;
			}
		}
	}

	return result_matrix;
}

template <size_t NodeAmount, bool isOriented, bool isWeighted,
	  typename value_type>
typename AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
			 value_type>::DegreeMatrix
AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
		value_type>::getDegreeMatrix() const
{
	size_t iSize = matrix.size();
	size_t jSize = matrix[0].size();

	DegreeMatrix result_matrix{};
	if constexpr (NodeAmount == kNodeAmountResizable) {
		result_matrix.resize(iSize);
	}
	for (size_t i = 0; i < iSize; ++i) {
		if constexpr (NodeAmount == kNodeAmountResizable) {
			result_matrix[i].resize(jSize);
		}
		for (size_t j = 0; j < jSize; ++j) {
			result_matrix[i][i] +=
				std::max(matrix[i][j], matrix[j][i]);
		}
	}

	return result_matrix;
}

template <size_t NodeAmount, bool isOriented, bool isWeighted,
	  typename value_type>
typename AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
			 value_type>::ReachabilityMatrix
AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
		value_type>::getReachabilityMatrix() const
{
	size_t iSize = matrix.size();
	size_t jSize = matrix[0].size();

	ReachabilityMatrix result_matrix{};
	if constexpr (NodeAmount == kNodeAmountResizable) {
		result_matrix.resize(iSize);
	}
	for (size_t i = 0; i < iSize; ++i) {
		if constexpr (NodeAmount == kNodeAmountResizable) {
			result_matrix[i].resize(jSize);
		}
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
		value_type>::getDistanceMatrix() const
{
	size_t iSize = matrix.size();
	size_t jSize = matrix[0].size();

	DistanceMatrix result_matrix{};
	if constexpr (NodeAmount == kNodeAmountResizable) {
		result_matrix.resize(iSize);
	}
	for (size_t i = 0; i < iSize; ++i) {
		if constexpr (NodeAmount == kNodeAmountResizable) {
			result_matrix[i].resize(jSize);
		}
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
		value_type>::getKirchoffMatrix() const
{
	size_t iSize = matrix.size();
	size_t jSize = matrix[0].size();

	KirchoffMatrix result_matrix{};
	if constexpr (NodeAmount == kNodeAmountResizable) {
		result_matrix.resize(iSize);
	}
	for (size_t i = 0; i < iSize; ++i) {
		if constexpr (NodeAmount == kNodeAmountResizable) {
			result_matrix[i].resize(jSize);
		}
		for (size_t j = 0; j < jSize; ++j) {
			result_matrix[i][i] +=
				std::max(matrix[i][j], matrix[j][i]);
			if (i != j) {
				result_matrix[i][j] =
					matrix[i][j] == 0 && matrix[j][i] != 0 ?
						-matrix[j][i] :
						-matrix[i][j];
			}
		}
	}

	return result_matrix;
}

template <size_t NodeAmount, bool isOriented, bool isWeighted,
	  typename value_type>
typename AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
			 value_type>::signed_value_type
AdjacencyMatrix<NodeAmount, isOriented, isWeighted, value_type>::djkstra(
	size_t first_node_index, size_t second_node_index) const
{
	if (first_node_index == second_node_index) {
		return 0;
	}
	VisitedMatrix visited{};
	if constexpr (kNodeAmountResizable == NodeAmount) {
		visited.resize(matrix.size());
	}
	visited[first_node_index] = true;

	WeightsMatrix weights{};

	if constexpr (kNodeAmountResizable == NodeAmount) {
		weights.resize(matrix.size());
	}

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
					      size_t second_node) const
{
	return djkstra(first_node, second_node) != node_value_max;
}
};
template <size_t NodeAmount, bool isOriented, bool isWeighted,
	  typename value_type>
void GraphFirst::AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
				 value_type>::removeVertex(size_t vertex_index)
{
	matrix.erase(matrix.begin() + vertex_index);
	for (auto &row : matrix) {
		row.erase(row.begin() + vertex_index);
	}
}

template <size_t NodeAmount, bool isOriented, bool isWeighted,
	  typename value_type>
void GraphFirst::AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
				 value_type>::addVertex()
{
	matrix.push_back({});
	matrix[matrix.size() - 1].resize(matrix.size()); //need to set to 0
	for (auto it = matrix.begin(); it != matrix.end() - 1; ++it) {
		(*it).push_back(0);
	}
}

template <size_t NodeAmount, bool isOriented, bool isWeighted,
	  typename value_type>
void GraphFirst::AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
				 value_type>::resize(size_t size_new)
{
	matrix.resize(size_new);
	for (auto &row : matrix) {
		row.resize(size_new);
	}
}

//===
template <size_t NodeAmount, bool isOriented, bool isWeighted,
	  typename value_type>
template <size_t NodeAmountSend>
GraphFirst::AdjacencyMatrix<GraphFirst::kNodeAmountResizable, isOriented,
			    isWeighted, value_type>
GraphFirst::AdjacencyMatrix<NodeAmount, isOriented, isWeighted, value_type>::
	GraphIntersection(const GraphFirst::AdjacencyMatrix<
			  NodeAmountSend, isOriented, isWeighted, value_type>
				  &SecondMatrix) const
{
	AdjacencyMatrix<kNodeAmountResizable, isOriented, isWeighted, value_type>
		result;

	return result;
}

template <size_t NodeAmount, bool isOriented, bool isWeighted,
	  typename value_type>
GraphFirst::AdjacencyMatrix<GraphFirst::kNodeAmountResizable, isOriented,
			    isWeighted, value_type>
GraphFirst::AdjacencyMatrix<NodeAmount, isOriented, isWeighted,
			    value_type>::PullEdge(size_t first_node,
						  size_t node_to_pull) const
{
	AdjacencyMatrix<kNodeAmountResizable, isOriented, isWeighted, value_type>
		result_matrix{};
	result_matrix.resize(matrix.size() - 1);
	for (size_t i = 0; i < matrix.size(); ++i) {
	}
	return result_matrix;
}

//===
template <typename ContainerInside, size_t CNodeAmount>
std::ostream &operator<<(std::ostream &out,
			 const std::array<ContainerInside, CNodeAmount> &matrix)
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
template <typename value_type>
std::ostream &operator<<(std::ostream &out,
			 const std::vector<std::vector<value_type> > &matrix)
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

	draw_name("Adjacency Matrix", kNodeAmount * kTabSignsAmount + 2);
	std::cout << matrix_changable;

	draw_name("Incidence Matrix", kEdgesAmount * kTabSignsAmount + 2);
	std::cout << matrix_changable.getIncidenceMatrix();

	draw_name("Degree Matrix", kNodeAmount * kTabSignsAmount + 2);
	std::cout << matrix_changable.getDegreeMatrix();

	draw_name("Reachability Matrix", kNodeAmount * kTabSignsAmount + 2);
	std::cout << matrix_changable.getReachabilityMatrix();

	draw_name("Distance Matrix", kNodeAmount * kTabSignsAmount + 2);
	std::cout << matrix_changable.getDistanceMatrix();

	draw_name("Kirchoff Matrix", kNodeAmount * kTabSignsAmount + 2);
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
