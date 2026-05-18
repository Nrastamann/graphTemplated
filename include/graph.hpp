#pragma once
#include <algorithm>
#include <array>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <queue>
#include <set>
#include <span>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace graph_first {

  using default_node_value_type = int16_t;
  struct ClustersFlag {};

  enum class GraphFlags : uint8_t {
    Weighted,
    Oriented,
  };

  namespace graph_flags {
    constexpr size_t kEmpty{};

    constexpr size_t kWeighted{1 << static_cast<size_t>(GraphFlags::Weighted)};
    constexpr size_t kOriented{1 << static_cast<size_t>(GraphFlags::Oriented)};
    constexpr size_t kFull{kWeighted | kOriented};

  }  // namespace graph_flags

  struct AdjacencyListTag {};
  struct EdgesListTag {};
  struct NodeListTag {};
  struct AdjacencyMatrixTag {};

  template <size_t Nodeamount, size_t Flags, typename Valuetype,
            typename Containertag>
  class Graph;

  namespace graph_types {
    constexpr size_t kNodeAmountResizable{0};
    template <size_t GraphFlags, typename ValueType = uint8_t,
              typename ContainerTag = AdjacencyMatrixTag>
    using ResizableGraph =
        Graph<kNodeAmountResizable, GraphFlags, ValueType, ContainerTag>;

    template <size_t NodeAmount, typename ValueType,
              typename ContainerTag = AdjacencyMatrixTag>
    using WeightedGraph =
        Graph<NodeAmount, graph_flags::kWeighted, ValueType, ContainerTag>;

    template <size_t NodeAmount, typename ValueType = uint8_t,
              typename ContainerTag = AdjacencyMatrixTag>
    using OrientedGraph =
        Graph<NodeAmount, graph_flags::kOriented, ValueType, ContainerTag>;

    template <size_t NodeAmount, typename ValueType,
              typename ContainerTag = AdjacencyMatrixTag>
    using WeightedOrientedGraph =
        Graph<NodeAmount, graph_flags::kFull, ValueType, ContainerTag>;
  }  // namespace graph_types

  template <typename ValueType>
  struct EdgeEntry {
    size_t _startNode;
    size_t _endNode;
    ValueType _value;
  };

  template <typename ValueType>
  struct NodeEntry {
    size_t _idx{};
    std::string _name;
    std::vector<size_t> _neighbours;
    std::vector<std::pair<size_t, ValueType>> _edges;
  };

  template <typename ValueType>
  using AdjacencyListEntry = std::pair<size_t, ValueType>;

  //===========================
  template <bool IsWeighted, typename ValueType, size_t N,
            typename ContainerTag = AdjacencyMatrixTag>
  class GraphContainerTrait {
   protected:
    using Container      = std::array<std::array<ValueType, N>, N>;
    using NamesContainer = std::array<std::string, N>;
  };

  template <bool IsWeighted, typename ValueType>
  class GraphContainerTrait<IsWeighted, ValueType,
                            graph_types::kNodeAmountResizable,
                            AdjacencyMatrixTag> {
   protected:
    using Container      = std::vector<std::vector<ValueType>>;
    using NamesContainer = std::vector<std::string>;
  };

  template <bool IsWeighted, typename ValueType, size_t N>
  class GraphContainerTrait<IsWeighted, ValueType, N, AdjacencyListTag> {
   protected:
    using Container = std::array<std::vector<AdjacencyListEntry<ValueType>>, N>;
    using NamesContainer = std::array<std::string, N>;
  };

  template <bool IsWeighted, typename ValueType>
  class GraphContainerTrait<IsWeighted, ValueType,
                            graph_types::kNodeAmountResizable,
                            AdjacencyListTag> {
   protected:
    using Container = std::vector<std::vector<AdjacencyListEntry<ValueType>>>;
    using NamesContainer = std::vector<std::string>;
  };

  template <bool IsWeighted, typename ValueType, size_t N>
  class GraphContainerTrait<IsWeighted, ValueType, N, EdgesListTag> {
   protected:
    using Container      = std::array<EdgeEntry<ValueType>, N>;
    using NamesContainer = std::array<std::string, N>;
  };

  template <bool IsWeighted, typename ValueType>
  class GraphContainerTrait<IsWeighted, ValueType,
                            graph_types::kNodeAmountResizable, EdgesListTag> {
   protected:
    using Container      = std::vector<EdgeEntry<ValueType>>;
    using NamesContainer = std::vector<std::string>;
  };

  template <bool IsWeighted, typename ValueType, size_t N>
  class GraphContainerTrait<IsWeighted, ValueType, N, NodeListTag> {
   protected:
    using Container      = std::array<NodeEntry<ValueType>, N>;
    using NamesContainer = bool;
  };

  template <bool IsWeighted, typename ValueType>
  class GraphContainerTrait<IsWeighted, ValueType,
                            graph_types::kNodeAmountResizable, NodeListTag> {
   protected:
    using Container      = std::vector<NodeEntry<ValueType>>;
    using NamesContainer = bool;
  };

  template <bool IsOriented, typename ValueType, size_t NodeAmount>
  class IncidenceMatrixType {};

  template <typename ValueType, size_t NodeAmount>
  class IncidenceMatrixType<true, ValueType, NodeAmount> {
   protected:
    using IncidenceMatrix = std::array<std::vector<ValueType>, NodeAmount>;
  };

  template <typename ValueType, size_t NodeAmount>
  class IncidenceMatrixType<false, ValueType, NodeAmount> {
   protected:
    using IncidenceMatrix = std::array<std::vector<bool>, NodeAmount>;
  };
  template <typename ValueType>
  class IncidenceMatrixType<false, ValueType,
                            graph_types::kNodeAmountResizable> {
   protected:
    using IncidenceMatrix = std::vector<std::vector<ValueType>>;
  };
  template <typename ValueType>
  class IncidenceMatrixType<true, ValueType,
                            graph_types::kNodeAmountResizable> {
   protected:
    using IncidenceMatrix = std::vector<std::vector<bool>>;
  };

  template <size_t NodeAmount, typename ValueType>
  class MatrixTraits {
   protected:
    using DegreeMatrix =
        std::array<std::array<std::make_unsigned_t<ValueType>, NodeAmount>,
                   NodeAmount>;
    using ReachabilityMatrix = std::array<std::bitset<NodeAmount>, NodeAmount>;
    using DistanceMatrix =
        std::array<std::array<std::make_signed_t<ValueType>, NodeAmount>,
                   NodeAmount>;
    using KirchoffMatrix =
        std::array<std::array<std::make_signed_t<ValueType>, NodeAmount>,
                   NodeAmount>;
    using VisitedMatrix = std::array<bool, NodeAmount>;
    using WeightsMatrix = std::array<ValueType, NodeAmount>;
  };

  template <typename ValueType>
  class MatrixTraits<graph_types::kNodeAmountResizable, ValueType> {
   protected:
    using DegreeMatrix =
        std::vector<std::vector<std::make_unsigned_t<ValueType>>>;
    using ReachabilityMatrix =
        std::vector<std::vector<std::make_unsigned_t<ValueType>>>;
    using DistanceMatrix =
        std::vector<std::vector<std::make_signed_t<ValueType>>>;
    using KirchoffMatrix =
        std::vector<std::vector<std::make_signed_t<ValueType>>>;
    using VisitedMatrix = std::vector<bool>;
    using WeightsMatrix = std::vector<ValueType>;
  };

  template <size_t Flags, size_t NodeAmount, typename ValueType,
            typename ContainerType>
  class GraphTraits
      : protected MatrixTraits<NodeAmount, ValueType>,
        protected IncidenceMatrixType<(Flags & graph_flags::kOriented) != 0U,
                                      ValueType, NodeAmount>,
        protected GraphContainerTrait<(Flags & graph_flags::kWeighted) != 0U,
                                      ValueType, NodeAmount, ContainerType> {
   protected:
    using matrix_traits = MatrixTraits<NodeAmount, ValueType>;
    using incedence_matrix_type =
        IncidenceMatrixType<(Flags & graph_flags::kOriented) != 0U, ValueType,
                            NodeAmount>;
    using graph_container_traits =
        GraphContainerTrait<(Flags & graph_flags::kWeighted) != 0U, ValueType,
                            NodeAmount>;
  };

  //=============================
  template <size_t NodeAmount, size_t Flags, typename ValueType,
            typename ContainerTag = AdjacencyMatrixTag>
  class Graph
      : protected GraphTraits<Flags, NodeAmount, ValueType, ContainerTag> {
   private:
    using traits = GraphTraits<Flags, NodeAmount, ValueType, ContainerTag>;
    static constexpr ValueType kNodeValueMax =
        std::numeric_limits<ValueType>::max();

    static constexpr bool kIsOriented{(Flags & graph_flags::kOriented) != 0U};
    static constexpr bool kIsWeighted{(Flags & graph_flags::kWeighted) != 0U};
    static constexpr bool kResizable{NodeAmount ==
                                     graph_types::kNodeAmountResizable};

   public:
    using signed_value_type   = std::make_signed_t<ValueType>;
    using unsigned_value_type = std::make_unsigned_t<ValueType>;

    using ContainerType       = traits::Container;
    using NameContainerType   = traits::NamesContainer;
    using VisitedMatrix       = traits::VisitedMatrix;
    using WeightsMatrix       = traits::WeightsMatrix;
    using IncidenceMatrix     = traits::IncidenceMatrix;
    using DegreeMatrix        = traits::DegreeMatrix;
    using ReachabilityMatrix  = traits::ReachabilityMatrix;
    using DistanceMatrix      = traits::DistanceMatrix;
    using KirchoffMatrix      = traits::KirchoffMatrix;

    template <typename ContainerType = ContainerTag>
    void
    addEdgeImpl(size_t firstNode, size_t secondNode, ValueType value = 1)
    {
      _matrix[firstNode][secondNode] = value;
      if constexpr (!kIsOriented) {
        _matrix[secondNode][firstNode] = value;
      }
    }

    /*
    template <>
    void
    addEdgeImpl<AdjacencyListTag>(size_t firstNode, size_t secondNode,
                                  ValueType value = 1)
    {
      if (_matrix[firstNode].size() == 0 ||
          _matrix[firstNode][_matrix.size() - 1] != 0) {
        _matrix[firstNode].emplace_back();
      }

      auto result = std::ranges::find(
          _matrix[firstNode], [](auto& value) { return value.first == 0; });
      result->first            = secondNode;
      result->second           = value;

      _matrix[firstNode].first = firstNode;
      if constexpr (!kIsOriented) {
        if (_matrix[secondNode].size() == 0 ||
            _matrix[secondNode][_matrix.size() - 1] != 0) {
          _matrix[secondNode].emplace_back();
        }
        result = std::ranges::find(
            _matrix[secondNode], [](auto& value) { return value.first == 0; });
        result->first  = firstNode;
        result->second = value;
      }
    }


     * */

    template <>
    void
    addEdgeImpl<NodeListTag>(size_t firstNode, size_t secondNode,
                             ValueType value)
    {
      _matrix[firstNode]._edges.emplace_back(secondNode, value);

      bool check = {false};
      for (auto& neh : _matrix[firstNode]._neighbours) {
        if (neh == secondNode) {
          check = true;
          break;
        }
      }
      if (!check && _matrix[firstNode]._idx != secondNode) {
        _matrix[firstNode]._neighbours.emplace_back(secondNode);
      }
      check = false;
      for (auto& neh : _matrix[secondNode]._neighbours) {
        if (neh == secondNode) {
          check = true;
          break;
        }
      }
      if (!check && _matrix[secondNode]._idx != firstNode) {
        _matrix[secondNode]._neighbours.emplace_back(firstNode);
      }
      // need to add for not-oriented
    }

    template <>
    void
    addEdgeImpl<EdgesListTag>(size_t firstNode, size_t secondNode,
                              ValueType value)
    {
      if constexpr (kResizable) {
        _matrix.push_back({firstNode, secondNode, value});
        return;
      }
    }

    void
    addEdge(size_t firstNode, size_t secondNode, ValueType value = 1)
    {
      // static_assert(!kResizable, "Not resizable");
      if constexpr (!kResizable) {
      }
      else {
        /*   if (firstNode >= _matrix.size() || secondNode >= _matrix.size()) {
             _matrix.resize(std::max(firstNode, secondNode));
           }*/
      }
      addEdgeImpl(firstNode, secondNode, value);
    }

    std::vector<size_t>
    colorEdges()
    {
      std::vector<size_t> result_colours(_matrix.size());
      std::vector<std::set<size_t>> used_colours(_matrix.size());

      for (size_t i = 0; i != result_colours.size(); ++i) {
        size_t colour_id{};

        for (auto colour : used_colours[i]) {
          if (colour_id != colour) {
            break;
          }
          colour_id++;
        }

        result_colours[i] = colour_id;

        for (auto edge = _matrix.begin(); edge != _matrix.end(); ++edge) {
          if (edge->_startNode == _matrix[i]._startNode ||
              edge->_startNode == _matrix[i]._endNode ||
              edge->_endNode == _matrix[i]._endNode ||
              edge->_endNode == _matrix[i]._startNode) {
            used_colours[edge - _matrix.begin()].insert(colour_id);
          }
        }
      }
      return result_colours;
    }

    /*
        void
        removeEdge(size_t firstNode, size_t secondNode)
        {
            _matrix[firstNode][secondNode] = 0;
          if constexpr (!IsOriented) {
            _matrix[secondNode][firstNode] = 0;
          }
        }
    */
    /*    void
        clearMatrix()
        {
          for (auto& row : _matrix) {
            for (auto& element : row) {
              element = 0;
            }
          }
        }

        void
        connectAll(ValueType value = 1)
        {
          for (auto& row : _matrix) {
            for (auto& element : row) {
              element = value;
            }
          }
        }

    template <typename T = ContainerTag>
    void
    addName(const std::string& str, size_t idx)
    {
      if constexpr (kResizable) {
        if (_matrix_names.size() <= idx) {
          _matrix_names.resize(idx + 1);
        }
      }
      _matrix_names[idx] = str;
    }
    template <>
    void
    addName<NodeListTag>(const std::string& str, size_t idx)
    {
      _matrix[idx]._name = str;
    }

    template <size_t NodeAmountSend>
    Graph<kNodeAmountResizable, IsOriented, IsWeighted, ValueType>
    graphIntersection(const Graph<NodeAmountSend, IsOriented, IsWeighted,
                                  ValueType>& SecondMatrix) const;
    Graph<kNodeAmountResizable, IsOriented, IsWeighted, ValueType>
    pullEdge(size_t first_node, size_t node_to_pull) const
    {
      Graph<kNodeAmountResizable, IsOriented, IsWeighted, ValueType>
          result_matrix{};
      result_matrix.resize(_matrix.size() - 1);
      auto& working_matrix = result_matrix.getMatrix();

      size_t index_i       = 0;
      for (size_t i = 0; i < _matrix.size(); ++i) {
        if (i == node_to_pull) {
          continue;
        }

        size_t matrix_index_i = i == first_node ? node_to_pull : i;
        size_t index_j        = 0;
        for (size_t j = 0; j < _matrix[0].size(); ++j) {
          if (j == node_to_pull) {
            continue;
          }

          size_t matrix_index_j = j == first_node ? node_to_pull : j;
          working_matrix[index_i][index_j++] =
              _matrix[matrix_index_i][matrix_index_j];
        }
        index_i++;
      }
      return result_matrix;
    }

    void
    addVertex(const std::string& name)
    {
      _matrix_names.push_back(name);
      _matrix.push_back({});
      _matrix[_matrix.size() - 1].resize(_matrix.size());  // need to set to 0
      for (auto it = _matrix.begin(); it != _matrix.end() - 1; ++it) {
        (*it).push_back(0);
      }
    }
    */
    void
    resize(size_t size_new)
    {
      _matrix.resize(size_new);
      for (auto& row : _matrix) {
        row.resize(size_new);
      }
    }

    void
    removeVertex(size_t vertex_index)
    {
      _matrix.erase(_matrix.begin() + static_cast<int64_t>(vertex_index));
      for (auto& row : _matrix) {
        row.erase(row.begin() + static_cast<int64_t>(vertex_index));
      }
    }
    /*
    double
    getClasterization()
    {
      std::vector<double> local_claster(_matrix.size());

      for (size_t i = 0; i < _matrix.size(); ++i) {
        uint64_t sum_value{};
        size_t k_value{};
        for (size_t j = i + 1; j < _matrix[i].size(); ++j) {
          for (size_t k = j + 1; k < _matrix[i].size(); ++k) {
            sum_value += static_cast<uint64_t>(_matrix[i][j] * _matrix[j][k] *
                                               _matrix[k][i]);
          }
          k_value += static_cast<size_t>(_matrix[i][j]);
        }
        if (k_value <= 1) {
          sum_value = 0;
          k_value   = 2;
        }
        local_claster[i] = static_cast<double>(sum_value) /
                           static_cast<double>(k_value * (k_value - 1));
      }

      return static_cast<double>(std::accumulate(local_claster.begin(),
                                                 local_claster.end(), 0.)) /
             static_cast<double>(local_claster.size());
    }
  */
    template <typename Tag = ContainerTag>
    bool
    isThereChain(std::span<size_t> vertexes)
    {
      for (size_t i = 0; i < vertexes.size() - 1; ++i) {
        if (_matrix[vertexes[i]][vertexes[i + 1]] == 0) {
          return false;
        }
      }
      return _matrix[vertexes[vertexes.size() - 1]][vertexes[0]] != 0;
    }
    void
    dfs(int src, int dest, std::vector<size_t>& path,
        std::vector<bool>& visited, std::vector<std::vector<size_t>>& allPathes)
    {
      path.push_back(src);

      visited[src] = true;
      if (src == dest) {
        allPathes.push_back(path);
      }
      else {
        for (auto& edge : _matrix) {
          if (visited[edge._endNode] || edge._startNode != src) {
            continue;
          }
          visited[edge._endNode] = true;
          dfs(edge._endNode, dest, path, visited, allPathes);
        }
      }

      path.pop_back();
      visited[src] = false;
    }
    std::vector<size_t>
    removeExternal()
    {
      std::vector<size_t> edges_to_remove{};
      size_t degr{};
      for (auto i = 0; i != _matrix.size(); ++i) {
        for (auto j = i; j != _matrix.size(); ++j) {
          if (_matrix[i]._startNode == _matrix[j]._endNode &&
              _matrix[j]._startNode == _matrix[i]._endNode) {
            edges_to_remove.push_back(j);
          }
        }
      }

      for (auto i : edges_to_remove) {
        _matrix.erase(_matrix.begin() + i - (degr++));
      }
    }

    std::vector<std::vector<size_t>>
    getAllPathes(size_t point1, size_t point2)
    {
      std::set<size_t> vertex_count{};
      std::ranges::for_each(_matrix, [&vertex_count](auto& edge) {
        vertex_count.insert(edge._startNode);
        vertex_count.insert(edge._endNode);
      });

      std::vector<std::vector<size_t>> pathes;
      std::vector<size_t> path;
      std::vector<bool> visited(vertex_count.size(), false);

      dfs(point1, point2, path, visited, pathes);
      removeExternal();

      return pathes;
    }
    size_t
    processPath(const std::vector<size_t>& path, std::vector<size_t>& edges)
    {
      size_t cost = 0;
      for (size_t i = 0; i != path.size() - 1; ++i) {
        size_t edge_idx = 0;
        for (auto& edge : _matrix) {
          if ((edge._startNode == path[i] && edge._endNode == path[i + 1]) ||
              (edge._endNode == path[i] && edge._startNode == path[i + 1])) {
            cost += edge._value;
            edges.push_back(edge_idx);
            break;
          }
          edge_idx++;
        }
      }
    }
    template <>
    bool
    isThereChain<NodeListTag>(std::span<size_t> vertexes)
    {
      for (size_t i = 0; i < vertexes.size() - 1; ++i) {
        bool check{false};
        for (auto& neight : _matrix[vertexes[i]]._neighbours) {
          if (neight == vertexes[i + 1]) {
            check = true;
            break;
          }
        }
        if (!check) {
          return false;
        }
      }

      for (auto& neghb : _matrix[vertexes[vertexes.size() - 1]]._neighbours) {
        if (neghb == vertexes[0]) {
          return true;
        }
      }
      return false;
    }

    template <>
    bool
    isThereChain<EdgesListTag>(std::span<size_t> vertexes)
    {
      for (size_t i = 0; i < vertexes.size() - 1; ++i) {
        bool check = false;

        for (auto& edge : _matrix) {
          if (edge._startNode == vertexes[i] &&
              edge._endNode == vertexes[i + 1]) {
            check = true;
            break;
          }
        }
        if (!check) {
          return false;
        }
      }

      for (auto& edge : _matrix) {
        if (edge._startNode == vertexes[vertexes.size() - 1] &&
            edge._endNode == vertexes[0]) {
          return true;
        }
      }
      return false;
    }

    template <typename Tag = ContainerTag>
    std::vector<std::pair<size_t, ValueType>>
    getBigVertexes(ValueType limit)
    {
      std::unordered_map<size_t, ValueType> vertexes{};
      std::set<size_t> done_pairs;

      std::vector<std::pair<size_t, ValueType>> res;

      for (size_t i = 0; i < _matrix.size(); ++i) {
        for (size_t j = 0; j < _matrix[i].size(); ++j) {
          if (done_pairs.contains(i)) {
            break;
          }
          if (done_pairs.contains(j)) {
            continue;
          }

          size_t val{};

          bool res_l   = _matrix[i][j] != 0;
          val         += res_l ? _matrix[i][j] : 0;
          res_l        = _matrix[j][i] != 0;
          val         += res_l ? _matrix[j][i] : 0;

          vertexes[i] += val;
          vertexes[j] += val;

          vertexes[i] > limit                        ? done_pairs.insert(i),
              void(res.emplace_back(i, vertexes[i])) : void();
          vertexes[j] > limit                        ? done_pairs.insert(j),
              void(res.emplace_back(j, vertexes[j])) : void();
        }
      }
      return res;
    }

    template <>
    std::vector<std::pair<size_t, ValueType>>
    getBigVertexes<EdgesListTag>(ValueType limit)
    {
      std::unordered_map<size_t, ValueType> vertexes{};
      std::set<size_t> done_pairs;

      std::vector<std::pair<size_t, ValueType>> res;

      for (size_t i = 0; i < _matrix.size(); ++i) {
        size_t st  = _matrix[i]._startNode;
        size_t end = _matrix[i]._endNode;
        size_t val = _matrix[i]._value;

        if (!done_pairs.contains(st)) {
          vertexes[st] += val;
          vertexes[st] > limit                         ? done_pairs.insert(st),
              void(res.emplace_back(st, vertexes[st])) : void();
        }
        if (!done_pairs.contains(end)) {
          vertexes[end] += val;
          vertexes[end] > limit ? done_pairs.insert(end),
              void(res.emplace_back(end, vertexes[end])) : void();
        }
      }
      return res;
    }

    template <>
    std::vector<std::pair<size_t, ValueType>>
    getBigVertexes<NodeListTag>(ValueType limit)
    {
      std::unordered_map<size_t, ValueType> vertexes{};
      std::set<size_t> done_pairs;

      std::vector<std::pair<size_t, ValueType>> res;

      for (size_t i = 0; i < _matrix.size(); ++i) {
        for (size_t j = 0; j < _matrix[i]._edges.size(); ++j) {
          size_t st  = _matrix[i]._idx;
          size_t end = _matrix[i]._edges[j].first;
          size_t val = _matrix[i]._edges[j].second;

          if (!done_pairs.contains(st)) {
            vertexes[st] += val;
            vertexes[st] > limit ? done_pairs.insert(st),
                void(res.emplace_back(st, vertexes[st])) : void();
          }
          if (!done_pairs.contains(end)) {
            vertexes[end] += val;
            vertexes[end] > limit ? done_pairs.insert(end),
                void(res.emplace_back(end, vertexes[end])) : void();
          }
        }
      }
      return res;
    }

    const auto&
    getContainer()
    {
      return _matrix;
    }
    template <typename Tag = ContainerTag>
    std::span<std::string>
    getNames()
    {
      return _matrix_names;
    }
    void
    addEntry(size_t idx, const std::string name)
    {
      _matrix.push_back({idx, name});
    }
    std::vector<std::string_view>
    getNames(NodeListTag)
    {
      std::vector<std::string_view> res;
      for (auto& i : _matrix) {
        res.push_back(i._name);
      }
      return res;
    }
    template <typename Tag = ContainerTag>
    std::set<std::pair<size_t, size_t>>
    bfsEdges(size_t start = 0)
    {
      std::vector<bool> visited(_matrix.size(), false);
      std::vector<size_t> res(_matrix.size());
      std::set<std::pair<size_t, size_t>> edges;
      std::queue<size_t> q;

      res[start] = 0;
      q.push(start);
      visited[start] = true;

      while (!q.empty()) {
        size_t current = q.front();
        q.pop();
        visited[current] = true;

        for (size_t i = 0; i < _matrix[current].size(); ++i) {
          if (_matrix[current][i] == 0) {
            continue;
          }

          edges.emplace(current, i);

          if (!visited[i] && _matrix[current][i] != 0) {
            visited[i] = true;
            res[i]     = res[current] + 1;
            q.push(i);
          }
        }
      }

      return edges;
    }
    template <>
    std::set<std::pair<size_t, size_t>>
    bfsEdges<EdgesListTag>(size_t)
    {
      std::set<std::pair<size_t, size_t>> res;
      for (auto& i : _matrix) {
        res.emplace(i._startNode, i._endNode);
      }
      return res;
    }

    template <>
    std::set<std::pair<size_t, size_t>>
    bfsEdges<NodeListTag>(size_t)
    {
      std::set<std::pair<size_t, size_t>> res;
      for (auto& i : _matrix) {
        for (size_t j = 0; j < i._edges.size(); ++j) {
          res.emplace(i._idx, i._edges[j].first);
        }
      }
      return res;
    }

    std::vector<size_t>
    bfs(size_t start = 0)
    {
      std::vector<bool> visited(_matrix.size(), false);
      std::vector<size_t> res(_matrix.size());
      std::queue<size_t> q;

      res[start] = 0;
      q.push(start);
      visited[start] = true;

      while (!q.empty()) {
        size_t current = q.front();
        q.pop();
        visited[current] = true;

        for (size_t i = 0; i < _matrix[current].size(); ++i) {
          if (!visited[i] && _matrix[current][i] != 0) {
            visited[i] = true;
            res[i]     = res[current] + 1;
            q.push(i);
          }
        }
      }

      return res;
    }

    double
    averagePath()
    {
      uint64_t sum_path{};
      for (size_t i = 0; i < _matrix.size(); ++i) {
        for (size_t j = i + 1; j < _matrix.size(); ++j) {
          uint64_t temp  = _matrix[i][j] != 0
                               ? static_cast<uint64_t>(_matrix[i][j])
                               : static_cast<uint64_t>(djkstra(i, j));
          sum_path      += temp != kNodeValueMax ? temp : 0;
        }
      }
      return static_cast<double>(sum_path) /
             static_cast<double>(_matrix.size() * (_matrix.size() - 1));
    }

    std::vector<std::vector<size_t>>
    getClusters(ClustersFlag)
    {
      std::vector<std::vector<size_t>> result;
      std::vector<size_t> vec(_matrix.size());
      auto it      = vec.begin();
      auto it_prev = vec.begin();

      while (it != vec.end()) {
        size_t index                 = static_cast<size_t>(it - vec.begin());
        std::vector<size_t> vec_temp = bfs(index);
        vec_temp[index]              = 1;

        std::ranges::transform(vec, vec_temp, vec.begin(),
                               [](size_t a1, size_t a2) { return a1 | a2; });

        it = std::ranges::find(vec, 0);
        if (it != it_prev) {
          size_t i = 0;
          std::vector<size_t> temp;
          std::ranges::for_each(vec_temp, [&i, &temp](auto edge_count) {
            if (edge_count != 0) {
              temp.push_back(i);
            }
            ++i;
          });
          result.emplace_back(std::move(temp));
          it_prev = it;
        }
      }
      return result;
    }
    size_t
    getClusters()
    {
      return getClusters({}).size();
    }

    size_t
    getBiggestCluster()
    {
      size_t kluster_size_final{std::numeric_limits<size_t>::min()};

      std::vector<size_t> vec(_matrix.size());
      auto it = vec.begin();

      while (it != vec.end()) {
        size_t index                 = static_cast<size_t>(it - vec.begin());

        std::vector<size_t> vec_temp = bfs(index);
        vec_temp[index]              = 1;

        std::ranges::transform(vec, vec_temp, vec.begin(),
                               [](size_t a1, size_t a2) { return a1 || a2; });

        auto kluster_size = static_cast<size_t>(
            std::ranges::count_if(vec_temp, [](size_t a) { return a > 0; }));

        kluster_size_final = kluster_size > kluster_size_final
                                 ? kluster_size
                                 : kluster_size_final;
        it                 = std::ranges::find(vec, 0);
      }

      return kluster_size_final;
    }

    double
    getDensity()
    {
      size_t edges_n = getEdgesNumber();

      if constexpr (kIsOriented) {
        return (static_cast<double>(edges_n) /
                static_cast<double>(_matrix.size() * (_matrix.size() - 1)));
      }
      return (2 * static_cast<double>(edges_n) /
              static_cast<double>(_matrix.size() * (_matrix.size() - 1)));
    }

    IncidenceMatrix
    getIncidenceMatrix() const
    {
      size_t i_size                                = _matrix.size();
      std::vector<std::pair<size_t, size_t>> edges = getEdges();
      size_t j_size                                = edges.size();
      IncidenceMatrix result_matrix{};
      if constexpr (kResizable) {
        result_matrix.resize(i_size);
      }

      for (size_t i = 0; i < i_size; ++i) {
        result_matrix[i].resize(j_size);
        for (size_t j = 0; j < j_size; ++j) {
          if (edges[j].second == i) {
            result_matrix[i][j] = 1;
          }
          else if (edges[j].first == i) {
            result_matrix[i][j] = -1;
          }
        }
      }

      return result_matrix;
    }
    DegreeMatrix
    getDegreeMatrix() const
    {
      size_t i_size = _matrix.size();
      size_t j_size = _matrix[0].size();

      DegreeMatrix result_matrix{};
      if constexpr (kResizable) {
        result_matrix.resize(i_size);
      }
      for (size_t i = 0; i < i_size; ++i) {
        if constexpr (kResizable) {
          result_matrix[i].resize(j_size);
        }
        for (size_t j = 0; j < j_size; ++j) {
          result_matrix[i][i] += std::max(_matrix[i][j], _matrix[j][i]);
        }
      }

      return result_matrix;
    }
    ReachabilityMatrix
    getReachabilityMatrix() const
    {
      size_t i_size = _matrix.size();
      size_t j_size = _matrix[0].size();

      ReachabilityMatrix result_matrix{};
      if constexpr (kResizable) {
        result_matrix.resize(i_size);
      }
      for (size_t i = 0; i < i_size; ++i) {
        if constexpr (kResizable) {
          result_matrix[i].resize(j_size);
        }
        for (size_t j = 0; j < j_size; ++j) {
          result_matrix[i][j] = i == j ? 1 : isReachable(i, j);
        }
      }

      return result_matrix;
    }
    DistanceMatrix
    getDistanceMatrix() const
    {
      size_t i_size = _matrix.size();
      size_t j_size = _matrix[0].size();

      DistanceMatrix result_matrix{};
      if constexpr (kResizable) {
        result_matrix.resize(i_size);
      }
      for (size_t i = 0; i < i_size; ++i) {
        if constexpr (kResizable) {
          result_matrix[i].resize(j_size);
        }
        for (size_t j = 0; j < j_size; ++j) {
          result_matrix[i][j] = i == j ? 0 : djkstra(i, j);
        }
      }

      return result_matrix;
    }

    KirchoffMatrix
    getKirchoffMatrix() const
    {
      size_t i_size = _matrix.size();
      size_t j_size = _matrix[0].size();

      KirchoffMatrix result_matrix{};
      if constexpr (kResizable) {
        result_matrix.resize(i_size);
      }
      for (size_t i = 0; i < i_size; ++i) {
        if constexpr (kResizable) {
          result_matrix[i].resize(j_size);
        }
        for (size_t j = 0; j < j_size; ++j) {
          result_matrix[i][i] += std::max(_matrix[i][j], _matrix[j][i]);
          if (i != j) {
            result_matrix[i][j] = _matrix[i][j] == 0 && _matrix[j][i] != 0
                                      ? -_matrix[j][i]
                                      : -_matrix[i][j];
          }
        }
      }

      return result_matrix;
    }

    ContainerType&
    getMatrix()
    {
      return _matrix;
    }
    const ContainerType&
    getCmatrix() const
    {
      return _matrix;
    }

    template <typename Tag = ContainerTag>
    [[nodiscard]] std::vector<std::pair<std::string_view, size_t>>
    getNeighbours(size_t idx)
    {
      std::vector<std::pair<std::string_view, size_t>> res;
      auto lambda = [idx](auto value, size_t i) {
        return value != 0 && i != idx;
      };
      for (size_t i = 0; i < _matrix.size(); ++i) {
        if (lambda(_matrix[i][idx], i)) {
          res.emplace_back(_matrix_names[i], i);
        }
      }

      for (size_t j = 0; j < _matrix[idx].size(); ++j) {
        if (lambda(_matrix[idx][j], j)) {
          res.emplace_back(_matrix_names[j], j);
        }
      }

      return res;
    }

    template <>
    [[nodiscard]] std::vector<std::pair<std::string_view, size_t>>
    getNeighbours<EdgesListTag>(size_t idx)
    {
      std::vector<std::pair<std::string_view, size_t>> res;
      std::set<size_t> picked_neighbours;

      for (auto& i : _matrix) {
        bool start = idx == i._startNode;
        if (start || idx == i._endNode) {
          auto idx_to_push = start ? i._endNode : i._startNode;

          res.emplace_back(_matrix_names[idx_to_push], idx_to_push);
          picked_neighbours.insert(idx_to_push);
        }
      }
      return res;
    }
    template <>
    [[nodiscard]] std::vector<std::pair<std::string_view, size_t>>
    getNeighbours<NodeListTag>(size_t idx)
    {
      std::vector<std::pair<std::string_view, size_t>> res;
      std::set<size_t> picked_neighbours;
      for (auto& negh : _matrix[idx]._neighbours) {
        res.emplace_back(_matrix[negh]._name, negh);
        picked_neighbours.insert(negh);
      }

      return res;
    }

    size_t
    size()
    {
      return _matrix.size();
    }

    ~Graph()                       = default;
    Graph()                        = default;

    Graph(const Graph&)            = default;
    Graph(Graph&&)                 = default;

    Graph& operator=(const Graph&) = default;
    Graph& operator=(Graph&&)      = default;

   private:
    ContainerType _matrix{};
    NameContainerType _matrix_names{};

    size_t
    getEdgesNumber()
    {
      return getEdges().size();
    }
    [[nodiscard]] std::vector<std::pair<size_t, size_t>>
    getEdges() const
    {
      std::vector<std::pair<size_t, size_t>> edges{};
      for (size_t i = 0; i != _matrix.size(); ++i) {
        size_t j = i;
        if constexpr (kIsOriented) {
          j = 0;
        }
        for (; j != _matrix[0].size(); ++j) {
          if (_matrix[i][j] != 0) {
            edges.emplace_back(i, j);
          }
        }
      }
      return edges;
    }
    unsigned_value_type
    djkstra(size_t first_node_index, size_t second_node_index) const
    {
      if (first_node_index == second_node_index) {
        return 0;
      }
      VisitedMatrix visited{};
      if constexpr (kResizable) {
        visited.resize(_matrix.size());
      }
      visited[first_node_index] = true;

      WeightsMatrix weights{};

      if constexpr (kResizable) {
        weights.resize(_matrix.size());
      }

      for (auto& i : weights) {
        i = kNodeValueMax;
      }

      weights[first_node_index] = 0;

      size_t current_node       = first_node_index;

      while (current_node != second_node_index) {
        for (size_t i = 0; i < weights.size(); ++i) {
          if (visited[i] || _matrix[current_node][i] == 0) {
            continue;
          }
          ValueType new_value =
              weights[current_node] + _matrix[current_node][i];

          weights[i] = std::min(weights[i], new_value);
        }
        int64_t min_weight_index = -1;
        ValueType min_value      = kNodeValueMax;

        for (size_t i = 0; i < weights.size(); ++i) {
          if (min_value > weights[i] && !visited[i]) {
            min_value        = weights[i];
            min_weight_index = static_cast<int64_t>(i);
          }
        }

        if (min_value == kNodeValueMax) {
          return static_cast<unsigned_value_type>(kNodeValueMax);
        }

        current_node          = static_cast<size_t>(min_weight_index);
        visited[current_node] = true;
      }
      return static_cast<unsigned_value_type>(weights[second_node_index]);
    }

    [[nodiscard]] bool
    isReachable(size_t first_node, size_t second_node) const
    {
      return djkstra(first_node, second_node) != kNodeValueMax;
    }
  };
  /*
    template <size_t NodeAmount2, bool IsOriented, bool IsWeighted,
              typename ValueType2>
    std::ostream&
    operator<<(
        std::ostream& out,
        const Graph<NodeAmount2, IsOriented, IsWeighted, ValueType2>& matrix)
    {
      size_t i_size = matrix._matrix.size();
      size_t j_size = matrix._matrix[0].size();
      out << "\t|";
      for (size_t i = 0; i < i_size; ++i) {
        out << i + 1 << '\t';
      }

      out << '\n' << "\t|";
      for (size_t i = 0; i < i_size; ++i) {
        out << "=======";
      }

      out << '\n';

      for (size_t i = 0; i < i_size; ++i) {
        out << i + 1 << "\t|";
        for (size_t j = 0; j < j_size; ++j) {
          out << std::format("{}\t", matrix._matrix[i][j]);
        }
        out << '\n';
      }
      return out;
    }

    template <typename ContainerInside, size_t CNodeAmount>
    std::ostream&
    operator<<(std::ostream& out,
               const std::array<ContainerInside, CNodeAmount>& matrix)
    {
      size_t i_size = matrix.size();
      size_t j_size = matrix[0].size();
      out << "\t|";
      for (size_t i = 0; i < j_size; ++i) {
        out << i + 1 << '\t';
      }

      out << '\n' << "\t|";
      for (size_t i = 0; i < j_size; ++i) {
        out << "=======";
      }

      out << '\n';

      for (size_t i = 0; i < i_size; ++i) {
        out << i + 1 << "\t|";
        for (size_t j = 0; j < j_size; ++j) {
          out << matrix[i][j] << '\t';
        }
        out << '\n';
      }
      return out;
    }
    template <typename ValueType>
    std::ostream&
    operator<<(std::ostream& out,
               const std::vector<std::vector<ValueType>>& matrix)
    {
      size_t i_size = matrix.size();
      size_t j_size = matrix[0].size();
      out << "\t|";
      for (size_t i = 0; i < j_size; ++i) {
        out << i + 1 << '\t';
      }

      out << '\n' << "\t|";
      for (size_t i = 0; i < j_size; ++i) {
        out << "=======";
      }

      out << '\n';

      for (size_t i = 0; i < i_size; ++i) {
        out << i + 1 << "\t|";
        for (size_t j = 0; j < j_size; ++j) {
          out << matrix[i][j] << '\t';
        }
        out << '\n';
      }
      return out;
    }
    void drawName(std::string_view str, size_t value);*/
}  // namespace graph_first
