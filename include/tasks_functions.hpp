#pragma once
#include <algorithm>
#include <format>
#include <iostream>
#include <numeric>

#include "graph.hpp"

constexpr size_t kNameSize{10};
template <size_t N, bool IsOriented, bool IsWeighted, typename ValueType>
void
task2(
    graph_first::AdjacencyMatrix<N, IsOriented, IsWeighted, ValueType>& matrix,
    std::string_view name)
{
  std::cout << (std::format("\n\n{:=^120}\n\n", name));
  auto const& output = matrix.getCmatrix();
  for (auto const& i : output) {
    for (auto const& j : i) {
      std::cout << +j << ',';
    }
    std::cout << '\n';
  }
  std::cout << "\n\n\n";

  auto degrees = matrix.getDegreeMatrix();
  std::vector<size_t> degrees_vec;
  degrees_vec.reserve(degrees.size());

  for (size_t i = 0; i < degrees.size(); ++i) {
    degrees_vec.push_back(degrees[i][i]);
  }
  std::cout << std::format(
      "avg degree:\n{}",
      std::accumulate(degrees_vec.begin(), degrees_vec.end(), 0.) /
          static_cast<double>(matrix.size()));

  auto bfs_list = matrix.bfs();
  bfs_list = matrix.bfs(static_cast<size_t>(std::ranges::max_element(bfs_list) -
                                            bfs_list.begin()));
  std::cout << std::format("\nclusterization - {}", matrix.getClasterization());

  std::cout << std::format("\ndiameter - {}\n",
                           *std::ranges::max_element(bfs_list));

  std::cout << "Average path - " << matrix.averagePath() << '\n';
  std::cout << "Density - " << matrix.getDensity() << "\n\n";
  std::cout << "Components - " << matrix.getClusters() << "\n\n";
  std::cout << "Gigantic component - " << matrix.getBiggestCluster() << "\n\n";
}
