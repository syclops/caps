#include <fstream>
#include <iostream>
#include <sstream>

#include "mafsa.h"

void usage()
{
  std::cerr << "Usage: ./mafsatest [infile] [testfile] [outfile]" << std::endl;
}

int main(int argc, char* argv[])
{
  if (argc !=4) {
    usage();
    return 1;
  }
  SignalSet graph;
  std::ifstream infile(argv[1]);
  if (!infile) {
    std::cerr << "Input file error" << std::endl;
    return 1;
  }
  graph.read_file(infile);
  infile.close();
  std::ifstream testfile(argv[2]);
  if (!testfile) {
    std::cerr << "Test file error" << std::endl;
    return 1;
  }
  std::string line;
  while (std::getline(testfile, line)) {
    if (!graph.check_word(line)) {
      std::cerr << "Expected " << line << " to work but didn't" << std::endl;
      return 2;
    }
  }
  testfile.close();
  std::ofstream outfile(argv[3]);
  if (!outfile) {
    std::cerr << "Output file error" << std::endl;
    return 1;
  }
  graph.dump_dictionary(outfile);
  outfile.close();
  return 0;
}
