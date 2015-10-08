// Very basic 2d vector class - If any additional math functions are required,
// adopt a professional library such as Eigen or Bullet Vector2 Math
#pragma once
#include <cmath>
#include <algorithm>

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

inline Vector2 operator/(const Vector2& v0, double scalar) {
  return { v0.x / scalar, v0.y / scalar };
}

inline double Dot(const Vector2& v0, const Vector2& v1) {
  return (v0.x * v1.x) + (v0.y * v1.y);
}

inline double Distance(const Vector2& v0, const Vector2& v1) {
  const auto diff = v1 - v0;
  return sqrt( Dot(diff,diff) );
}

inline void PiecewiseMin(Vector2& a, const Vector2& b) {
  a.x = std::min(a.x,b.x);
  a.y = std::min(a.y,b.y);
}

inline void PiecewiseMax(Vector2& a, const Vector2& b) {
  a.x = std::max(a.x,b.x);
  a.y = std::max(a.y,b.y);
}