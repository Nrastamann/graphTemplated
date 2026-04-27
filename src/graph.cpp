#include "graph.hpp"
namespace graph_first {
  void
  draw_name(std::string_view str, size_t value)
  {
    std::string final_str(value, '=');

    auto it = final_str.begin() + (final_str.length() - str.length()) / 2;
    for (char i : str) {
      *it++ = i;
    }
    std::cout << final_str << '\n';
  }
}  // namespace graph_first
