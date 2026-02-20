#include "graph.hpp"
namespace GraphFirst
{
void draw_name(std::string_view str, size_t value)
{
	std::string final_str(value, '=');

	auto it = final_str.begin() + (final_str.length() - str.length()) / 2;
	for (auto i = str.begin(); i != str.end(); ++i) {
		*it++ = *i;
	}
	std::cout << final_str << '\n';
}
}
