#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <print>
#include <queue>
#include <random>

#include <cstdint>
#include <ctime>
#include <expected>
#include <unordered_map>
#include <vector>

#include <glad/gl.h>

#include <GLFW/glfw3.h>
#include "GraphVisualisation.hpp"
#include "graph.hpp"
#include "graph_generation.hpp"
#include "linear_algrebra.hpp"

using return_type = std::expected<uint32_t, bool>;
namespace {

  constexpr float kRed{1.00F};
  constexpr float kGreen{1.0F};
  constexpr float kBlue{1.0F};
  constexpr float kAlpha{1.0F};
  constexpr bool kRenderOn{true};
  constexpr size_t kRandomseed{10};
  //  constexpr bool kHexagon{true};
  //    constexpr float kFov{45.0F};
  //    constexpr float kNear{0.1F};
  //    constexpr float kFar{10.0F};
}  // namespace
struct GraphMovement {
  size_t _idx{};
  bool _pressed{};
};

static void
updatePosition(math::vec2F position, size_t index,
               visual::GraphRenderer& renderer)
{
  renderer.setPosition(index, position);
  renderer.updateEdge(index);
}

static void
mousePressed(math::vec2F& pos, GraphMovement& metadata,
             visual::GraphRenderer& renderer)
{
  metadata._pressed = true;
  metadata._idx     = renderer.getNearest(pos);
}

void
mouseEvents(GLFWwindow* window, GraphMovement& metadata,
            visual::GraphRenderer& renderer)
{
  std::integral auto state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

  double x{};
  double y{};
  glfwGetCursorPos(window, &x, &y);
  math::vec2F position{static_cast<float>(x), static_cast<float>(1080. - y)};

  metadata._pressed = false;

  switch (state) {
    case GLFW_PRESS:
      if (!metadata._pressed) {
        mousePressed(position, metadata, renderer);
      }
      updatePosition(position, metadata._idx, renderer);
      break;
    case GLFW_RELEASE:
      metadata._pressed = false;
    default:
      return;
  }
}
static void
renderGraphVertexesColoured(visual::GraphRenderer& renderer, auto& matrix,
                            const std::vector<size_t>& colours)
{
  const auto& matrix_inner = matrix.getContainer();
  std::set<size_t> vertexes;
  for (auto& i : matrix_inner) {
    vertexes.insert(i._startNode);
    vertexes.insert(i._endNode);
  }

  renderer.use(visual::ShaderTypes::Circle);

  renderer.resize(vertexes.size());

  std::mt19937 gen(kRandomseed);

  size_t idx = vertexes.size();

  // auto clusters = matrix.getClusters({});

  // std::println("{}", matrix.getClusters({}));

  std::unordered_map<size_t, float> colours_render;

  for (auto i : colours) {
    if (colours_render.contains(i)) {
      continue;
    }

    colours_render.emplace(i, static_cast<float>(gen() % 10000 / 10000.));
  }
  std::println("{}", colours_render);
  /*
    for (auto& cluster : clusters) {
      math::vec3F colour = {static_cast<float>(gen() % 100 / 100.),
                            static_cast<float>(gen() % 100 / 100.),
                            static_cast<float>(gen() % 100 / 100.)};
      for (auto idx : cluster) {
        renderer.setColour(idx, colour);
      }
    }*/

  for (size_t i = 0; i < vertexes.size(); ++i) {
    math::vec2F position{80., 80.};
    renderer.setPosition(
        i,
        {std::max(
             std::min(position[0] * static_cast<float>(i * std::cos(i)) + 80.F,
                      1920.F),
             80.F),
         std::max(
             std::min(position[1] * static_cast<float>(i * std::sin(i)) + 80.F,
                      1080.F),
             80.F)});
    renderer.setColour(
        i, {colours_render[colours[i]], colours_render[colours[i]], 0.0F});
  }

  for (size_t i = 0; i < matrix_inner.size(); ++i) {
    renderer.addNode();
    renderer.setLine(idx++, matrix_inner[i]._startNode,
                     matrix_inner[i]._endNode);
  }
}

