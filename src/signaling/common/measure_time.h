//
// Created by smaptas on 26.11.17.
//

#ifndef CAPS_MEASURE_TIME_H
#define CAPS_MEASURE_TIME_H

#include <chrono>
#include <functional>
#include <type_traits>
#include <utility>

/**
 * Class to measure the amount of time taken by the execution of a function.
 *
 * @tparam ReturnType - the function's return type.
 * @tparam Args - the types of the arguments to the function.
 */
template <typename ReturnType, typename... Args>
class FunctionTimer
{
 public:

  explicit FunctionTimer(std::function<ReturnType(Args...)> function)
    : function_{function}, time_{0}
  {
    // Nothing to do here.
  }

  template <typename R = ReturnType>
  typename std::enable_if<std::is_void<R>::value, R>::type run(Args... args)
  {
    namespace chr = std::chrono;
    auto start_time = chr::high_resolution_clock::now();
    function_(std::forward<Args>(args)...);
    time_ = chr::duration_cast<chr::duration<double>>(
      chr::high_resolution_clock::now() - start_time).count();
  }

  template <typename R = ReturnType>
  typename std::enable_if<!std::is_void<R>::value, R>::type run(Args... args)
  {
    namespace chr = std::chrono;
    auto start_time = chr::high_resolution_clock::now();
    auto return_val = function_(std::forward<Args>(args)...);
    time_ = chr::duration_cast<chr::duration<double>>(
      chr::high_resolution_clock::now() - start_time).count();
    return return_val;
  }

  std::function<ReturnType(Args...)> function() const
  {
    return function_;
  }

  double time() const
  {
    return time_;
  }

 private:
  std::function<ReturnType(Args...)> function_;
  double time_;
};

#endif //CAPS_MEASURE_TIME_H
