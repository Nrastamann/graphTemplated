#pragma once
#include <random>

#include "graph.hpp"
static constexpr uint8_t kPercentHighest{100};
template <size_t N>
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

graph_first::ResizableAdjacencyMatrix<false, false, uint8_t> getWSGraph(
    size_t node_amount, size_t neighbour_number, uint8_t percent,
    std::mt19937& generator);
