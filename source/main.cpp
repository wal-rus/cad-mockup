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

void ProduceQuote(const MachineInfo& tooling, const ToolPath& path) {
  
  const auto cutTime = path.ComputeTravelHeuristic() / tooling.max_speed;
  std::cout << "Estimated cut time: " << cutTime << " seconds" << std::endl;
  
  std::cout << "Estimated cost: $" <<
    std::fixed << std::setprecision(2) <<
    ComputeCost(tooling, path.ComputeBounds(), cutTime) << std::endl;
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
