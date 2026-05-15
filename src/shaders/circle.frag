#version 460 core

in vec2 vertexFragment;
in vec3 colourFragment;
in vec3 BordercolourFragment;
in float FlagFrag;

out vec4 screenColor;

uniform vec2 u_resolution;
uniform float borderThickness;
uniform float radius;

const float h_divided = 1;
const float h         = 3;
const float epsilon   = 0.0001;
const float epsilonSQ = epsilon * epsilon;

vec4
getPointy(vec2 peak)  // direction up - true
{
  float angle    = atan(peak.y / peak.x);
  float cosangle = cos(angle);
  float sinangle = 1 - cosangle * cosangle;

  float x0_h     = peak.y / sinangle - h;

  return vec4(x0_h * cosangle - h_divided * sinangle,
              x0_h * sinangle + h_divided * cosangle,
              x0_h * cosangle + h_divided * sinangle,
              x0_h * sinangle - h_divided * cosangle);
}

float
side(vec2 point1, vec2 point2, vec2 pointTest)
{
  return (point2.y - point1.y) * (pointTest.x - point1.x) +
         (-point2.x + point1.x) * (pointTest.y - point1.y);
}

bool
naiveTriangle(vec2 point1, vec2 point2, vec2 point3, vec2 normalizedCoords)
{
  bool check = side(point1, point2, normalizedCoords) >= 0;
  check      = check && side(point2, point3, normalizedCoords) >= 0;
  check      = check && side(point3, point1, normalizedCoords) >= 0;
  return check;
}

bool
boundingBox(vec2 point1, vec2 point2, vec2 point3, vec2 normalizedCoords)
{
  float xmin = min(point1.x, min(point2.x, point3.x)) - epsilon;
  float ymin = min(point1.y, min(point2.y, point3.y)) - epsilon;
  float xmax = max(point1.x, max(point2.x, point3.x)) + epsilon;
  float ymax = max(point1.y, max(point2.y, point3.y)) + epsilon;

  return (normalizedCoords.x < xmin || xmax < normalizedCoords.x ||
          normalizedCoords.y < ymin || ymax < normalizedCoords.y)
             ? false
             : true;
}

float
distanceSqPointToSegment(vec2 point1, vec2 point2, vec2 normalizedCoords)
{
  float dot_p = dot(point2, point1);

  if (dot_p < 0) {
    return (normalizedCoords.y - point1.y) * (normalizedCoords.y - point1.y) +
           (normalizedCoords.x - point1.x) * (normalizedCoords.x - point1.x);
  }
  if (dot_p <= 1) {
    float sq_length =
        (point1.y - normalizedCoords.y) * (point1.y - normalizedCoords.y) +
        (point1.x - normalizedCoords.x) * (point1.x - normalizedCoords.x);

    return sq_length - dot_p * dot_p *
                           ((point2.y - point1.y) * (point2.y - point1.y) +
                            (point2.x - point1.x) * (point2.x - point1.x));
  }

  return ((point2.y - normalizedCoords.y) * (point2.y - normalizedCoords.y) +
          (point2.x - normalizedCoords.x) * (point2.x - normalizedCoords.x));
}

float
isInTriangle(vec2 point1, vec2 point2, vec2 point3, vec2 normalizedCoords)
{
  if (!boundingBox(point1, point2, point3, normalizedCoords)) {
    return 0.0;
  }

  if (naiveTriangle(point1, point2, point3, normalizedCoords)) {
    return 1.0;
  }

  if (distanceSqPointToSegment(point1, point2, normalizedCoords) <= epsilonSQ) {
    return 1.0;
  }

  if (distanceSqPointToSegment(point2, point3, normalizedCoords) <= epsilonSQ) {
    return 1.0;
  }

  if (distanceSqPointToSegment(point3, point1, normalizedCoords) <= epsilonSQ) {
    return 1.0;
  }

  return 0.0;
}

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
  float p              = (a + b + c) * 0.5;
  float h              = 2 / c * sqrt(p * (p - a) * (p - b) * (p - c));

  vec4 triangle_points = getPointy(vec2(p2.x - p1.x, p2.y - p1.y));

  return max(isInTriangle(vec2(triangle_points.xy), vec2(triangle_points.zw),
                          p2, gl_FragCoord.xy),
             mix(1.0, 0.0, smoothstep(0.5 * 0.002, 1.5 * 0.002, h)));
}
// https://math.stackexchange.com/questions/3632502/how-to-check-if-a-point-px-y-lies-inside-a-thick-line
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
