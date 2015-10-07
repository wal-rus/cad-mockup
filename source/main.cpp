#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>

#define _USE_MATH_DEFINES
#include <cmath>

#include "picojson.h"
#include "JobProfile.h"
#include "Vector2.h"

void PrintUsage() {
  std::cout << "Invalid arguments. Json Data required" << std::endl;
}

struct ToolPath {
  const picojson::value v; //for now just do operations in place.
};

const static JobProfile LASER_CUT_ALUMINUM = {.1, .5, 0.07, 0.75};


Vector2 ParseVector(const picojson::value& xyPair) {
  return { xyPair.get("X").get<double>(), xyPair.get("Y").get<double>() };
}

Vector2 ParseVertex(const picojson::value& vertices, const std::string& id) {
  const auto& vertex = vertices.get(id);
  const auto& position = vertex.get("Position");
  return ParseVector(position);
}

//Assumes the edges form a connected shape, we can garuntee that
//the total tool travel time is the sum of the travel time of each edge.

double ComputeTravelDistance(const ToolPath& path) {
  
  double sumDistance = 0;
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
      sumDistance += dist;
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
      
      //Scale to compensate for the tool's arc traversing behavior
      const auto arcEffectiveLength = arcLength * (1/exp(-1/radius));
      sumDistance += arcEffectiveLength;
    }
    else {
      std::cout << "Unknown segment type:" << type << std::endl;
    }
  }

  return sumDistance;
};

Vector2 ComputeRequiredDimensions(const ToolPath& path) {
  const auto& obj = path.v.get<picojson::object>();
  const auto& edges = obj.find("Edges")->second.get<picojson::object>();
  const auto& vertices = obj.find("Vertices")->second;
  
  Vector2 minPoint = { std::numeric_limits<double>::max(), std::numeric_limits<double>::max() };
  Vector2 maxPoint = { std::numeric_limits<double>::min(), std::numeric_limits<double>::min() };
  
  for( const auto& edge : edges) {
    const auto type = edge.second.get("Type").to_str();
    if(type == "LineSegment") {
      const auto& edgeVertices = edge.second.get("Vertices").get<picojson::array>();
      const auto v0 = ParseVertex(vertices, edgeVertices[0].to_str());
      const auto v1 = ParseVertex(vertices, edgeVertices[1].to_str());
      minPoint.x = std::min(minPoint.x, std::min(v0.x,v1.x));
      minPoint.y = std::min(minPoint.y, std::min(v0.y,v1.y));
      maxPoint.x = std::max(maxPoint.x, std::max(v0.x,v1.x));
      maxPoint.y = std::max(maxPoint.y, std::max(v0.y,v1.y));
    }
    else if( type == "CircularArc") {
      const auto& edgeVertices = edge.second.get("Vertices").get<picojson::array>();
      const auto center = ParseVector(edge.second.get("Center"));
      
      auto v0 = ParseVertex(vertices, edgeVertices[0].to_str());
      auto v1 = ParseVertex(vertices, edgeVertices[1].to_str());

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

double ComputeMaterialCost(const JobProfile& job, const ToolPath& path) {
  auto dimensions = ComputeRequiredDimensions(path);
  dimensions.x += job.padding;
  dimensions.y += job.padding;
  const auto area = dimensions.x * dimensions.y;
  const auto cost = area * job.cost_per_sq_in;
  return cost;
}

void ProduceQuote(const JobProfile& job, const ToolPath& path) {
  
  const auto cutTime = ComputeTravelDistance(path) / job.max_speed;
  std::cout << "Estimated cut time: " << cutTime << " seconds" << std::endl;
  
  const auto totalCost = (cutTime * job.cost_per_s) + ComputeMaterialCost(job, path);
  
  std::cout << "Estimated cost: $" <<
    std::fixed << std::setprecision(2) <<
    totalCost << std::endl;
  
}

int main(int argc, char** argv) {
  if(argc != 2) {
    PrintUsage();
    return 1;
  }
  
  std::ifstream pathFile(argv[1]);
  if( pathFile.bad() ) {
    throw std::runtime_error("Error opening path file." );
  }
  
  picojson::value v;
  pathFile >> v;
  
  ToolPath path = {std::move(v)};
  
  ProduceQuote(LASER_CUT_ALUMINUM, path);
  return 0;
}
