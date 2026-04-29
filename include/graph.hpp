#pragma once
#include <algorithm>
#include <array>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <format>
#include <iostream>
#include <limits>
#include <numeric>
#include <ostream>
#include <queue>
#include <type_traits>
#include <utility>
#include <vector>

namespace graph_first {
  using default_node_value_type = int16_t;

  template <size_t NodeAmount, bool IsOriented, bool IsWeighted,
            typename ValueType>
  class AdjacencyMatrix;

  constexpr size_t kNodeAmountResizable{0};

  template <bool IsOriented = false, bool IsWeighted = false,
            typename ValueType = uint8_t>
  using ResizableAdjacencyMatrix =
      AdjacencyMatrix<kNodeAmountResizable, IsOriented, IsWeighted, ValueType>;

  template <bool IsWeighted, typename ValueType, size_t N>
  class GraphContainerTrait {
   protected:
    using Container      = std::array<std::array<ValueType, N>, N>;
    using NamesContainer = std::array<std::string, N>;
  };
  template <bool IsWeighted, typename ValueType>
  class GraphContainerTrait<IsWeighted, ValueType, kNodeAmountResizable> {
   protected:
    using Container      = std::vector<std::vector<ValueType> >;
    using NamesContainer = std::vector<std::string>;
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
  class IncidenceMatrixType<false, ValueType, kNodeAmountResizable> {
   protected:
    using IncidenceMatrix = std::vector<std::vector<ValueType> >;
  };
  template <typename ValueType>
  class IncidenceMatrixType<true, ValueType, kNodeAmountResizable> {
   protected:
    using IncidenceMatrix = std::vector<std::vector<bool> >;
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
  class MatrixTraits<kNodeAmountResizable, ValueType> {
   protected:
    using DegreeMatrix =
        std::vector<std::vector<std::make_unsigned_t<ValueType> > >;
    using ReachabilityMatrix =
        std::vector<std::vector<std::make_unsigned_t<ValueType> > >;
    using DistanceMatrix =
        std::vector<std::vector<std::make_signed_t<ValueType> > >;
    using KirchoffMatrix =
        std::vector<std::vector<std::make_signed_t<ValueType> > >;
    using VisitedMatrix = std::vector<bool>;
    using WeightsMatrix = std::vector<ValueType>;
  };

