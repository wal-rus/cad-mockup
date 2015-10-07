#pragma once

#include "Vector2.h"

namespace picojson {
  class value;
}


//JSON -> Data type conversion functions

Vector2 ParseVector(const picojson::value& xyPair);
Vector2 ParseVertex(const picojson::value& vertices, const picojson::value& id);
