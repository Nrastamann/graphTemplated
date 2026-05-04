#version 460 core

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec3 colour;
layout(location = 2) in vec3
    Bordercolour;  // when drawing lines, use colour as colour
layout(location = 3) in vec2 vertexTarget;
layout(location = 4) in float Flag;  // and borderColour + Flag as 4 points

uniform vec2 u_resolution;
uniform float borderThickness;
uniform float radius;

out vec2 vertexFragment;
out vec3 colourFragment;
out vec3 BordercolourFragment;
out float FlagFrag;

void
main()
{
  gl_Position    = vec4(vertexTarget / u_resolution * 2.0 - 1.0, 0.0f, 1.0f);
  vertexFragment = vertex;
  colourFragment = colour;
  BordercolourFragment = Bordercolour;
  FlagFrag             = Flag;
}