static void
renderGraphEdges(visual::GraphRenderer& renderer, auto& matrix,
                 const std::vector<size_t>& colours)
{
  const auto& matrix_inner = matrix.getContainer();
  std::set<size_t> vertexes;
  for (auto& i : matrix_inner) {
    vertexes.insert(i._startNode);
    vertexes.insert(i._endNode);
  }

  renderer.use(visual::ShaderTypes::Circle);

  renderer.resize(vertexes.size());

  std::mt19937 gen(kRandomseed);

  size_t idx = vertexes.size();

  // auto clusters = matrix.getClusters({});

  // std::println("{}", matrix.getClusters({}));

  std::unordered_map<size_t, float> colours_render;

  for (auto i : colours) {
    if (colours_render.contains(i)) {
      continue;
    }

    colours_render.emplace(i, static_cast<float>(gen() % 10000 / 10000.));
  }
  std::println("{}", colours_render);
  /*
    for (auto& cluster : clusters) {
      math::vec3F colour = {static_cast<float>(gen() % 100 / 100.),
                            static_cast<float>(gen() % 100 / 100.),
                            static_cast<float>(gen() % 100 / 100.)};
      for (auto idx : cluster) {
        renderer.setColour(idx, colour);
      }
    }*/

  for (size_t i = 0; i < vertexes.size(); ++i) {
    math::vec2F position{80., 80.};
    renderer.setPosition(
        i,
        {std::max(
             std::min(position[0] * static_cast<float>(i * std::cos(i)) + 80.F,
                      1920.F),
             80.F),
         std::max(
             std::min(position[1] * static_cast<float>(i * std::sin(i)) + 80.F,
                      1080.F),
             80.F)});
  }
  constexpr std::array<std::pair<float, float>, 10> kColours{
      {
       {0.0F, 1.0F},
       {0.0F, 0.5F},
       {0.0F, 0.0F},
       {0.5F, 0.0F},
       {1.0F, 0.0F},
       {1.0F, 1.0F},
       {0.75F, 0.75F},
       {.50F, 0.5F},
       {0.25F, 0.25F},
       {.1F, 0.1F},

       },
  };
  for (size_t i = 0; i < matrix_inner.size(); ++i) {
    std::cout << matrix_inner[i]._startNode << ' ' << matrix_inner[i]._endNode
              << '\n';
    renderer.addNode();

    renderer.setLine(idx, matrix_inner[i]._startNode, matrix_inner[i]._endNode);
    renderer.setColour(
        idx++,
        math::vec3F{kColours[colours[i] % kColours.size()].first,
                    kColours[colours[i] % kColours.size()].second, 0.0F});
  }
}

