#include <array>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <ostream>

namespace GraphFirst
{
using node_value_type = int16_t;
constexpr node_value_type node_value_max =
	std::numeric_limits<node_value_type>::max();

template <bool isWeighted, typename value_type = node_value_type>
class graphTraits {};

template <typename value_type> class graphTraits<true, value_type> {
    public:
	template <size_t N> using container_inside = std::bitset<N>;
};

template <typename value_type> class graphTraits<false, value_type> {
    public:
	template <size_t N>
	using container_inside = typename std::array<value_type, N>;
};

template <size_t NodeAmount, bool isOriented = true, bool isWeighted = true,
	  typename value_type = node_value_type>
class AdjacencyMatrix {
    public:
	using AdjacencyMatrixSelf =
		AdjacencyMatrix<NodeAmount, isOriented, isWeighted, value_type>;

    private:
	using ContainerInside = typename graphTraits<
		isWeighted, value_type>::template container_inside<NodeAmount>;

	using matrix_type = std::array<ContainerInside, NodeAmount>;

	using IntersectionMatrixRight =
		AdjacencyMatrix<NodeAmount + 1, isOriented, isWeighted,
				value_type>;

	using VisitedMatrix = std::array<bool, NodeAmount>;
	using WeightsMatrix = std::array<value_type, NodeAmount>;
	using IncidenceMatrix =
		std::array<std::array<value_type, 0>, NodeAmount>;

	using DegreeMatrix = matrix_type;
	using ReachabilityMatrix = matrix_type;
	using DistanceMatrix = matrix_type;
	using KirchoffMatrix = matrix_type;

    public:
	friend std::ostream &operator<<(std::ostream &out,
					const AdjacencyMatrixSelf &matrix);

	void add_edge(size_t firstNode, size_t secondNode,
		      value_type value = 1);
	void remove_edge(size_t firstNode, size_t secondNode);
	void clear_matrix();
	void connect_all(value_type value = 1);

	IncidenceMatrix getIncidenceMatrix();
	DegreeMatrix getDegreeMatrix();
	ReachabilityMatrix getReachabilityMatrix();
	DistanceMatrix getDistanceMatrix();

	matrix_type &get_matrix()
	{
		return matrix;
	}
	const matrix_type &get_cmatrix() const
	{
		return matrix;
	}

	KirchoffMatrix getKirchoffMatrix();
	AdjacencyMatrixSelf
	GraphIntersection(const IntersectionMatrixRight &matrix_second,
			  size_t removed_vertex);
	AdjacencyMatrix<NodeAmount - 1, isOriented, isWeighted, value_type>
	PullEdge(size_t first_node, size_t node_to_pull);

	AdjacencyMatrix() = default;

	AdjacencyMatrix(const AdjacencyMatrix &) = default;
	AdjacencyMatrix(AdjacencyMatrix &&) = default;

	AdjacencyMatrix &operator=(const AdjacencyMatrix &) = default;
	AdjacencyMatrix &operator=(AdjacencyMatrix &&) = default;

    private:
	matrix_type matrix = { 0 };

	value_type djkstra(size_t first_node_index, size_t second_node_index);
	bool isReachable(size_t first_node, size_t second_node);
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
std::ostream &operator<<(std::ostream &out,
			 const AdjacencyMatrix<NodeAmount, isOriented,
					       isWeighted, value_type> &matrix)
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
};
