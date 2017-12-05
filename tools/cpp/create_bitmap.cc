//
// Created by smaptas on 29.11.17.
//

// Include C standard library headers.

// Include C++ standard library headers.
#include <fstream>
#include <iostream>
#include <sstream>

// Include other headers from this project.
#include "../../../src/cpp/common/bit_writer.h"
//#include "../../../src/cpp/common/measure_time.h"
//#include "../../../src/cpp/common/io_option.h"

int main(int argc, char* argv[])
{
  std::ifstream infile(argv[1]);
  std::ofstream outfile(argv[2]);
  int height, width;
  infile >> height >> width;
  outfile << "P4\n" << width << " " << height << std::endl;
  std::string line;
  std::getline(infile, line);
  BitWriter writer(outfile);
  int repeat;
  std::string s;
  while(std::getline(infile, line)) {
    std::stringstream(line) >> repeat >> s;
    for (auto i = 0; i < repeat; ++i) {
      for (auto c: s) {
        writer.write_bit(c == '1');
      }
      writer.flush();
    }
  }
  return 0;
}
