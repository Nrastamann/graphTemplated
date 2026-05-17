#pragma once
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