  template <size_t NodeAmount, bool IsOriented = true, bool IsWeighted = true,
            typename ValueType = default_node_value_type>
  class AdjacencyMatrix
      : GraphContainerTrait<IsWeighted, ValueType, NodeAmount>,
        IncidenceMatrixType<IsOriented, ValueType, NodeAmount>,
        MatrixTraits<NodeAmount, ValueType> {
   private:
    static constexpr ValueType kNodeValueMax =
        std::numeric_limits<ValueType>::max();

   public:
    using signed_value_type   = std::make_signed_t<ValueType>;
    using unsigned_value_type = std::make_unsigned_t<ValueType>;

    using ContainerType = typename GraphContainerTrait<IsWeighted, ValueType,
                                                       NodeAmount>::Container;
    using NameContainerType =
        typename GraphContainerTrait<IsWeighted, ValueType,
                                     NodeAmount>::NamesContainer;

    using VisitedMatrix =
        typename MatrixTraits<NodeAmount, ValueType>::VisitedMatrix;

    using WeightsMatrix =
        typename MatrixTraits<NodeAmount, ValueType>::WeightsMatrix;

    using IncidenceMatrix =
        typename IncidenceMatrixType<IsOriented, ValueType,
                                     NodeAmount>::IncidenceMatrix;

    using DegreeMatrix =
        typename MatrixTraits<NodeAmount, ValueType>::DegreeMatrix;
    using ReachabilityMatrix =
        typename MatrixTraits<NodeAmount, ValueType>::ReachabilityMatrix;
    using DistanceMatrix =
        typename MatrixTraits<NodeAmount, ValueType>::DistanceMatrix;

    using KirchoffMatrix =
        typename MatrixTraits<NodeAmount, ValueType>::KirchoffMatrix;

    template <size_t NodeAmountArg, bool IsOrientedArg, bool IsWeightedArg,
              typename ValueTypeArg>
    friend std::ostream& operator<<(
        std::ostream& out,
        AdjacencyMatrix<NodeAmountArg, IsOrientedArg, IsWeightedArg,
                        ValueTypeArg> const& matrix);

    void
    addEdge(size_t firstNode, size_t secondNode, ValueType value = 1)
    {
      _matrix[firstNode][secondNode] = value;
      if constexpr (!IsOriented) {
        _matrix[secondNode][firstNode] = value;
      }
    }
    void
    removeEdge(size_t firstNode, size_t secondNode)
    {
      _matrix[firstNode][secondNode] = 0;
      if constexpr (!IsOriented) {
        _matrix[secondNode][firstNode] = 0;
      }
    }
    void
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

    template <size_t NodeAmountSend>
    AdjacencyMatrix<kNodeAmountResizable, IsOriented, IsWeighted, ValueType>
    graphIntersection(AdjacencyMatrix<NodeAmountSend, IsOriented, IsWeighted,
                                      ValueType> const& SecondMatrix) const;
    AdjacencyMatrix<kNodeAmountResizable, IsOriented, IsWeighted, ValueType>
    pullEdge(size_t first_node, size_t node_to_pull) const
    {
      AdjacencyMatrix<kNodeAmountResizable, IsOriented, IsWeighted, ValueType>
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
    addVertex()
    {
      _matrix.push_back({});
      _matrix[_matrix.size() - 1].resize(_matrix.size());  // need to set to 0
      for (auto it = _matrix.begin(); it != _matrix.end() - 1; ++it) {
        (*it).push_back(0);
      }
    }
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
      _matrix.erase(_matrix.begin() + vertex_index);
      for (auto& row : _matrix) {
        row.erase(row.begin() + vertex_index);
      }
    }
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

    size_t
    getClusters()
    {
      size_t kluster_num{};
      std::vector<size_t> vec(_matrix.size());
      auto it = vec.begin();

      while (it != vec.end()) {
        size_t index                 = static_cast<size_t>(it - vec.begin());
        std::vector<size_t> vec_temp = bfs(index);
        vec_temp[index]              = 1;

        std::ranges::transform(vec, vec_temp, vec.begin(),
                               [](size_t a1, size_t a2) { return a1 | a2; });

        kluster_num++;
        it = std::ranges::find(vec, 0);
      }

      return kluster_num;
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

      if constexpr (IsOriented) {
        return (static_cast<double>(edges_n) /
                static_cast<double>(_matrix.size() * (_matrix.size() - 1)));
      }
      return (2 * static_cast<double>(edges_n) /
              static_cast<double>(_matrix.size() * (_matrix.size() - 1)));
    }

    IncidenceMatrix
    getIncidenceMatrix() const
    {
      size_t i_size                                 = _matrix.size();
      std::vector<std::pair<size_t, size_t> > edges = getEdges();
      size_t j_size                                 = edges.size();
      IncidenceMatrix result_matrix{};
      if constexpr (NodeAmount == kNodeAmountResizable) {
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
      if constexpr (NodeAmount == kNodeAmountResizable) {
        result_matrix.resize(i_size);
      }
      for (size_t i = 0; i < i_size; ++i) {
        if constexpr (NodeAmount == kNodeAmountResizable) {
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
      if constexpr (NodeAmount == kNodeAmountResizable) {
        result_matrix.resize(i_size);
      }
      for (size_t i = 0; i < i_size; ++i) {
        if constexpr (NodeAmount == kNodeAmountResizable) {
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
      if constexpr (NodeAmount == kNodeAmountResizable) {
        result_matrix.resize(i_size);
      }
      for (size_t i = 0; i < i_size; ++i) {
        if constexpr (NodeAmount == kNodeAmountResizable) {
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
      if constexpr (NodeAmount == kNodeAmountResizable) {
        result_matrix.resize(i_size);
      }
      for (size_t i = 0; i < i_size; ++i) {
        if constexpr (NodeAmount == kNodeAmountResizable) {
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
    ContainerType const&
    getCmatrix() const
    {
      return _matrix;
    }

    size_t
    size()
    {
      return _matrix.size();
    }

    ~AdjacencyMatrix()                                 = default;
    AdjacencyMatrix()                                  = default;

    AdjacencyMatrix(AdjacencyMatrix const&)            = default;
    AdjacencyMatrix(AdjacencyMatrix&&)                 = default;

    AdjacencyMatrix& operator=(AdjacencyMatrix const&) = default;
    AdjacencyMatrix& operator=(AdjacencyMatrix&&)      = default;

   private:
    ContainerType _matrix{};
    NameContainerType _matrix_names{};

    size_t
    getEdgesNumber()
    {
      return getEdges().size();
    }
    [[nodiscard]] std::vector<std::pair<size_t, size_t> >
    getEdges() const
    {
      std::vector<std::pair<size_t, size_t> > edges{};
      for (size_t i = 0; i != _matrix.size(); ++i) {
        size_t j = i;
        if constexpr (IsOriented) {
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
      if constexpr (kNodeAmountResizable == NodeAmount) {
        visited.resize(_matrix.size());
      }
      visited[first_node_index] = true;

      WeightsMatrix weights{};

      if constexpr (kNodeAmountResizable == NodeAmount) {
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

  template <size_t NodeAmount2, bool IsOriented, bool IsWeighted,
            typename ValueType2>
  std::ostream&
  operator<<(std::ostream& out,
             AdjacencyMatrix<NodeAmount2, IsOriented, IsWeighted,
                             ValueType2> const& matrix)
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
             std::array<ContainerInside, CNodeAmount> const& matrix)
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
             std::vector<std::vector<ValueType> > const& matrix)
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
  void drawName(std::string_view str, size_t value);
}  // namespace graph_first
