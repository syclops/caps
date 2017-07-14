#include <fstream>
#include <iostream>
#include <sstream>

#include "mafsa.h"

void usage()
{
  std::cerr << "Usage: ./mafsabuild <infile>" << std::endl;
}

int main(int argc, char* argv[])
{
  Graph graph;
  if (argc < 2) {
    graph.read_file(std::cin);
  } else if (argc == 2) {
    std::ifstream infile(argv[1]);
    if (!infile) {
      std::cerr << "Input file error" << std::endl;
      return 1;
    }
    graph.read_file(infile);
    infile.close();
  } else {
    usage();
    return 0;
  }
  std::ostringstream file_name_stream;
  file_name_stream << graph.num_strings() << "_"
                   << graph.num_states() << "_"
                   << graph.num_bytes() << "_"
                   << graph.num_edges() << ".gv";
  std::ofstream outfile(file_name_stream.str());
  outfile << graph.to_dot();
  outfile.close();
  //std::cout << "Number of strings: " << graph.num_strings() << std::endl;
  //std::cout << "Number of states: " << graph.num_states() << std::endl;
  //std::cout << "Number of bytes: " << graph.num_bytes() << std::endl;
  //std::cout << "Number of edges: " << graph.num_edges() << std::endl;
  //std::cout << graph << std::endl;
  //std::cout << graph.to_dot() << std::endl;
  return 0;
}
