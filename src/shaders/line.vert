#version 460 core

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec3 colour;
layout(location = 2) in vec4 line_points;

uniform float u_resolution;

out vec4 line_points_frag;
out vec3 colour_frag;

void
main()
{
  gl_Position      = vec4(vertex / u_resolution * 2.0 - 1.0, -1.0f, 1.0f);

  line_points_frag = line_points;
  colour_frag      = colour;
}
