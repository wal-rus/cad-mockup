// Very basic 2d vector class - If any additional math functions are required,
// adopt a professional library such as Eigen or Bullet Vector2 Math
#pragma once
#include <cmath>

struct Vector2 {
  double x,y;
};

inline Vector2 operator-(const Vector2& v0, const Vector2& v1) {
  return { v0.x - v1.x, v0.y - v1.y };
}

inline Vector2 operator+(const Vector2& v0, const Vector2& v1) {
  return { v0.x + v1.x, v0.y + v1.y };
}

inline Vector2 operator*(const Vector2& v0, const Vector2& v1) {
  return { v0.x * v1.x, v0.y * v1.y };
}

inline Vector2 operator/(const Vector2& v0, const Vector2& v1) {
  return { v0.x / v1.x, v0.y / v1.y };
}

inline double Dot(const Vector2& v0, const Vector2& v1) {
  return (v0.x * v1.x) + (v0.y * v1.y);
}

inline double Distance(const Vector2& v0, const Vector2& v1) {
  const auto diff = v1 - v0;
  return sqrt( Dot(diff,diff) );
}

inline Vector2 Normalized(const Vector2& v) {
  const static Vector2 zero = {0.,0.};
  auto mag = Distance(v, zero);
  return {v.x / mag, v.y / mag};
}