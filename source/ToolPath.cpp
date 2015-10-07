#include "ToolPath.h"
#include "JsonSerialization.h"

//Assumes the edges form a connected shape, we can garuntee that
//the total tool travel time is the sum of the travel time of each edge.
double ToolPath::ComputeTravelHeuristic() const {
  double sumDistance = 0;
  if(!v.is<picojson::object>())
    return 0;
  
  const auto& obj = v.get<picojson::object>();
  const auto& edges = obj.find("Edges")->second.get<picojson::object>();
  const auto& vertices = obj.find("Vertices")->second;
  
  for( const auto& edge : edges) {
    const auto type = edge.second.get("Type").to_str();
    if(type == "LineSegment") {
      const auto& edgeVertices = edge.second.get("Vertices").get<picojson::array>();
      const auto v0 = ParseVertex(vertices, edgeVertices[0]);
      const auto v1 = ParseVertex(vertices, edgeVertices[1]);
      
      const auto dist = Distance(v0,v1);
      sumDistance += dist;
    }
    else if( type == "CircularArc") {
      const auto& edgeVertices = edge.second.get("Vertices").get<picojson::array>();
      const auto v0 = ParseVertex(vertices, edgeVertices[0]);
      const auto v1 = ParseVertex(vertices, edgeVertices[1]);
      const auto center = ParseVector(edge.second.get("Center"));
      
      const auto radius = Distance(center,v0);
      const auto arcLine0 = Normalized(v0 - center);
      const auto arcLine1 = Normalized(v1 - center);
      const double arcAngle = acos(Dot(arcLine0,arcLine1));
      const double arcLength = arcAngle * radius;
      
      //Scale to account for linear stepper arc traversing behavior
      const auto arcEffectiveLength = arcLength * (1/exp(-1/radius));
      sumDistance += arcEffectiveLength;
    }
    else {
      std::cout << "Unknown segment type:" << type << std::endl;
    }
  }
  
  return sumDistance;
};


Vector2 ToolPath::ComputeBounds() const {
  const auto& obj = v.get<picojson::object>();
  const auto& edges = obj.find("Edges")->second.get<picojson::object>();
  const auto& vertices = obj.find("Vertices")->second;
  
  Vector2 minPoint = { std::numeric_limits<double>::max(), std::numeric_limits<double>::max() };
  Vector2 maxPoint = { std::numeric_limits<double>::min(), std::numeric_limits<double>::min() };
  
  for( const auto& edge : edges) {
    const auto type = edge.second.get("Type").to_str();
    if(type == "LineSegment") {
      const auto& edgeVertices = edge.second.get("Vertices").get<picojson::array>();
      const auto v0 = ParseVertex(vertices, edgeVertices[0]);
      const auto v1 = ParseVertex(vertices, edgeVertices[1]);
      minPoint.x = std::min(minPoint.x, std::min(v0.x,v1.x));
      minPoint.y = std::min(minPoint.y, std::min(v0.y,v1.y));
      maxPoint.x = std::max(maxPoint.x, std::max(v0.x,v1.x));
      maxPoint.y = std::max(maxPoint.y, std::max(v0.y,v1.y));
    }
    else if( type == "CircularArc") {
      const auto& edgeVertices = edge.second.get("Vertices").get<picojson::array>();
      const auto center = ParseVector(edge.second.get("Center"));
      
      auto v0 = ParseVertex(vertices, edgeVertices[0]);
      auto v1 = ParseVertex(vertices, edgeVertices[1]);
      
      //Ensure that v0 is the first vertex on the arc, moving counter-clockwise.
      if(edgeVertices[1] != edge.second.get("ClockwiseFrom"))
        std::swap(v0,v1);
      
      const auto radius = Distance(center,v0);
      const auto arcLine0 = Normalized(v0 - center);
      const auto arcLine1 = Normalized(v1 - center);
      
      //we only care about the 4 maximal points on the circle, or in the case of the arc,
      //the ones that fit within the arc boundaries.
      const auto a0 = atan2(arcLine0.y, arcLine0.x);
      auto a1 = atan2(arcLine1.y, arcLine1.x);
      
      //ensure a1 > a0 so that the clamping operation works correctly
      if( a1 <= a0)
        a1 += 2*M_PI;
      
      //For each of the 4 cardinal directions...
      for(int i = 0; i < 4; ++i) {
        double theta = M_PI_2 * i;
        theta = std::min( a1, std::max(a0, theta) ); //clamp between a0 and a1
        
        const Vector2 angleVector = { cos(theta)*radius, sin(theta)*radius };
        const Vector2 arcPoint = center + angleVector;
        
        minPoint.x = std::min(minPoint.x, arcPoint.x);
        minPoint.y = std::min(minPoint.y, arcPoint.y);
        maxPoint.x = std::max(maxPoint.x, arcPoint.x);
        maxPoint.y = std::max(maxPoint.y, arcPoint.y);
      }
    }
    else {
      std::cout << "Unknown segment type:" << type << std::endl;
    }
  }
  
  return maxPoint - minPoint;
}