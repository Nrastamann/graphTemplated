#version 460 core

in vec4 line_points_frag;
in vec3 colour_frag;

uniform float u_resolution;

out vec4 screen_colour;

void
main()
{
  vec2 pixelCoord  = gl_FragCoord.xy / u_resolution - 2.0 + 1.0;
  vec4 line_points = line_points_frag / u_resolution - 2.0 + 1.0;

  vec2 pa = pixelCoord - line_points.xy, ba = line_points.zw - line_points.xy;
  float h       = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
  float alpha   = smoothstep(0.0, 1.0 / u_resolution.x, length(pa - ba * h));

  screen_colour = vec4(vec3(0.0), 1.0);
}