static void
renderGraph(visual::GraphRenderer& renderer, auto& matrix, auto& colours)
{
  const auto& matrix_inner = matrix.getContainer();

  renderer.use(visual::ShaderTypes::Circle);

  renderer.resize(matrix.size());

  std::mt19937 gen(kRandomseed);

  size_t idx = 0;

  std::array<std::pair<float, float>, 4> colours_rna{};

  for (auto& rna : colours_rna) {
    rna.first  = static_cast<float>(gen() % 1000 / 1000.);
    rna.second = static_cast<float>(gen() % 1000 / 1000.);
  }

  for (auto rna_type : colours) {
    renderer.setColour(idx++, math::vec3F{colours_rna[rna_type].first,
                                          colours_rna[rna_type].second, 1.0F});
  }

  for (size_t i = 0; i < matrix_inner.size(); ++i) {
    math::vec2F position{80., 80.};
    renderer.setPosition(
        i,
        {std::max(
             std::min(position[0] * static_cast<float>(static_cast<double>(i) *
                                                       std::cos(i)) +
                          80.F,
                      1920.F),
             80.F),
         std::max(
             std::min(position[1] * static_cast<float>(static_cast<double>(i) *
                                                       std::sin(i)) +
                          80.F,
                      1080.F),
             80.F)});
  }

  for (size_t i = 0; i < matrix_inner.size(); ++i) {
    for (size_t j = i; j < matrix_inner[i].size(); ++j) {
      if (matrix_inner[i][j] == 0) {
        continue;
      }
      renderer.addNode();
      renderer.setLine(idx++, i, j);
    }
  }
}
/*
static void
generateWS(visual::GraphRenderer& renderer)
{
  std::mt19937 gen(kRandomseed);

  graph_first::graph_types::ResizableGraph<graph_first::graph_flags::kWeighted>
      matrix = getWSGraph(30, 2, 50, gen);

  renderGraph(renderer, matrix);
}
static graph_first::graph_types::ResizableGraph<
    graph_first::graph_flags::kEmpty>
generateHex(size_t M)
{
  graph_first::graph_types::ResizableGraph<graph_first::graph_flags::kEmpty>
      matrix = {};
  matrix.resize(M * M);

  for (size_t i = 0; i < M; ++i) {
    for (size_t j = 0; j < M; ++j) {
      size_t v = ((i * M) + j);
      size_t r = v / M;
      size_t c = v % M;
      if ((r % 4 == 0 && (c & 1) == 1) || (r % 4 == 2 && (c & 1) == 0)) {
        if (r > 0) {
          matrix.addEdge(v, v - M);
          matrix.addEdge(v - M, v);
        }
        if (r < (M - 1) && c < (M - 1)) {
          matrix.addEdge(v + M + 1, v);
          matrix.addEdge(v, v + M + 1);
        }
        if (r < (M - 1) && c > 0) {
          matrix.addEdge(v + M - 1, v);
          matrix.addEdge(v, v + M - 1);
        }
      }
    }
  }

  std::set<size_t, std::greater<>> idxes;
  size_t c_i = 0;
  for (const auto& arr : matrix.getContainer()) {
    size_t test = 0;
    for (auto i : arr) {
      if (i != 0) {
        test++;
      }
    }
    if (test < 2) {
      idxes.insert(c_i);
    }
    c_i++;
  }

  for (auto i : idxes) {
    matrix.removeVertex(i);
  }

  return matrix;
}

static void
benchmark()
{
  for (size_t i = 3; i < 10; ++i) {
    auto matrix = generateHex(i);
    std::println("Matrix {}x{}", i, i);
    double time_elapsed{};
    constexpr size_t kBenchN{1000000};
    for (size_t j = 0; j < kBenchN; ++j) {
      auto time     = std::chrono::steady_clock::now();
      auto res      = matrix.getClusters({});
      auto duration = std::chrono::duration<double>{
          std::chrono::steady_clock::now() - time};
      time_elapsed += duration.count();
    }
    std::println("RESULT: {}", time_elapsed / kBenchN);
    std::print("\n\n");
  }
}
*/
/*
static graph_first::Graph<graph_first::graph_types::kNodeAmountResizable, 0,
                          uint8_t, graph_first::EdgesListTag>
generateEdges1()
{
  graph_first::Graph<graph_first::graph_types::kNodeAmountResizable, 0, uint8_t,
                     graph_first::EdgesListTag>
      matrix{};
  matrix.addEdge(0, 1);
  matrix.addEdge(0, 2);
  matrix.addEdge(0, 3);
  matrix.addEdge(1, 2);
  matrix.addEdge(1, 3);
  matrix.addEdge(2, 3);
  return matrix;
}
*//*
static graph_first::Graph<graph_first::graph_types::kNodeAmountResizable,
                          graph_first::graph_flags::kWeighted, uint8_t,
                          graph_first::EdgesListTag>
    
    generateEdges2()
    {
      graph_first::Graph<graph_first::graph_types::kNodeAmountResizable,
                         graph_first::graph_flags::kWeighted, uint8_t,
                         graph_first::EdgesListTag>
          matrix{};
      matrix.addEdge(0, 6, 2);
      matrix.addEdge(0, 3, 3);
      matrix.addEdge(0, 4, 5);

      matrix.addEdge(1, 2, 5);
      matrix.addEdge(1, 3, 4);
      matrix.addEdge(1, 5, 4);
      matrix.addEdge(1, 6, 6);

      matrix.addEdge(2, 3, 7);

      matrix.addEdge(3, 4, 9);

      matrix.addEdge(5, 6, 6);

      return matrix;
    }
    */
static graph_first::Graph<graph_first::graph_types::kNodeAmountResizable,
                          graph_first::graph_flags::kWeighted |
                              graph_first::graph_flags::kOriented,
                          uint8_t, graph_first::EdgesListTag>
