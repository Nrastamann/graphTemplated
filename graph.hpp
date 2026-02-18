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
constexpr size_t kNodeAmountResizable{ 0 };
using node_value_type = int16_t;
constexpr node_value_type node_value_max =
	std::numeric_limits<node_value_type>::max();

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

template <size_t NodeAmount, bool isOriented = true, bool isWeighted = true,
	  typename value_type = node_value_type>
class AdjacencyMatrix
	: graphContainerTrait<isWeighted, value_type, NodeAmount>,
	  IncidenceMatrixType<isOriented, value_type, NodeAmount> {
    private:
    public:
	using signed_value_type = std::make_signed_t<value_type>;
	using unsigned_value_type = std::make_unsigned_t<value_type>;

	using ContainerType =
		typename graphContainerTrait<isWeighted, value_type,
					     NodeAmount>::Container;
	using NameContainerType =
		typename graphContainerTrait<isWeighted, value_type,
					     NodeAmount>::NamesContainer;

	using VisitedMatrix = std::array<bool, NodeAmount>;
	using WeightsMatrix = std::array<value_type, NodeAmount>;

	using IncidenceMatrix =
		typename IncidenceMatrixType<isOriented, value_type,
					     NodeAmount>::IncidenceMatrix;

	using DegreeMatrix =
		std::array<std::array<unsigned_value_type, NodeAmount>,
			   NodeAmount>;
	using ReachabilityMatrix =
		std::array<std::bitset<NodeAmount>, NodeAmount>;
	using DistanceMatrix =
		std::array<std::array<signed_value_type, NodeAmount>,
			   NodeAmount>;
	using KirchoffMatrix =
		std::array<std::array<signed_value_type, NodeAmount>,
			   NodeAmount>;

	template <size_t NodeAmountArg, bool isOrientedArg, bool isWeightedArg,
		  typename value_type_arg>
	friend std::ostream &operator<<(
		std::ostream &out,
		const AdjacencyMatrix<NodeAmountArg, isOrientedArg,
				      isWeightedArg, value_type_arg> &matrix);

	void add_edge(size_t firstNode, size_t secondNode,
		      value_type value = 1);
	void remove_edge(size_t firstNode, size_t secondNode);
	void clear_matrix();
	void connect_all(value_type value = 1);

	IncidenceMatrix getIncidenceMatrix();
	DegreeMatrix getDegreeMatrix();
	ReachabilityMatrix getReachabilityMatrix();
	DistanceMatrix getDistanceMatrix();
	KirchoffMatrix getKirchoffMatrix();

	ContainerType &get_matrix()
	{
		return matrix;
	}
	const ContainerType &get_cmatrix() const
	{
		return matrix;
	}

	AdjacencyMatrix() = default;

	AdjacencyMatrix(const AdjacencyMatrix &) = default;
	AdjacencyMatrix(AdjacencyMatrix &&) = default;

	AdjacencyMatrix &operator=(const AdjacencyMatrix &) = default;
	AdjacencyMatrix &operator=(AdjacencyMatrix &&) = default;

    private:
	ContainerType matrix = { 0 };
	NameContainerType matrix_names;
	std::vector<std::pair<size_t, size_t> > getEdges();
	signed_value_type djkstra(size_t first_node_index,
				  size_t second_node_index);
	bool isReachable(size_t first_node, size_t second_node);
};

template <bool isOriented, bool isWeighted, typename value_type>
class AdjacencyMatrix<kNodeAmountResizable, isOriented, isWeighted, value_type>
	: graphContainerTrait<isWeighted, value_type, kNodeAmountResizable>,
	  IncidenceMatrixType<isOriented, value_type, kNodeAmountResizable> {
    public:
	using signed_value_type = std::make_signed_t<value_type>;
	using unsigned_value_type = std::make_unsigned_t<value_type>;

	using ContainerType =
		typename graphContainerTrait<isWeighted, value_type,
					     kNodeAmountResizable>::Container;
	using NameContainerType = typename graphContainerTrait<
		isWeighted, value_type, kNodeAmountResizable>::NamesContainer;

	using VisitedMatrix = std::vector<bool>;
	using WeightsMatrix = std::vector<value_type>;

	using IncidenceMatrix = typename IncidenceMatrixType<
		isOriented, value_type, kNodeAmountResizable>::IncidenceMatrix;

	using DegreeMatrix = std::vector<std::vector<unsigned_value_type> >;
	using ReachabilityMatrix =
		std::vector<std::vector<unsigned_value_type> >;
	using DistanceMatrix = std::vector<std::vector<signed_value_type> >;
	using KirchoffMatrix = std::vector<std::vector<signed_value_type> >;

    public:
	template <size_t NodeAmountArg, bool isOrientedArg, bool isWeightedArg,
		  typename value_type_arg>
	friend std::ostream &operator<<(
		std::ostream &out,
		const AdjacencyMatrix<NodeAmountArg, isOrientedArg,
				      isWeightedArg, value_type_arg> &matrix);

	void add_edge(size_t firstNode, size_t secondNode,
		      value_type value = 1);
	void remove_edge(size_t firstNode, size_t secondNode);
	void clear_matrix();
	void connect_all(value_type value = 1);

	void remove_vertex(size_t vertex_index);

	IncidenceMatrix getIncidenceMatrix();
	DegreeMatrix getDegreeMatrix();
	ReachabilityMatrix getReachabilityMatrix();
	DistanceMatrix getDistanceMatrix();
	KirchoffMatrix getKirchoffMatrix();

	template <size_t NodeAmountSend>
	AdjacencyMatrix<kNodeAmountResizable, isOriented, isWeighted, value_type>
	GraphIntersection(
		const AdjacencyMatrix<NodeAmountSend, isOriented, isWeighted,
				      value_type> &SecondMatrix)
	{
		AdjacencyMatrix<kNodeAmountResizable, isOriented, isWeighted,
				value_type>
			result;

		return result;
	}
	AdjacencyMatrix<kNodeAmountResizable, isOriented, isWeighted, value_type>
	PullEdge(size_t first_node, size_t node_to_pull)
	{
		AdjacencyMatrix<kNodeAmountResizable, isOriented, isWeighted,
				value_type>
			result_matrix;

		return result_matrix;
	}
	void addVertex();

	ContainerType &get_matrix()
	{
		return matrix;
	}
	const ContainerType &get_cmatrix() const
	{
		return matrix;
	}

	AdjacencyMatrix() = default;

	AdjacencyMatrix(const AdjacencyMatrix &) = default;
	AdjacencyMatrix(AdjacencyMatrix &&) = default;

	AdjacencyMatrix &operator=(const AdjacencyMatrix &) = default;
	AdjacencyMatrix &operator=(AdjacencyMatrix &&) = default;

    private:
	ContainerType matrix;
	NameContainerType matrix_names;

	std::vector<std::pair<size_t, size_t> > getEdges();
	signed_value_type djkstra(size_t first_node_index,
				  size_t second_node_index);
	bool isReachable(size_t first_node, size_t second_node);
};
template <bool isOriented, bool isWeighted, typename value_type>
void AdjacencyMatrix<kNodeAmountResizable, isOriented, isWeighted,
		     value_type>::addVertex()
{
	matrix.push_back({});
	matrix[matrix.size() - 1].resize(matrix.size()); //need to set to 0
	for (auto it = matrix.begin(); it != matrix.end() - 1; ++it) {
		(*it).push_back(0);
	}
};

template <bool isOriented, bool isWeighted, typename value_type>
using ResizableAdjacencyMatrix =
	AdjacencyMatrix<kNodeAmountResizable, isOriented, isWeighted,
			value_type>;
};
