#include <cmath>
#include <print>
#include <random>

#include <cstdint>
#include <ctime>
#include <expected>

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

  constexpr size_t kRandomseed{228322};
  constexpr bool kHexagon{true};
  //  constexpr float kFov{45.0F};
  //  constexpr float kNear{0.1F};
  //  constexpr float kFar{10.0F};
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
renderGraph(visual::GraphRenderer& renderer, auto& matrix)
{
  const auto& matrix_inner = matrix.getContainer();

  renderer.use(visual::ShaderTypes::Circle);

  renderer.resize(matrix.size());

  std::mt19937 gen(kRandomseed);

  size_t idx    = matrix.size();

  auto clusters = matrix.getClusters({});

  std::println("{}", matrix.getClusters({}));

  for (auto& cluster : clusters) {
    math::vec3F colour = {static_cast<float>(gen() % 100 / 100.),
                          static_cast<float>(gen() % 100 / 100.),
                          static_cast<float>(gen() % 100 / 100.)};
    for (auto idx : cluster) {
      renderer.setColour(idx, colour);
    }
  }

  for (size_t i = 0; i < matrix_inner.size(); ++i) {
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

int
main()
{
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

  if (kHexagon) {
    // benchmark();
    auto matrix = generateHex(6);
    renderGraph(renderer, matrix);

    for (size_t i = 0; i < matrix.size(); ++i) {
      renderer.setPosition(i, {i / 4 * 80.F + 960.F, i % 4 * 80.F + 540.F});
      renderer.updateEdge(i);
    }
  }
  else {
    generateWS(renderer);
  }

  math::vec2F resolution       = {static_cast<float>(w), static_cast<float>(h)};

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
  return 0;
}
