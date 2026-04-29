#version 460 core

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec3 colour;
layout(location = 2) in vec3 Bordercolour;
out vec4 result_colour;

// uniform mat4 model;
// uniform mat4 view;
// uniform mat4 projection;
uniform vec4 borderColor;
uniform float borderThickness;
uniform float radius;

#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

void
main()
{
  vec2 st       = gl_FragCoord.xy / u_resolution;
  // a. The DISTANCE from the pixel to the center
  float d       = distance(st, vertex.xy);
  float bg      = 0.5;

  float pct     = 1.0 - smoothstep(0.0, borderThickness, abs(radius - d));

  float t1      = 1.0 - smoothstep(radius - borderThickness, radius, d);
  float t2      = 1.0 - smoothstep(radius, radius + borderThickness, d);

  vec3 bgColor  = vec3(1.0);
  vec3 color    = vec3(pct) * Bordercolour;

  result_colour = vec4(mix(color, colour, t1), t2);
  result_colour = mix(result_colour, vec4(bgColor * vec3(bg), 1.848), 2.0);
}
