#version 460 core

in vec2 vertexFragment;
in vec3 colourFragment;
in vec3 BordercolourFragment;
in float FlagFrag;

out vec4 screenColor;

uniform vec2 u_resolution;
uniform float borderThickness;
uniform float radius;
float
drawLine(vec2 p1, vec2 p2)
{
  vec2 fragCoordHalfed = gl_FragCoord.xy / u_resolution.xy;

  float a              = abs(distance(p1, fragCoordHalfed));
  float b              = abs(distance(p2, fragCoordHalfed));
  float c              = abs(distance(p1, p2));

  if (a >= c || b >= c) {
    return 0.0;
  }
  float p = (a + b + c) * 0.5;
  float h = 2 / c * sqrt(p * (p - a) * (p - b) * (p - c));

  return mix(1.0, 0.0, smoothstep(0.5 * 0.002, 1.5 * 0.002, h));
}

void
main()
{
  if (FlagFrag > 0) {
    screenColor =
        vec4(colourFragment.xyz,
             drawLine((BordercolourFragment.xy) / u_resolution,
                      vec2(BordercolourFragment.z, FlagFrag) / u_resolution));
    return;
  }

  vec2 pixelCoord  = gl_FragCoord.xy - vertexFragment;

  float d          = pixelCoord.x * pixelCoord.x + pixelCoord.y * pixelCoord.y;
  float kNewRadius = radius + borderThickness;

  if (d >= kNewRadius * kNewRadius) {
    screenColor = vec4(0.0);
    return;
  }

  screenColor = vec4(
      ((d - radius * radius) < 0.0) ? colourFragment : BordercolourFragment,
      1.0);
}