generateEdges3()
{
  graph_first::Graph<graph_first::graph_types::kNodeAmountResizable,
                     graph_first::graph_flags::kWeighted |
                         graph_first::graph_flags::kOriented,
                     uint8_t, graph_first::EdgesListTag>
      matrix{};
  matrix.addEdge(0, 6, 2);
  matrix.addEdge(0, 3, 3);
  matrix.addEdge(4, 0, 5);

  matrix.addEdge(2, 1, 5);
  matrix.addEdge(3, 1, 4);
  matrix.addEdge(1, 5, 4);
  matrix.addEdge(1, 6, 6);

  matrix.addEdge(3, 2, 7);

  matrix.addEdge(4, 3, 9);
  matrix.addEdge(3, 0, 3);
  matrix.addEdge(6, 1, 6);
  matrix.addEdge(6, 5, 6);

  return matrix;
}
graph_first::Graph<graph_first::graph_types::kNodeAmountResizable,
                   graph_first::graph_flags::kOriented, uint8_t,
                   graph_first::EdgesListTag>
generateEdges4(std::span<std::array<uint8_t, 10>> arr)
{
  graph_first::Graph<graph_first::graph_types::kNodeAmountResizable,
                     graph_first::graph_flags::kOriented, uint8_t,
                     graph_first::EdgesListTag>
      matrix{};

  for (size_t i = 0; i < arr.size(); ++i) {
    for (size_t j = i; j < arr.size(); ++j) {
      if (arr[i][j] != 1) {
        continue;
      }
      matrix.addEdge(i, j);
    }
  }
  return matrix;
}

