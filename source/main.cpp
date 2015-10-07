#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>

#define _USE_MATH_DEFINES
#include <cmath>

#include "picojson.h"
#include "MachineInfo.h"
#include "Vector2.h"
#include "ToolPath.h"
#include "JsonSerialization.h"

void PrintUsage() {
  std::cout << "Invalid arguments. Json Data required" << std::endl;
}

const static MachineInfo LASER_CUT_ALUMINUM = {.1, .5, 0.07, 0.75};


double ComputeMaterialCost(const MachineInfo& job, const ToolPath& path) {
  auto dimensions = path.ComputeBounds();
  dimensions.x += job.padding;
  dimensions.y += job.padding;
  const auto area = dimensions.x * dimensions.y;
  const auto cost = area * job.cost_per_sq_in;
  return cost;
}

void ProduceQuote(const MachineInfo& job, const ToolPath& path) {
  
  const auto cutTime = path.ComputeTravelHeuristic() / job.max_speed;
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
  
  ToolPath path(std::move(v));
  
  ProduceQuote(LASER_CUT_ALUMINUM, path);
  return 0;
}
