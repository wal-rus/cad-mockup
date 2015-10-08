#pragma once
#include "picojson.h"
#include "Vector2.h"
#include <vector>


class ToolPath {
public:

  ToolPath(const picojson::value &value);

  //Returns roughly the distance in inches, but scaled slightly to
  //account for accelleration time between direction changes and
  //the slower speed of traversing arcs.
  double ComputeTravelHeuristic() const;

  Vector2 ComputeBounds() const;
private:

  //We parse all vertices before the edges, so it is safe to store pointers
  //into the vertex set directly.
  struct LinearEdge {
    LinearEdge(Vector2* a, Vector2* b ) : v0(a), v1(b) {}
    const Vector2* v0;
    const Vector2* v1;
  };

  struct ArcEdge {
    ArcEdge(Vector2* a, Vector2* b, const Vector2& c) : v0(a), v1(b), center(c) {}
    const Vector2* v0;
    const Vector2* v1;
    const Vector2 center;
  };

  std::vector<Vector2> m_vertices; //rarely accessed directly.
  std::vector<LinearEdge> m_linearEdges;
  std::vector<ArcEdge> m_arcEdges;
};

