//
// Created by smaptas on 22.11.17.
//

#include <cmath>

#include <chrono>
#include <iostream>
#include <fstream>
#include <utility>

#include "../../src/cpp/encoding/huffman.h"
#include "../../src/cpp/common/io_option.h"
#include "../../src/cpp/common/measure_time.h"

#include <cxxopts.hpp>

std::unordered_map<std::string, int> count_labels(std::istream& in_stream)
{
  std::string line, label;
  int count;
  std::unordered_map<std::string, int> label_counts;
  while (std::getline(in_stream, line)) {
    std::stringstream(line) >> label >> count;
    label_counts.emplace(std::move(label), count);
  }
  return label_counts;
}

HuffmanCoder<std::string> build_coder(
  const std::unordered_map<std::string, int>& label_counts)
{
  return HuffmanCoder<std::string>(label_counts);
}

int write_codebook(std::ostream& out_stream, HuffmanCoder<std::string>& coder)
{
  int size = 0;
  const auto codebook = coder.decoding_map();
  for (const auto& symbol: std::get<0>(codebook)) {
    out_stream << symbol << std::endl;
    size += 8 * symbol.length();
  }
  const auto index_size = static_cast<int>(
    ceil(log2(*std::get<1>(codebook).rbegin())));
  for (auto index: std::get<1>(codebook)) {
    out_stream << index << " ";
    size += index_size;
  }
  out_stream << std::endl;
  if (size % 8 != 0) {
    size += 8 - (size % 8);
  }
  const auto code_size = static_cast<int>(
    ceil(log2(*std::get<2>(codebook).rbegin())));
  for (auto code: std::get<2>(codebook)) {
    out_stream << code << " ";
    size += code_size;
  }
  out_stream << std::endl;
  if (size % 8 != 0) {
    size += 8 - (size % 8);
  }
  return size;
}

int main(int argc, char* argv[])
{
  try {
    // Options processing.
    cxxopts::Options options(argv[0], "Huffman codebook build and dump");
    options.add_options()
      ("i,infile", "Input file",
       cxxopts::value<std::string>()->default_value(""))
      ("h,help", "Print this help message")
      ("o,outfile", "Output file",
       cxxopts::value<std::string>()->default_value(""))
      ("s,suppress", "Suppress output of codebook");
    auto result = options.parse(argc, argv);

    if (result.count("help") > 0) {
      std::cout << options.help({""}) << std::endl;
      return 0;
    }

    const auto infile = result["infile"].as<std::string>();
    const auto outfile = result["outfile"].as<std::string>();

    FunctionTimer<HuffmanCoder<std::string>, std::string> in_timer(
      [](std::string in_file) {
      return build_coder(input_option(count_labels, in_file));
    });
    std::cout << "Building Huffman coder from "
              << (infile.empty() ? "standard input" : infile)
              << "..." << std::flush;
    auto coder = in_timer.run(infile);
    std::cout << "done! (took " << in_timer.time() << " seconds)" << std::endl;

    FunctionTimer<int, std::string> out_timer([&coder](std::string out_file) {
      return output_option(write_codebook, out_file, coder);
    });
    int size;
    if (result.count("suppress") > 0) {
      size = out_timer.run("/dev/null");
    } else {
      std::cout << "Writing codebook to "
                << (outfile.empty() ? "standard output" : outfile)
                << "..." << std::flush;
      size = out_timer.run(outfile);
      std::cout << "done! (took " << out_timer.time() << " seconds)"
                << std::endl;
    }
    std::cout << "Codebook is " << size << " bits" << std::endl;
  } catch (const cxxopts::OptionException& e) {
    std::cout << "Error parsing options: " << e.what() << std::endl;
    exit(1);
  }
  return 0;
}
