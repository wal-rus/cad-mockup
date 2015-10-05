#include <iostream>
#include <fstream>

#include "picojson.h"

void PrintUsage() {
  std::cout << "Invalid arguments. Json Data required" << std::endl;
}

struct ToolInfo {
  const double padding; //In inches
  const double max_speed; //In inches per second
  const double cost; //In dollars per second
};

struct MaterialInfo {
  const double cost_per_sq_in;
};

const static ToolInfo LASER_CUTTER = {.1, .5, 0.07};
const static MaterialInfo ALUMINUM = { 0.75 };

struct ToolPath {
  const picojson::value v; //for now just do operations in place.
};

struct Vector {
  double x,y;
};

Vector operator-(const Vector& v0, const Vector& v1) {
  return { v0.x - v1.x, v0.y - v1.y };
}

double Dot(const Vector& v0, const Vector& v1) {
  return (v0.x * v1.x) + (v0.y * v1.y);
}

double Distance(const Vector& v0, const Vector& v1) {
  const auto diff = v1 - v0;
  return sqrt( Dot(diff,diff) );
}

Vector Normalized(const Vector& v) {
  const static Vector zero = {0.,0.};
  auto mag = Distance(v, zero);
  return {v.x / mag, v.y / mag};
}
  
Vector ParseVector(const picojson::value& xyPair) {
  return { xyPair.get("X").get<double>(), xyPair.get("Y").get<double>() };
}

Vector ParseVertex(const picojson::value& vertices, const std::string& id) {
  const auto& vertex = vertices.get(id);
  const auto& position = vertex.get("Position");
  return ParseVector(position);
}

//Assumes the edges form a connected shape, we can garuntee that
//the total tool travel time is the sum of the travel time of each edge.
double ComputePathCost(const ToolInfo& tool, const ToolPath& path) {
  double sumCost = 0;
  if(!path.v.is<picojson::object>())
    return 0;
  
  const auto& obj = path.v.get<picojson::object>();
  const auto& edges = obj.find("Edges")->second.get<picojson::object>();
  const auto& vertices = obj.find("Vertices")->second;
  
  for( const auto& edge : edges) {
    const auto type = edge.second.get("Type").to_str();
    if(type == "LineSegment") {
      const auto& edgeVertices = edge.second.get("Vertices").get<picojson::array>();
      const auto v0 = ParseVertex(vertices, edgeVertices[0].to_str());
      const auto v1 = ParseVertex(vertices, edgeVertices[1].to_str());
      
      const auto dist = Distance(v0,v1);
      const auto time = dist / tool.max_speed;
      const auto cost = time * tool.cost;
      sumCost += cost;
    }
    else if( type == "CircularArc") {
      const auto& edgeVertices = edge.second.get("Vertices").get<picojson::array>();
      const auto v0 = ParseVertex(vertices, edgeVertices[0].to_str());
      const auto v1 = ParseVertex(vertices, edgeVertices[1].to_str());
      const auto center = ParseVector(edge.second.get("Center"));
      
      const auto radius = Distance(center,v0);
      const auto arcLine0 = Normalized(v0 - center);
      const auto arcLine1 = Normalized(v1 - center);
      const double arcAngle = acos(Dot(arcLine0,arcLine1));
      const double arcLength = arcAngle * radius;
      
      const auto cutSpeed = tool.max_speed * exp(-1/radius);
      const auto arcTime = arcLength / cutSpeed;
      const auto arcCost = arcTime * tool.cost;

      sumCost += arcCost;
    }
    else {
      std::cout << "Unknown segment type:" << type << std::endl;
    }
  }

  return sumCost;
};

int main(int argc, char** argv) {
  if(argc != 2) {
    PrintUsage();
    return 1;
  }

  std::string dataFileName(argv[1]);

  std::ifstream dataFile(dataFileName);
  if( dataFile.fail() ) {
    std::cout << "Error opening file " << dataFileName << std::endl;
    return 1;
  }

  picojson::value v;
  dataFile >> v;

  ToolPath path = {std::move(v)};
  
  auto machineCost = ComputePathCost(LASER_CUTTER, path);
  std::cout << "Machine cost is " << machineCost << std::endl;
  

  return 0;
}
