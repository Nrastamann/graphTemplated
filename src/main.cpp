#include <random>

#include "graph_generation.hpp"
#include "tasks_functions.hpp"

constexpr size_t kN{60};

constexpr size_t kProbability{7};
constexpr size_t kEdgesM{1};
constexpr size_t kNeighbourNumber{4};
constexpr size_t kProbabilityWS{5};

/*
        static graph_first::ResizableAdjacencyMatrix<false, false, uint8_t>
getMatrix(std::string_view str)
{
  graph_first::AdjacencyMatrix<0, false, false, uint8_t> matrix;
  matrix.resize(kN);
  auto& working_matrix = matrix.getMatrix();
  for (size_t i = 0; i < kN; ++i) {
    for (size_t j = 0; j < kN; ++j) {
      std::string_view number = str.substr(0, 1);
      auto [ptr, ec] =
          std::from_chars(number.begin(), number.end(), working_matrix[i][j]);

      if (ec != std::errc() || ptr != number.end()) {
        std::cout << "FUCK\n";
      }

      str.remove_prefix(2);
    }
  }

  return matrix;
}
*/
int
main()
{
  std::random_device r;

  std::seed_seq seed{r(), r(), r(), r(), r(), r(), r(), r()};
  std::mt19937 generator(seed);
  auto matrix_er = getERGraph<kN>(kProbability, generator);
  task2(matrix_er, "ER - GRAPH");

  auto matrix_ba = getBAGraph(kN, kEdgesM, generator);
  task2(matrix_ba, "BA - GRAPH");

  auto matrix_ws = getWSGraph(kN, kNeighbourNumber, kProbabilityWS, generator);
  task2(matrix_ws, "WS - GRAPH");
}
