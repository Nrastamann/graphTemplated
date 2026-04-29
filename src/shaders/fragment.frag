#version 460 core

in vec4 result_colour;

out vec4 screenColor;

void
main()
{
  screenColor = result_colour;
}
