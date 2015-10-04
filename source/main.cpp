#include <iostream>
#include <fstream>

#include "picojson.h"

void PrintUsage() {
  std::cout << "Invalid arguments. Json Data required" << std::endl;
}

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

  std::cout << "-- dump --" << std::endl;
  std::cout << v << std::endl;
  return 0;
}
