#include "JsonSerialization.h"
#include "picojson.h"

Vector2 ParseVector(const picojson::value& xyPair) {
  return { xyPair.get("X").get<double>(), xyPair.get("Y").get<double>() };
}

Vector2 ParseVertex(const picojson::value& vertices, const picojson::value& id) {
  const auto& vertex = vertices.get(id.to_str());
  const auto& position = vertex.get("Position");
  return ParseVector(position);
}