std::array<std::array<uint8_t, 10>, 10>
getRaw()
{
  return {
      {
       {0, 1, 1, 0, 0, 0, 1, 0, 1, 0},
       {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
       {1, 0, 0, 1, 0, 0, 1, 1, 1, 0},
       {0, 0, 1, 0, 0, 0, 1, 1, 0, 0},
       {0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
       {0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
       {1, 0, 1, 1, 0, 0, 0, 0, 0, 0},
       {0, 0, 1, 1, 1, 0, 0, 0, 1, 0},
       {1, 0, 1, 0, 1, 1, 0, 1, 0, 0},
       {0, 1, 0, 0, 1, 1, 0, 0, 0, 0},
       }
  };
}

void
pathesSorted()
{
  auto graph1  = generateEdges3();

  auto pathes  = graph1.getAllPathes(4, 5);

  auto colours = graph1.colorEdges();
  std::println("{} - colours", colours);
  std::vector<std::vector<size_t>> edges(pathes.size());
  std::vector<size_t> costs(pathes.size());

  std::vector<std::pair<size_t, std::vector<size_t>>> result(graph1.size());
  auto jt = pathes.begin();

  for (auto& res : result) {
    size_t idx = static_cast<size_t>(jt - pathes.begin());
    costs[idx] = graph1.processPath(*jt, edges[idx]);

    res.first  = costs[idx];
    res.second = std::move(edges[idx]);
    std::advance(jt, 1);
  }

  std::ranges::sort(
      result, [](const auto& a, const auto& b) { return a.first <= b.first; });

  for (auto& i : result) {
    std::println("{} - {} cost", i.second, i.first);
  }

  auto random_colour_it    = result.end();
  const auto shortest_path = result.begin();

  for (auto candidate_it = result.begin(); candidate_it != result.end();
       ++candidate_it) {
    std::set<size_t> picked_colours;
    bool random_colours = true;
    for (auto edge_idx : candidate_it->second) {
      if (picked_colours.contains(colours[edge_idx])) {
        random_colours = false;
        break;
      }
      picked_colours.insert(colours[edge_idx]);
    }
    if (random_colours) {
      random_colour_it = candidate_it;
      break;
    }
  }
  if (result.size() != 0) {
    std::println("Shortest path is - {} with cost - {}", shortest_path->second,
                 shortest_path->first);
  }
  if (random_colour_it != result.end()) {
    std::println(
        "Shortest path with all different colours is - {} with cost - {}",
        random_colour_it->second, random_colour_it->first);
  }
  else {
    std::println("Path with all different colours is not found");
  }
}
void
degreesCheck()
{
  std::mt19937 gen(static_cast<uint64_t>(
      std::chrono::high_resolution_clock::now().time_since_epoch().count()));
  uint8_t percent = 0;
  std::array<std::array<size_t, 10>, 10> mtx{};
  while (percent != 105) {
    std::unordered_map<size_t, size_t> degrees;
    auto result = generateGraph<10>(percent, gen, mtx).getDegreeMatrix();
    for (size_t i = 0; i < result.size(); ++i) {
      degrees[result[i][i]]++;
    }
    for (size_t i = 0; i < 10; ++i) {
      if (degrees.contains(i)) {
      }
      //   }
      //    std::cout << "\n";
      percent += 5;
    }
  }
}

static bool
bfs(const std::array<std::array<size_t, 10>, 10>& _matrix)
{
  std::queue<size_t> q;

  constexpr size_t kDummyValue = 65535;
  size_t start{};
  for (const auto& i : _matrix) {
    if (i[0] != kDummyValue) {
      q.push(start * 10);
    }
    start++;
  }

  if (q.empty()) {
    return false;
  }

  start *= 10;

  std::array<std::array<bool, 10>, 10> visited{};

  while (!q.empty()) {
    size_t current = q.front();
    q.pop();

    size_t r      = current / 10;
    size_t c      = current % 10;

    visited[r][c] = true;

    if (r > 0 && !visited[r - 1][c]) {
      _matrix[r - 1][c] != kDummyValue ? q.push((r - 1) * 10 + c) : void();
    }

    if (r + 1 != 10 && !visited[r + 1][c]) {
      _matrix[r + 1][c] != kDummyValue ? q.push((r + 1) * 10 + c) : void();
    }
    if (c + 1 != 10 && !visited[r][c + 1]) {
      _matrix[r][c + 1] != kDummyValue ? q.push(r * 10 + c + 1) : void();
    }
    if (c > 0 && !visited[r][c - 1]) {
      _matrix[r][c - 1] != kDummyValue ? q.push(r * 10 + c - 1) : void();
    }
  }

  for (size_t i = 0; i != 10; ++i) {
    if (visited[i][9]) {
      return true;
    }
  }

  return false;
}

void
secondPart()
{
  std::mt19937 gen(static_cast<uint64_t>(
      std::chrono::high_resolution_clock::now().time_since_epoch().count()));
  uint8_t percent = 0;
  std::array<std::array<size_t, 10>, 10> mtx;
  std::unordered_map<uint8_t, std::vector<bool>> results;

  std::ofstream file("output.txt");
  if (!file.is_open()) {
    std::cout << "COULDN'T OPEN FILE\n";
  }

  while (percent != 105) {
    size_t i = 0;
    while (i++ != 10000) {
      generateGraph<10>(percent, gen, mtx);
      results[percent].push_back(bfs(mtx));
    }

    size_t count_true{};
    std::ranges::for_each(results[percent],
                          [&count_true](auto value) { count_true += value; });

    file << +percent << ' ' << count_true << ' '
         << results[percent].size() - count_true << '\n';
    percent += 5;
  }
  file.close();
}

enum class RNKType : uint8_t {
  A = 0,
  U = 1,
  G = 2,
  C = 3
};

template <size_t N>
static graph_first::graph_types::ResizableGraph<0>
nussinov(const std::array<uint8_t, N>& rna_types)
{
  auto compatability_check = [](RNKType type1, RNKType type2) {
    if ((type1 == RNKType::A && type2 == RNKType::U) ||
        (type1 == RNKType::U && type2 == RNKType::A)) {
      return true;
    }
    if ((type1 == RNKType::G && type2 == RNKType::C) ||
        (type1 == RNKType::C && type2 == RNKType::G)) {
      return true;
    }
    return false;
  };

  std::array<std::array<size_t, N>, N> result_matrix{};
  for (size_t k = 1; k != N; ++k) {
    for (size_t i = 0; i != N - k; ++i) {
      size_t j = i + k;
      size_t bifurc{};
      for (size_t k4 = i + 1; k4 < j; ++k4) {
        bifurc =
            std::max(bifurc, result_matrix[i][k4] + result_matrix[k4 + 1][j]);
      }

      result_matrix[i][j] = std::max(
          std::max(result_matrix[i + 1][j], result_matrix[i][j - 1]),
          std::max(result_matrix[i + 1][j - 1] +
                       compatability_check(static_cast<RNKType>(rna_types[i]),
                                           static_cast<RNKType>(rna_types[j])),
                   bifurc));
    }
  }

  std::vector<std::pair<size_t, size_t>> pairs;
  std::queue<std::pair<size_t, size_t>> queue;
  std::array<std::array<bool, N>, N> visited{};

  queue.emplace(0, N - 1);

  while (!queue.empty()) {
    size_t i = queue.front().first;
    size_t j = queue.front().second;

    queue.pop();
    if (i >= j) {
      continue;
    }
    visited[i][j] = true;
    if (result_matrix[i][j] == result_matrix[i][j - 1]) {
      j -= 1;
    }

    for (size_t k = i + 1; k < j; ++k) {
      if (visited[k][j] ||
          !compatability_check(static_cast<RNKType>(rna_types[k]),
                               static_cast<RNKType>(rna_types[j]))) {
        continue;
      }

      if (result_matrix[i][j] !=
          (result_matrix[i][k - 1] + result_matrix[k + 1][j - 1] + 1)) {
        continue;
      }
      pairs.emplace_back(k, j);
      !visited[i][k - 1] ? queue.emplace(i, k - 1)
                         : std::pair<size_t, size_t>{};
      !visited[k + 1][j - 1] ? queue.emplace(k + 1, j - 1)
                             : std::pair<size_t, size_t>{};
    }
  }
  graph_first::graph_types::ResizableGraph<0> result_graph;
  result_graph.resize(N);
  for (auto& edge : pairs) {
    result_graph.addEdge(edge.first, edge.second);
  }
  for (size_t i = 0; i != N - 1; ++i) {
    result_graph.addEdge(i, i + 1);
  }
  return result_graph;
}

int
main()
{
  // degreesCheck();
  //  secondPart();
  std::mt19937 gen(static_cast<uint64_t>(
      std::chrono::high_resolution_clock::now().time_since_epoch().count()));

  std::array<uint8_t, 15> rna_types{};

  generateRNK(gen, rna_types);
  std::println("{}", rna_types);
  auto graph1 = nussinov(rna_types);

  if constexpr (kRenderOn) {
    GLFWwindow* window = nullptr;
    if (glfwInit() == 0) {
      std::cout << "GLFW couldn't init window\n";
      return -1;
    }

    window = glfwCreateWindow(visual::kWidth, visual::kHeight, "GraphsLabs",
                              nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (gladLoadGL(glfwGetProcAddress) == 0) {
      std::cout << "Couldn't load opengl\n";
      glfwTerminate();
      return -1;
    }

    // Set up OpenGL
    glClearColor(kRed, kGreen, kBlue, kAlpha);
    // Set the rendering region to the actual screen size
    int w = 0;
    int h = 0;

    glfwGetFramebufferSize(window, &w, &h);
    //(left, top, width, height)
    glViewport(0, 0, w, h);

    auto renderer_res = visual::GraphRenderer::createGraph(0);

    if (!renderer_res.has_value()) {
      return -1;
    }

    auto& renderer = renderer_res.value();

    renderer.use(visual::ShaderTypes::Circle);

    renderGraph(renderer, graph1, rna_types);

    math::vec2F resolution = {static_cast<float>(w), static_cast<float>(h)};

    constexpr float kBorderThick = {10.F};
    constexpr float kRadius      = {40.0F};

    renderer.setResolution(resolution);
    renderer.setBorderThickness(kBorderThick);
    renderer.setRadius(kRadius);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    renderer.setResolution(resolution);

    GraphMovement metadata{};

    while (glfwWindowShouldClose(window) == 0) {
      glfwPollEvents();
      glClear(GL_COLOR_BUFFER_BIT);
      renderer.draw();

      glfwSwapBuffers(window);
      mouseEvents(window, metadata, renderer);
    }

    glfwTerminate();
  }
  return 0;
}
