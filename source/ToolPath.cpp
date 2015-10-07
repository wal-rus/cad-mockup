#include "ToolPath.h"
#include "JsonSerialization.h"

#include <unordered_map>

template<typename T = picojson::value>
const T& GetRequired(const picojson::value& v, const std::string& name) {
  if(!v.contains(name))
    throw std::runtime_error("Error parsing json: Requred field " + name + "not found");
  
  const auto& ret = v.get(name);

  if(!ret.is<T>()) {
    throw std::runtime_error("Error parsing json: Requred field " + name + "had unexpected type");
  }
  
  return ret.get<T>();
}
template<>
const picojson::value& GetRequired(const picojson::value& v, const std::string& name) {
  if(!v.contains(name))
    throw std::runtime_error("Error parsing json: Requred field " + name + "not found");
  
  return v.get(name);
}

ToolPath::ToolPath(const picojson::value &v) {
  const auto& vertices = GetRequired<picojson::object>(v,"Vertices");
  
  //Temporary cache of old vertexIds to new Vertex indexes.
  std::unordered_map<std::string,size_t> vertexIndexes;
  for(const auto& vertex : vertices) {
    const auto& vertexID = vertex.first;
    vertexIndexes[vertexID] = m_vertices.size();
    
    m_vertices.push_back(
      ParseVector(GetRequired(vertex.second,"Position"))
    );
  }

  const auto& edges = GetRequired<picojson::object>(v, "Edges");
  
  for(const auto& edge : edges) {
    if(!edge.second.contains("Vertices"))
      throw std::runtime_error("Error parsing json: Edge contains no Vertices");
    
    const auto& edgeVertices = edge.second.get("Vertices").get<picojson::array>();
    
    Vector2* v0 = &m_vertices[ vertexIndexes[edgeVertices[0].to_str()] ];
    Vector2* v1 = &m_vertices[ vertexIndexes[edgeVertices[1].to_str()] ];
    
    const auto& type = GetRequired<std::string>(edge.second,"Type");
    if(type == "LineSegment"){
      m_linearEdges.push_back(LinearEdge(v0,v1));
    }
    else if( type == "CircularArc") {
      //Ensure that v0 is the first vertex on the arc, moving counter-clockwise.
      const auto& clockwiseFrom = GetRequired(edge.second, "ClockwiseFrom");
      if(edgeVertices[1] != clockwiseFrom)
        std::swap(v0, v1);
      
      if(!edge.second.contains("Center"))
        throw std::runtime_error("Error parsing json: Arc has no center");

      m_arcEdges.push_back(ArcEdge(v0,v1,ParseVector( GetRequired(edge.second,"Center") )));
    }
    else
      throw std::runtime_error("Error parsing json: Unkown edge type: " + type);
  }
}

//Assumes the edges form a connected shape, we can garuntee that
//the total tool travel time is the sum of the travel time of each edge.
double ToolPath::ComputeTravelHeuristic() const {
  double sumDistance = 0;
  
  for( const auto& edge : m_linearEdges) {
    const auto dist = Distance(*edge.v0,*edge.v1);
    sumDistance += dist;
  }
  
  for(const auto& arc : m_arcEdges) {
    const auto &v0 = *arc.v0;
    const auto &v1 = *arc.v1;

    const auto radius = Distance(arc.center,v0);
    const auto arcLine0 = (v0 - arc.center) / radius;
    const auto arcLine1 = (v1 - arc.center) / radius;
    const double arcAngle = acos(Dot(arcLine0, arcLine1));
    const double arcLength = arcAngle * radius;
      
    //Scale to account for linear stepper arc traversing behavior
    const auto arcEffectiveLength = arcLength * (1/exp(-1/radius));
    sumDistance += arcEffectiveLength;
  }

  return sumDistance;
};

Vector2 ToolPath::ComputeBounds() const {
  Vector2 minPoint = { std::numeric_limits<double>::max(), std::numeric_limits<double>::max() };
  Vector2 maxPoint = { std::numeric_limits<double>::min(), std::numeric_limits<double>::min() };
  
  for( const auto& edge : m_linearEdges) {
    const auto &v0 = *edge.v0;
    const auto &v1 = *edge.v1;

    minPoint.x = std::min(minPoint.x, std::min(v0.x,v1.x));
    minPoint.y = std::min(minPoint.y, std::min(v0.y,v1.y));
    maxPoint.x = std::max(maxPoint.x, std::max(v0.x,v1.x));
    maxPoint.y = std::max(maxPoint.y, std::max(v0.y,v1.y));
  }
  
  for( const auto& edge : m_arcEdges) {
    const auto &v0 = *edge.v0;
    const auto &v1 = *edge.v1;
    const auto &center = edge.center;
      
    const auto radius = Distance(center,v0);
    const auto arcLine0 = (v0 - center) / radius;
    const auto arcLine1 = (v1 - center) / radius;
      
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
  
  return maxPoint - minPoint;
}