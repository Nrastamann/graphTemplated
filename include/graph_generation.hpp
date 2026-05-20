#pragma once
#include <algorithm>
#include <array>
#include <iostream>
#include <print>
#include <random>

#include "graph.hpp"
static constexpr uint8_t kPercentHighest{100};
/*template <size_t N>
graph_first::AdjacencyMatrix<N, false, false>
getERGraph(uint8_t percent, std::mt19937& generator)
{
  graph_first::AdjacencyMatrix<N, false, false> result_matrix;
  auto& working_matrix = result_matrix.getMatrix();

  for (size_t i = 0; i < working_matrix.size(); ++i) {
    for (size_t j = i + 1; j < working_matrix[i].size(); ++j) {
      working_matrix[i][j] = (generator() % kPercentHighest) <= percent ? 1 : 0;
      working_matrix[j][i] = working_matrix[i][j];
    }
  }
  return result_matrix;
}

graph_first::ResizableAdjacencyMatrix<false, false, uint8_t> getBAGraph(
    size_t node_amount, size_t edges_number, std::mt19937& generator);
*/
inline static graph_first::graph_types::ResizableGraph<
    graph_first::graph_flags::kWeighted>
getCircleMatrix(size_t node_amount, size_t neighbour_number)
{
  graph_first::graph_types::ResizableGraph<graph_first::graph_flags::kWeighted>
      result_matrix;

  result_matrix.resize(node_amount);
  auto& working_matrix = result_matrix.getMatrix();

  for (size_t i = 0; i < node_amount; ++i) {
    for (size_t j = 0; j < neighbour_number; ++j) {
      size_t index             = (i + j + 1) % node_amount;
      working_matrix[i][index] = 1;
      working_matrix[index][i] = 1;
    }
  }

  return result_matrix;
}

inline graph_first::graph_types::ResizableGraph<
    graph_first::graph_flags::kWeighted>
getWSGraph(size_t node_amount, size_t neighbour_number, uint8_t percent,
           std::mt19937& generator)
{
  neighbour_number     /= 2;
  auto result_matrix    = getCircleMatrix(node_amount, neighbour_number);
  auto& working_matrix  = result_matrix.getMatrix();

  for (size_t i = 0; i < node_amount; ++i) {
    for (size_t j = 0; j < neighbour_number; ++j) {
      size_t index = (i + j + 1) % node_amount;
      if (generator() % kPercentHighest <= percent) {
        working_matrix[i][index] = 0;
        working_matrix[index][i] = 0;

        index                    = i;
        while (index == i) {
          index = generator() % node_amount;
        }
        working_matrix[i][index] = 1;
        working_matrix[index][i] = 1;
      }
    }
  }

  return result_matrix;
}
template <size_t N>
graph_first::graph_types::ResizableGraph<0>
generateGraph(uint8_t percent, std::mt19937& generator,
              std::array<std::array<size_t, N>, N>& matrix)
{
  graph_first::graph_types::ResizableGraph<0> result_graph;
  constexpr double kDivisor{100. * 4.};
  constexpr size_t kNSquared{N * N};
  constexpr size_t kDummyValue = 65535;
  auto m                       = static_cast<size_t>(
      std::ceil(static_cast<double>(percent * kNSquared) / kDivisor));
  m = std::max(1LU, m);

  uint8_t node_value{0};
  std::ranges::for_each(matrix, [](auto& row) {
    std::ranges::for_each(row, [](auto& value) { value = kDummyValue; });
  });

  while (m-- != 0) {
    size_t i     = generator() % (N - 1);
    size_t j     = generator() % (N - 1);
    matrix[i][j] = matrix[i][j] != kDummyValue ? matrix[i][j] : node_value++;
    matrix[i + 1][j] =
        matrix[i + 1][j] != kDummyValue ? matrix[i + 1][j] : node_value++;
    matrix[i][j + 1] =
        matrix[i][j + 1] != kDummyValue ? matrix[i][j + 1] : node_value++;
    matrix[i + 1][j + 1] = matrix[i + 1][j + 1] != kDummyValue
                               ? matrix[i + 1][j + 1]
                               : node_value++;
  }

  result_graph.resize(static_cast<size_t>(node_value));

  auto populate_graph = [&result_graph, &matrix](size_t i, size_t j,
                                                 size_t i_second,
                                                 size_t j_second) {
    matrix[i][j] != kDummyValue&& matrix[i_second][j_second] != kDummyValue
        ? result_graph.addEdge(matrix[i][j], matrix[i_second][j_second])
        : void();
  };

  for (size_t i = 0; i != N; ++i) {
    for (size_t j = 0; j != N; ++j) {
      j != N - 1 ? populate_graph(i, j, i, j + 1) : void();
      j != 0 ? populate_graph(i, j, i, j - 1) : void();
      i != N - 1 ? populate_graph(i, j, i + 1, j) : void();
      i != 0 ? populate_graph(i, j, i - 1, j) : void();
    }
  }

  return result_graph;
}

template <size_t N>
void
generateRNK(std::mt19937& generator, std::array<uint8_t, N>& rnaTypes)
{
  for (size_t i = 0; i != N - 1; ++i) {
    rnaTypes[i] = generator() % 4;
  }
  rnaTypes[N - 1] = generator() % 4;
}
