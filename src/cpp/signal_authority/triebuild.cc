#include <iostream>
#include <fstream>
#include "trie.h"

void usage()
{
  std::cerr << "Usage: ./mafsabuild <infile>" << std::endl;
}

int main(int argc, char* argv[])
{
  Trie trie;
  if (argc < 2) {
    trie.read_file(std::cin);
  } else if (argc == 2) {
    std::ifstream infile(argv[1]);
    if (!infile) {
      std::cerr << "Input file error" << std::endl;
      return 1;
    }
    trie.read_file(infile);
    infile.close();
  } else {
    usage();
    return 0;
  }
  trie.compact_edges();
  std::ostringstream file_name_stream;
  file_name_stream << trie.get_num_strings() << "_"
                   << trie.get_num_bytes() << "_"
                   << trie.get_num_nodes() << "_"
                   << trie.get_num_edges() << ".gv";
  std::ofstream outfile(file_name_stream.str());
  trie.dump_dictionary(outfile);
  outfile.close();
  return 0;
}
