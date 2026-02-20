#pragma once
#include <array>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <ostream>
#include <type_traits>
#include <utility>
#include <vector>

namespace GraphFirst
{
using default_node_value_type = int16_t;

template <size_t NodeAmount, bool isOriented, bool isWeighted,
	  typename value_type>
class AdjacencyMatrix;

constexpr size_t kNodeAmountResizable{ 0 };

template <bool isOriented, bool isWeighted, typename value_type>
using ResizableAdjacencyMatrix =
	AdjacencyMatrix<kNodeAmountResizable, isOriented, isWeighted,
			value_type>;

template <bool isWeighted, typename value_type, size_t N>
class graphContainerTrait {
    protected:
	using Container = std::array<std::array<value_type, N>, N>;
	using NamesContainer = std::array<std::string, N>;
};
template <bool isWeighted, typename value_type>
class graphContainerTrait<isWeighted, value_type, kNodeAmountResizable> {
    protected:
	using Container = std::vector<std::vector<value_type> >;
	using NamesContainer = std::vector<std::string>;
};
template <typename value_type, size_t N>
class graphContainerTrait<false, value_type, N> {
    protected:
	using Container = std::array<std::bitset<N>, N>;
	using NamesContainer = std::vector<std::string>;
};

template <bool isOriented, typename value_type, size_t NodeAmount>
class IncidenceMatrixType {};
template <typename value_type, size_t NodeAmount>
class IncidenceMatrixType<true, value_type, NodeAmount> {
    protected:
	using IncidenceMatrix = std::array<std::vector<value_type>, NodeAmount>;
};
template <typename value_type, size_t NodeAmount>
class IncidenceMatrixType<false, value_type, NodeAmount> {
    protected:
	using IncidenceMatrix = std::array<std::vector<bool>, NodeAmount>;
};
template <typename value_type>
class IncidenceMatrixType<false, value_type, kNodeAmountResizable> {
    protected:
	using IncidenceMatrix = std::vector<std::vector<value_type> >;
};
template <typename value_type>
class IncidenceMatrixType<true, value_type, kNodeAmountResizable> {
    protected:
	using IncidenceMatrix = std::vector<std::vector<bool> >;
};
template <size_t NodeAmount, typename value_type> class MatrixTraits {
    protected:
	using DegreeMatrix = std::array<
		std::array<std::make_unsigned_t<value_type>, NodeAmount>,
		NodeAmount>;
	using ReachabilityMatrix =
		std::array<std::bitset<NodeAmount>, NodeAmount>;
	using DistanceMatrix =
		std::array<std::array<std::make_signed_t<value_type>, NodeAmount>,
			   NodeAmount>;
	using KirchoffMatrix =
		std::array<std::array<std::make_signed_t<value_type>, NodeAmount>,
			   NodeAmount>;
	using VisitedMatrix = std::array<bool, NodeAmount>;
	using WeightsMatrix = std::array<value_type, NodeAmount>;
};

template <typename value_type>
class MatrixTraits<kNodeAmountResizable, value_type> {
    protected:
	using DegreeMatrix =
		std::vector<std::vector<std::make_unsigned_t<value_type> > >;
	using ReachabilityMatrix =
		std::vector<std::vector<std::make_unsigned_t<value_type> > >;
	using DistanceMatrix =
		std::vector<std::vector<std::make_signed_t<value_type> > >;
	using KirchoffMatrix =
		std::vector<std::vector<std::make_signed_t<value_type> > >;
	using VisitedMatrix = std::vector<bool>;
	using WeightsMatrix = std::vector<value_type>;
};

template <size_t NodeAmount, bool isOriented = true, bool isWeighted = true,
	  typename value_type = default_node_value_type>
class AdjacencyMatrix : graphContainerTrait<isWeighted, value_type, NodeAmount>,
			IncidenceMatrixType<isOriented, value_type, NodeAmount>,
			MatrixTraits<NodeAmount, value_type> {
    private:
	static constexpr value_type node_value_max =
		std::numeric_limits<value_type>::max();

    public:
	using signed_value_type = std::make_signed_t<value_type>;
	using unsigned_value_type = std::make_unsigned_t<value_type>;

	using ContainerType =
		typename graphContainerTrait<isWeighted, value_type,
					     NodeAmount>::Container;
	using NameContainerType =
		typename graphContainerTrait<isWeighted, value_type,
					     NodeAmount>::NamesContainer;

	using VisitedMatrix =
		typename MatrixTraits<NodeAmount, value_type>::VisitedMatrix;

	using WeightsMatrix =
		typename MatrixTraits<NodeAmount, value_type>::WeightsMatrix;

	using IncidenceMatrix =
		typename IncidenceMatrixType<isOriented, value_type,
					     NodeAmount>::IncidenceMatrix;

	using DegreeMatrix =
		typename MatrixTraits<NodeAmount, value_type>::DegreeMatrix;
	using ReachabilityMatrix =
		typename MatrixTraits<NodeAmount,
				      value_type>::ReachabilityMatrix;
	using DistanceMatrix =
		typename MatrixTraits<NodeAmount, value_type>::DistanceMatrix;

	using KirchoffMatrix =
		typename MatrixTraits<NodeAmount, value_type>::KirchoffMatrix;

	template <size_t NodeAmountArg, bool isOrientedArg, bool isWeightedArg,
		  typename value_type_arg>
	friend std::ostream &operator<<(
		std::ostream &out,
		const AdjacencyMatrix<NodeAmountArg, isOrientedArg,
				      isWeightedArg, value_type_arg> &matrix);

	inline void add_edge(size_t firstNode, size_t secondNode,
			     value_type value = 1)
	{
		matrix[firstNode][secondNode] = value;
		if constexpr (!isOriented) {
			matrix[secondNode][firstNode] = value;
		}
	}
	inline void remove_edge(size_t firstNode, size_t secondNode)
	{
		matrix[firstNode][secondNode] = 0;
		if constexpr (!isOriented) {
			matrix[secondNode][firstNode] = 0;
		}
	}
	inline void clear_matrix()
	{
		for (auto &row : matrix) {
			for (auto &element : row) {
				element = 0;
			}
		}
	}
	inline void connect_all(value_type value = 1)
	{
		for (auto &row : matrix) {
			for (auto &element : row) {
				element = value;
			}
		}
	}

	template <size_t NodeAmountSend>
	AdjacencyMatrix<kNodeAmountResizable, isOriented, isWeighted, value_type>
	GraphIntersection(
		const AdjacencyMatrix<NodeAmountSend, isOriented, isWeighted,
				      value_type> &SecondMatrix) const;
	AdjacencyMatrix<kNodeAmountResizable, isOriented, isWeighted, value_type>
	PullEdge(size_t first_node, size_t node_to_pull) const
	{
		AdjacencyMatrix<kNodeAmountResizable, isOriented, isWeighted,
				value_type>
			result_matrix{};
		result_matrix.resize(matrix.size() - 1);
		for (size_t i = 0; i < matrix.size(); ++i) {
		}
		return result_matrix;
	}

	void addVertex()
	{
		matrix.push_back({});
		matrix[matrix.size() - 1].resize(
			matrix.size()); //need to set to 0
		for (auto it = matrix.begin(); it != matrix.end() - 1; ++it) {
			(*it).push_back(0);
		}
	}
	void resize(size_t size_new)
	{
		matrix.resize(size_new);
		for (auto &row : matrix) {
			row.resize(size_new);
		}
	}
	void removeVertex(size_t vertex_index)
	{
		matrix.erase(matrix.begin() + vertex_index);
		for (auto &row : matrix) {
			row.erase(row.begin() + vertex_index);
		}
	}

	IncidenceMatrix getIncidenceMatrix() const
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
				if (edges[j].second == i) {
					result_matrix[i][j] = 1;
				} else if (edges[j].first == i) {
					result_matrix[i][j] = -1;
				}
			}
		}

		return result_matrix;
	}
	DegreeMatrix getDegreeMatrix() const
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
	ReachabilityMatrix getReachabilityMatrix() const
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
				result_matrix[i][j] =
					i == j ? 1 : isReachable(i, j);
			}
		}

		return result_matrix;
	}
	DistanceMatrix getDistanceMatrix() const
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
				result_matrix[i][j] = i == j ? 0 :
							       djkstra(i, j);
			}
		}

		return result_matrix;
	}

	KirchoffMatrix getKirchoffMatrix() const
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
						matrix[i][j] == 0 &&
								matrix[j][i] !=
									0 ?
							-matrix[j][i] :
							-matrix[i][j];
				}
			}
		}

		return result_matrix;
	}

	inline ContainerType &get_matrix()
	{
		return matrix;
	}
	inline const ContainerType &get_cmatrix() const
	{
		return matrix;
	}

	AdjacencyMatrix() = default;

	AdjacencyMatrix(const AdjacencyMatrix &) = default;
	AdjacencyMatrix(AdjacencyMatrix &&) = default;

	AdjacencyMatrix &operator=(const AdjacencyMatrix &) = default;
	AdjacencyMatrix &operator=(AdjacencyMatrix &&) = default;

    private:
	ContainerType matrix{};
	NameContainerType matrix_names{};

	std::vector<std::pair<size_t, size_t> > getEdges() const
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
	signed_value_type djkstra(size_t first_node_index,
				  size_t second_node_index) const
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
				value_type new_value = weights[current_node] +
						       matrix[current_node][i];

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

	inline bool isReachable(size_t first_node, size_t second_node) const
	{
		return djkstra(first_node, second_node) != node_value_max;
	}
};
template <size_t NodeAmount_2, bool isOriented, bool isWeighted,
	  typename value_type_2>
std::ostream &
operator<<(std::ostream &out,
	   const AdjacencyMatrix<NodeAmount_2, isOriented, isWeighted,
				 value_type_2> &matrix)
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
}

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
void draw_name(std::string_view str, size_t value);
}
