#include <iostream>
#include <fstream>

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
  std::cout << dataFile.rdbuf() << std::endl;
  return 0;
}
