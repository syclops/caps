#include <fstream>
#include <iostream>

void usage()
{
  std::cerr << "Usage: ./charcount <infile>" << std::endl;
}

void char_count(std::istream& input_stream)
{
  size_t counts[128];
  for (size_t i = 0; i < 128; ++i) {
    counts[i] = 0;
  }
  std::string line;
  while (std::getline(input_stream, line)) {
    for (size_t i = 0; i < line.length(); ++i) {
      ++counts[(int) line[i]];
    }
  }
  for (size_t i = 0; i < 128; ++i) {
    std::cout << (char) i << " " << counts[i] << std::endl;
  }
}

int main(int argc, char* argv[])
{
  if (argc < 2) {
    char_count(std::cin);
  } else if (argc == 2) {
    std::ifstream infile(argv[1]);
    if (!infile) {
      std::cerr << "Input file error" << std::endl;
      return 1;
    }
    char_count(infile);
    infile.close();
  } else {
    usage();
    return 0;
  }
  return 0;
}
