//
// Created by smaptas on 07.11.17.
//

#include <chrono>
#include <iostream>
#include <fstream>
#include <utility>

#include "../../src/cpp/lexicon/fsa_lexicon.h"

#include "../../build/cxxopts-src/include/cxxopts.hpp"

template <typename VoidFuncType, typename... Args>
double measure_time(VoidFuncType function, Args&&... args)
{
  namespace chr = std::chrono;
  auto start_time = chr::high_resolution_clock::now();
  function(std::forward<Args>(args)...);
  return chr::duration_cast<chr::duration<double>>(
    chr::high_resolution_clock::now() - start_time).count();
}

FSALexicon make_lexicon(const std::string& infile_name, bool load)
{
  FSALexicon lexicon;
  std::ifstream infile(infile_name);
  double time;
  if (load) {
    std::cout << "Loading lexicon from " << infile_name << "..." << std::flush;
    time = measure_time([=](FSALexicon& l, std::ifstream& f) { l.load(f); },
                        lexicon, infile);
  } else {
    std::cout << "Making lexicon from " << infile_name << "..." << std::flush;
    time = measure_time([](FSALexicon& l, std::ifstream& f) { l.add_file(f); },
                        lexicon, infile);
  }
  std::cout << "done! (took " << time << " seconds)" << std::endl;
  return lexicon;
}

void write_lexicon(const FSALexicon& lexicon, const std::string& outfile_name)
{
  std::ofstream outfile(outfile_name);
  std::cout << "Writing lexicon to " << outfile_name << "..." << std::flush;
  auto time = measure_time([&lexicon](std::ofstream& of) {
    lexicon.dump(of);
  }, outfile);
  std::cout << "done! (took " << time << "seconds)" << std::endl;
}

int main(int argc, char* argv[])
{
  try {
    cxxopts::Options options(argv[0], "FSA-based lexicon utility");
    bool load = false;
    options.add_options()
      ("i,infile", "Input file", cxxopts::value<std::string>())
      ("l,load", "Load a representation", cxxopts::value(load))
      ("o,outfile", "Output file", cxxopts::value<std::string>());
    options.parse(argc, argv);
    std::cout << "Infile: " << options["infile"].as<std::string>() << std::endl;
    std::cout << "Outfile: " << options["outfile"].as<std::string>() << std::endl;
    std::cout << std::boolalpha;
    std::cout << "Load: " << options["load"].as<bool>() << std::endl;

    auto lexicon = make_lexicon(options["infile"].as<std::string>(),
                                options["load"].as<bool>());

    std::cout << "Compacting long edges..." << std::flush;
    auto time = measure_time([](FSALexicon& l) { l.compact_long_edges(); }, lexicon);
    std::cout << "done! (took " << time << " seconds)" << std::endl;

    lexicon.compact();

//    std::cout << lexicon.debug();

//    write_lexicon(lexicon, options["outfile"].as<std::string>());

//    std::cout << "Lexicon size: " << lexicon.size() << std::endl;
//    std::cout << "Dump size: " << lexicon.dump_strings().size() << std::endl;
//    std::cout << "Debug: " << lexicon.debug() << std::endl;
//    for (auto s: lexicon.dump_strings()) {
//      std::cout << s << std::endl;
//    }
  } catch (const cxxopts::OptionException& e) {
    std::cout << "Error parsing options: " << e.what() << std::endl;
    exit(1);
  }
  return 0;
}