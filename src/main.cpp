#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <ctime>
#include <expected>
#include <iostream>
#include <iterator>
#include <numbers>
#include <print>

#include <glad/gl.h>
#include "GraphVisualisation.hpp"

#include <GLFW/glfw3.h>
#include "graph.hpp"
#include "linear_algrebra.hpp"
using return_type = std::expected<uint32_t, bool>;
namespace {
  constexpr float kRed{1.00F};
  constexpr float kGreen{1.0F};
  constexpr float kBlue{1.0F};
  constexpr float kAlpha{1.0F};
  constexpr size_t kNumber{100};
  constexpr size_t kDataSize{7};
  //  constexpr float kFov{45.0F};
  //  constexpr float kNear{0.1F};
  //  constexpr float kFar{10.0F};
  constexpr bool benchmark{false};
}  // namespace

int
main()
{
  if constexpr (!benchmark) {
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

    math::vec2F resolution = {static_cast<float>(w), static_cast<float>(h)};
    constexpr float kBorderThick = {10.F};

    float k_radius               = {80.0F};

    renderer.setResolution(resolution);
    renderer.setBorderThickness(kBorderThick);
    renderer.setRadius(k_radius);

    graph_first::AdjacencyMatrix<8, true, true, uint8_t,
                                 graph_first::AdjacencyListTag>
        matrix;

    matrix.addEdge(0, 1, 2);
    matrix.addEdge(7, 0, 9);
    matrix.addEdge(2, 0, 5);
    matrix.addEdge(1, 2, 4);
    matrix.addEdge(1, 7, 4);
    matrix.addEdge(1, 3, 34);
    matrix.addEdge(2, 3, 5);
    matrix.addEdge(3, 4, 6);
    matrix.addEdge(3, 5, 6);
    matrix.addEdge(5, 6, 7);
    matrix.addEdge(6, 7, 8);
    matrix.addEdge(6, 1, 2);

    double k_angle_rotation{(360. / matrix.size() * (std::numbers::pi / 180.))};
    static constexpr double kRadiusToOutside{500.};

    const auto& matrix_container = matrix.getContainer();
    size_t idx{};

    renderer.use(visual::ShaderTypes::Circle);

    renderer.resize(matrix_container.size());
    for (; idx != matrix_container.size(); ++idx) {
      renderer.setPosition(
          idx,
          {static_cast<float>((resolution[0] / 2) +
                              (kRadiusToOutside * cos(static_cast<double>(idx) *
                                                      k_angle_rotation))),

           static_cast<float>((resolution[1] / 2) +
                              (kRadiusToOutside * sin(static_cast<double>(idx) *
                                                      k_angle_rotation)))});
    }

    for (const auto* it = matrix_container.begin();
         it != matrix_container.end(); std::advance(it, 1)) {
      for (const auto* jt = it->begin(); jt != it->end(); std::advance(jt, 1)) {
        if (*jt == 0) {
          continue;
        }
        renderer.addNode();
        auto position_start =
            renderer.getPoint(static_cast<size_t>(matrix_container.end() - it));
        auto position_end =
            renderer.getPoint(static_cast<size_t>(it->end() - jt));

        renderer.setLine(idx++, {position_start[0], position_start[1],
                                 position_end[0], position_end[1]});
      }
    }

    for (size_t i = 0; i < idx; ++i) {
      renderer.printMeshState(i);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    renderer.setResolution(resolution);

    while (glfwWindowShouldClose(window) == 0) {
      glfwPollEvents();
      glClear(GL_COLOR_BUFFER_BIT);
      renderer.draw();

      glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
  }
  else {
    graph_first::AdjacencyMatrix<8, true, true, uint8_t,
                                 graph_first::AdjacencyListTag>
        matrix;

    matrix.addEdge(0, 1, 2);
    matrix.addEdge(7, 0, 9);
    matrix.addEdge(2, 0, 5);
    matrix.addEdge(1, 2, 4);
    matrix.addEdge(1, 7, 4);
    matrix.addEdge(1, 3, 34);
    matrix.addEdge(2, 3, 5);
    matrix.addEdge(3, 4, 6);
    matrix.addEdge(3, 5, 6);
    matrix.addEdge(5, 6, 7);
    matrix.addEdge(6, 7, 8);
    matrix.addEdge(6, 1, 2);

    matrix.addName("Nikolai II", 0);
    matrix.addName("Petr III", 1);
    matrix.addName("Ekaterina II", 2);
    matrix.addName("Pavel I", 3);
    matrix.addName("Aleksandr I", 4);
    matrix.addName("Nikolai I", 5);
    matrix.addName("Aleksandr II", 6);
    matrix.addName("Aleksandr III", 7);

    std::cout << "MATRIX\n";

    for (size_t i = 0; i < matrix.size(); ++i) {
      std::cout << matrix.getNames()[i] << '\t';
      const auto& _matrix = matrix.getContainer();
      for (unsigned char j : _matrix[i]) {
        std::cout << '\t' << static_cast<uint64_t>(j) << ' ';
      }
      std::cout << '\n';
    }
    std::cout << '\n';

    std::cout << "ALL Neighbours\n";
    auto res = matrix.getNeighbours(0);
    for (auto& i : res) {
      std::println("{}", i);
    }

    std::cout << "All Edges\n";

    auto bfs_res = matrix.bfsEdges(0);
    std::cout << bfs_res.size() << '\n';
    for (auto& i : bfs_res) {
      std::println("{}", i);
    }

    std::cout << "VERTEX BIGGER THAN 15\n";
    auto res_big = matrix.getBigVertexes(15);
    for (auto& i : res_big) {
      std::println("{}", i);
    }
    std::cout << "IS THERE CHAIN?\n";

    std::vector<size_t> path_wrong   = {5, 6, 7, 1};
    std::vector<size_t> path_correct = {1, 3, 5, 6};

    std::println("INCORRECT CHAIN - {}, RESULT: {}", path_wrong,
                 matrix.isThereChain(path_wrong));
    std::println("CORRECT CHAIN - {}, RESULT: {}", path_correct,
                 matrix.isThereChain(path_correct));

    //===================
    std::println("\n\n\n\n\n\n\n\nEDGELIST");
    graph_first::AdjacencyMatrix<graph_first::kNodeAmountResizable, true, true,
                                 uint8_t, graph_first::EdgesListTag>
        matrix_edges;

    matrix_edges.addEdge(0, 1, 2);
    matrix_edges.addEdge(7, 0, 9);
    matrix_edges.addEdge(2, 0, 5);
    matrix_edges.addEdge(1, 2, 4);
    matrix_edges.addEdge(1, 7, 4);
    matrix_edges.addEdge(1, 3, 34);
    matrix_edges.addEdge(2, 3, 5);
    matrix_edges.addEdge(3, 4, 6);
    matrix_edges.addEdge(3, 5, 6);
    matrix_edges.addEdge(5, 6, 7);
    matrix_edges.addEdge(6, 7, 8);
    matrix_edges.addEdge(6, 1, 2);

    matrix_edges.addName("Nikolai II", 0);
    matrix_edges.addName("Petr III", 1);
    matrix_edges.addName("Ekaterina II", 2);
    matrix_edges.addName("Pavel I", 3);
    matrix_edges.addName("Aleksandr I", 4);
    matrix_edges.addName("Nikolai I", 5);
    matrix_edges.addName("Aleksandr II", 6);
    matrix_edges.addName("Aleksandr III", 7);

    std::cout << "MATRIX\n";

    const auto& _matrix_edges = matrix_edges.getContainer();
    for (size_t i = 0; i < matrix_edges.size(); ++i) {
      std::cout << _matrix_edges[i]._startNode << ' '
                << _matrix_edges[i]._endNode << ' '
                << static_cast<uint64_t>(_matrix_edges[i]._value) << '\n';
    }
    for (auto& names : matrix_edges.getNames()) {
      std::cout << names << '\n';
    }

    std::cout << '\n';

    std::cout << "ALL Neighbours\n";
    res = matrix_edges.getNeighbours(0);
    for (auto& i : res) {
      std::println("{}", i);
    }

    std::cout << "All Edges\n";

    bfs_res = matrix_edges.bfsEdges(0);
    std::cout << bfs_res.size() << '\n';
    for (auto& i : bfs_res) {
      std::println("{}", i);
    }

    std::cout << "VERTEX BIGGER THAN 15\n";
    res_big = matrix_edges.getBigVertexes(15);
    for (auto& i : res_big) {
      std::println("{}", i);
    }
    std::cout << "IS THERE CHAIN?\n";

    std::println("INCORRECT CHAIN - {}, RESULT: {}", path_wrong,
                 matrix_edges.isThereChain(path_wrong));
    std::println("CORRECT CHAIN - {}, RESULT: {}", path_correct,
                 matrix_edges.isThereChain(path_correct));

    std::println("\n\n\n\n\n\n\n\nEDGELIST");
    graph_first::AdjacencyMatrix<graph_first::kNodeAmountResizable, true, true,
                                 uint8_t, graph_first::NodeListTag>
        matrix_node_list;
    matrix_node_list.addEntry(0, "Nikolai II");
    matrix_node_list.addEntry(1, "Petr III");
    matrix_node_list.addEntry(2, "Ekaterina II");
    matrix_node_list.addEntry(3, "Pavel I");
    matrix_node_list.addEntry(4, "Aleksandr I");
    matrix_node_list.addEntry(5, "Nikolai I");
    matrix_node_list.addEntry(6, "Aleksandr II");
    matrix_node_list.addEntry(7, "Aleksandr III");

    size_t _idx = 0;
    for (auto& i : matrix_node_list.getContainer()) {
      assert(i._idx == _idx++);
    }

    matrix_node_list.addEdge(0, 1, 2);
    matrix_node_list.addEdge(7, 0, 9);
    matrix_node_list.addEdge(2, 0, 5);
    matrix_node_list.addEdge(1, 2, 4);
    matrix_node_list.addEdge(1, 7, 4);
    matrix_node_list.addEdge(1, 3, 34);
    matrix_node_list.addEdge(2, 3, 5);
    matrix_node_list.addEdge(3, 4, 6);
    matrix_node_list.addEdge(3, 5, 6);
    matrix_node_list.addEdge(5, 6, 7);
    matrix_node_list.addEdge(6, 7, 8);
    matrix_node_list.addEdge(6, 1, 2);

    std::cout << "MATRIX\n";

    const auto& _matrix_node_list = matrix_node_list.getContainer();
    for (size_t i = 0; i < matrix_node_list.size(); ++i) {
      std::println("{} {} {}", _matrix_node_list[i]._idx,
                   _matrix_node_list[i]._edges, (_matrix_node_list[i]._name));
    }
    for (auto& names : matrix_node_list.getNames(graph_first::NodeListTag{})) {
      std::cout << names << '\n';
    }

    std::cout << '\n';

    std::cout << "ALL Neighbours\n";
    res = matrix_node_list.getNeighbours(0);
    for (auto& i : res) {
      std::println("{}", i);
    }

    std::cout << "All Edges\n";

    bfs_res = matrix_node_list.bfsEdges(0);
    std::cout << bfs_res.size() << '\n';
    for (auto& i : bfs_res) {
      std::println("{}", i);
    }

    std::cout << "VERTEX BIGGER THAN 15\n";
    res_big = matrix_node_list.getBigVertexes(15);
    for (auto& i : res_big) {
      std::println("{}", i);
    }
    std::cout << "IS THERE CHAIN?\n";

    std::println("INCORRECT CHAIN - {}, RESULT: {}", path_wrong,
                 matrix_node_list.isThereChain(path_wrong));
    std::println("CORRECT CHAIN - {}, RESULT: {}", path_correct,
                 matrix_node_list.isThereChain(path_correct));

    std::println("NEIGHBOURS BENCH");
    std::println("starting Bench adj ");
    double time_elapsed{};
    constexpr size_t kBenchN{1000000};
    for (size_t i = 0; i < kBenchN; ++i) {
      auto time     = std::chrono::steady_clock::now();
      auto res      = matrix.getNeighbours(0);
      auto duration = std::chrono::duration<double>{
          std::chrono::steady_clock::now() - time};
      time_elapsed += duration.count();
    }
    std::println("RESULT: {}", time_elapsed / kBenchN);

    std::println("starting Bench edges");
    time_elapsed = 0.0;
    for (size_t i = 0; i < kBenchN; ++i) {
      auto time     = std::chrono::steady_clock::now();
      auto res      = matrix_edges.getNeighbours(0);
      auto duration = std::chrono::duration<double>{
          std::chrono::steady_clock::now() - time};
      time_elapsed += duration.count();
    }
    std::println("RESULT: {}", time_elapsed / kBenchN);

    std::println("starting Bench Nodes");
    time_elapsed = 0.0;
    for (size_t i = 0; i < kBenchN; ++i) {
      auto time     = std::chrono::steady_clock::now();
      auto res      = matrix_node_list.getNeighbours(0);
      auto duration = std::chrono::duration<double>{
          std::chrono::steady_clock::now() - time};
      time_elapsed += duration.count();
    }
    std::println("RESULT: {}", time_elapsed / kBenchN);

    std::println("BENCH ALL EDGES");
    std::println("starting Bench adj ");
    time_elapsed = 0.0;
    for (size_t i = 0; i < kBenchN; ++i) {
      auto time     = std::chrono::steady_clock::now();
      auto res      = matrix.bfsEdges(0);
      auto duration = std::chrono::duration<double>{
          std::chrono::steady_clock::now() - time};
      time_elapsed += duration.count();
    }
    std::println("RESULT: {}", time_elapsed / kBenchN);

    std::println("starting Bench edges");
    time_elapsed = 0.0;
    for (size_t i = 0; i < kBenchN; ++i) {
      auto time     = std::chrono::steady_clock::now();
      auto res      = matrix_edges.bfsEdges(0);
      auto duration = std::chrono::duration<double>{
          std::chrono::steady_clock::now() - time};
      time_elapsed += duration.count();
    }
    std::println("RESULT: {}", time_elapsed / kBenchN);

    std::println("starting Bench Nodes");
    time_elapsed = 0.0;
    for (size_t i = 0; i < kBenchN; ++i) {
      auto time     = std::chrono::steady_clock::now();
      auto res      = matrix_node_list.bfsEdges(0);
      auto duration = std::chrono::duration<double>{
          std::chrono::steady_clock::now() - time};
      time_elapsed += duration.count();
    }
    std::println("RESULT: {}", time_elapsed / kBenchN);

    std::println("15 AND GREATER WEIGHT BENCH");
    std::println("starting Bench adj ");

    for (size_t i = 0; i < kBenchN; ++i) {
      auto time     = std::chrono::steady_clock::now();
      auto res      = matrix.getBigVertexes(25);
      auto duration = std::chrono::duration<double>{
          std::chrono::steady_clock::now() - time};
      time_elapsed += duration.count();
    }
    std::println("RESULT: {}", time_elapsed / kBenchN);

    std::println("starting Bench edges");
    time_elapsed = 0.0;
    for (size_t i = 0; i < kBenchN; ++i) {
      auto time     = std::chrono::steady_clock::now();
      auto res      = matrix_edges.getBigVertexes(25);
      auto duration = std::chrono::duration<double>{
          std::chrono::steady_clock::now() - time};
      time_elapsed += duration.count();
    }
    std::println("RESULT: {}", time_elapsed / kBenchN);

    std::println("starting Bench Nodes");
    time_elapsed = 0.0;
    for (size_t i = 0; i < kBenchN; ++i) {
      auto time     = std::chrono::steady_clock::now();
      auto res      = matrix_node_list.getBigVertexes(25);
      auto duration = std::chrono::duration<double>{
          std::chrono::steady_clock::now() - time};
      time_elapsed += duration.count();
    }
    std::println("RESULT: {}", time_elapsed / kBenchN);

    std::println("CHAIN BENCH");
    std::println("starting Bench adj ");
    time_elapsed = 0.0;
    for (size_t i = 0; i < kBenchN; ++i) {
      auto time     = std::chrono::steady_clock::now();
      auto res      = matrix.isThereChain(path_correct);
      res           = matrix.isThereChain(path_wrong);

      auto duration = std::chrono::duration<double>{
          std::chrono::steady_clock::now() - time};
      time_elapsed += duration.count();
    }
    std::println("RESULT: {}", time_elapsed / kBenchN);

    std::println("starting Bench edges");
    time_elapsed = 0.0;
    for (size_t i = 0; i < kBenchN; ++i) {
      auto time     = std::chrono::steady_clock::now();
      auto res      = matrix.isThereChain(path_correct);
      res           = matrix.isThereChain(path_wrong);

      auto duration = std::chrono::duration<double>{
          std::chrono::steady_clock::now() - time};
      time_elapsed += duration.count();
    }
    std::println("RESULT: {}", time_elapsed / kBenchN);

    std::println("starting Bench Nodes");
    time_elapsed = 0.0;
    for (size_t i = 0; i < kBenchN; ++i) {
      auto time     = std::chrono::steady_clock::now();
      auto res      = matrix.isThereChain(path_correct);
      res           = matrix.isThereChain(path_wrong);

      auto duration = std::chrono::duration<double>{
          std::chrono::steady_clock::now() - time};
      time_elapsed += duration.count();
    }

    std::println("RESULT: {}", time_elapsed / kBenchN);

    std::println(
        "Current sizes: \n\tAdjacency - {}\n\tEdges - {}\n\tNodeList - {}",
        sizeof(matrix), sizeof(matrix_edges), sizeof(matrix_node_list));
  }
}
