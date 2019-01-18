/**
 * Provide support for running a function on the standard input stream or on an
 * input file stream.
 *
 * Author: Steve Matsumoto
 */

#ifndef CAPS_IO_OPTION_H
#define CAPS_IO_OPTION_H

#include <fstream>
#include <functional>
#include <iostream>
#include <string>

/**
 * Execute a function reading from an input stream, using standard input as the
 * input stream if an input file name is provided and the file stream otherwise.
 *
 * @tparam FuncType - a function type that takes an input stream (and possibly
 *                    additional arguments).
 * @tparam Args - the type of additional arguments to the function.
 * @param f - the function to execute on the input stream and arguments.
 * @param infile_name - the name of the input file (empty for standard input).
 * @param args - additional arguments to the function.
 * @return - the return value of the function.
 */
template <typename FuncType, typename... Args>
auto input_option(FuncType f, const std::string& infile_name, Args... args)
  -> decltype(f(std::cin, args...))
{
  if (infile_name.empty()) {
    return f(std::cin, args...);
  } else {
    std::ifstream in_stream(infile_name);
    if (!in_stream.good()) {
      std::cerr << "\n\nERROR: Invalid input file: " << infile_name << ".  Using standard in instead." << std::endl;
      return f(std::cin, args...);
    } else {
      return f(in_stream, args...);
    }
    
  }
}

/**
 * Execute a function writing to an output stream, using standard output as the
 * output stream if an output file name is provided and the file stream
 * otherwise.
 *
 * @tparam FuncType - a function type that takes an output stream (and possibly
 *                    additional arguments).
 * @tparam Args - the type of additional arguments to the function.
 * @param f - the function to execute on the output stream and arguments.
 * @param outfile_name - the name of the output file (empty for standard
 *                       output).
 * @param args - additional arguments to the function.
 * @return - the return value of the function.
 */
template <typename FuncType, typename... Args>
auto output_option(FuncType f, const std::string& outfile_name, Args... args)
  -> decltype(f(std::cout, args...))
{
  if (outfile_name.empty()) {
    return f(std::cout, args...);
  } else {
    std::ofstream out_stream(outfile_name);
    return f(out_stream, args...);
  }
}

template <typename T>
inline T& empty_option(const T& candidate, const T& default_value)
{
  return candidate.empty() ? default_value : candidate;
}

#endif //CAPS_IO_OPTION_H
