#pragma once
#include <random>

#include "graph.hpp"
static constexpr uint8_t kPercentHighest{100};
template <size_t N>
GraphFirst::AdjacencyMatrix<N, false, false>
getERGraph(uint8_t percent, std::mt19937& generator)
{
  GraphFirst::AdjacencyMatrix<N, false, false> result_matrix;
  auto& working_matrix = result_matrix.get_matrix();
  for (size_t i = 0; i < working_matrix.size(); ++i) {
    for (size_t j = i; j < working_matrix[i].size(); ++j) {
      working_matrix[i][j] = (generator() % kPercentHighest) < percent ? 1 : 0;
      working_matrix[j][i] = working_matrix[i][j];
    }
  }
  return result_matrix;
}
