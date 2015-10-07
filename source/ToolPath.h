#pragma once
#include "picojson.h"
#include "Vector2.h"

class ToolPath {
public:
  ToolPath(const picojson::value &value) : v(value) {}
  
  const picojson::value v; //for now just do operations in place.

  //Returns roughly the distance in inches, but scaled slightly to
  //account for accelleration time between direction changes and
  //the slower speed of traversing arcs.
  double ComputeTravelHeuristic() const;
  
  Vector2 ComputeBounds() const;
};

