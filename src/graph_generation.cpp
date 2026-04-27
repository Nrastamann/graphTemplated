#include "graph_generation.hpp"

#include <vector>

#include "graph.hpp"

graph_first::ResizableAdjacencyMatrix<false, false, uint8_t>
getBAGraph(size_t node_amount, size_t edges_number, std::mt19937& generator)
{
  graph_first::ResizableAdjacencyMatrix<false, false, uint8_t> result_matrix;

  auto& working_matrix = result_matrix.getMatrix();

  result_matrix.addVertex();
  std::vector<size_t> degree_matrix(node_amount);
  uint64_t total_degree{};

  while (working_matrix.size() < node_amount) {
    result_matrix.addVertex();

    size_t generated_edges = 0;
    size_t current_node    = working_matrix.size() - 1;

    while (generated_edges != edges_number &&
           generated_edges != working_matrix.size() - 1) {
      for (size_t i = 0; i < current_node && generated_edges != edges_number;
           ++i) {
        if (working_matrix.size() - 1 <= edges_number ||
            generator() % kPercentHighest <=
                kPercentHighest * degree_matrix[i] / total_degree) {
          generated_edges++;
          degree_matrix[i]++;
          degree_matrix[current_node]++;
          total_degree += 2;
          working_matrix[i][current_node]++;
          working_matrix[current_node][i]++;
        }
      }
    }
  }

  return result_matrix;
}

static graph_first::ResizableAdjacencyMatrix<false, false, uint8_t>
getCircleMatrix(size_t node_amount, size_t neighbour_number)
{
  graph_first::ResizableAdjacencyMatrix<false, false, uint8_t> result_matrix;

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

graph_first::ResizableAdjacencyMatrix<false, false, uint8_t>
getWSGraph(size_t node_amount, size_t neighbour_number,
           [[maybe_unused]] uint8_t percent,
           [[maybe_unused]] std::mt19937& generator)
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
