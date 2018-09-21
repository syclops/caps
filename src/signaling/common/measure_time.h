//
// Created by smaptas on 26.11.17.
//

#ifndef CAPS_MEASURE_TIME_H
#define CAPS_MEASURE_TIME_H

#include <chrono>
#include <functional>
#include <type_traits>
#include <utility>

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

//template <typename FuncType, typename... Args>
//class FTimer
//{
// public:
//  explicit FTimer(FuncType function)
//    : function_{function}, time_{0}
//  {
//    // Nothing to do here.
//  }
//
//  template <typename R = std::result_of<FuncType>>
//  typename std::enable_if<std::is_void<R>::value, R>::type run(Args... args)
//  {
//    namespace chr = std::chrono;
//    auto start_time = chr::high_resolution_clock::now();
//    function_(std::forward<Args>(args)...);
//    auto end_time = chr::high_resolution_clock::now();
//    time_ = chr::duration_cast<chr::duration<double>>(
//      chr::high_resolution_clock::now() - start_time).count();
//  }
//
//  template <typename R = std::result_of<FuncType>>
//  typename std::enable_if<!std::is_void<R>::value, R>::type run(Args... args)
//  {
//    namespace chr = std::chrono;
//    auto start_time = chr::high_resolution_clock::now();
//    auto return_val = function_(std::forward<Args>(args)...);
//    auto end_time = chr::high_resolution_clock::now();
//    time_ = chr::duration_cast<chr::duration<double>>(
//      chr::high_resolution_clock::now() - start_time).count();
//    return return_val;
//  }
//
//  FuncType function() const
//  {
//    return function_;
//  }
//
//  double time() const
//  {
//    return time_;
//  }
//
// private:
//  FuncType function_;
//  double time_;
//};

//template <typename ReturnType>
//struct timer_pair
//{
//  timer_pair()
//    : time_{0}, value_{}
//  {
//    // Nothing to do here.
//  }
//
//  timer_pair(double time, ReturnType value)
//    : time_{time}, value_{value}
//  {
//    // Nothing to do here.
//  }
//
//  double time() const
//  {
//    return time_;
//  }
//
//  ReturnType value() const
//  {
//    return value_;
//  }
//
//  double time_;
//  ReturnType value_;
//};
//
//template <>
//struct timer_pair<void>
//{
//  timer_pair()
//    : time_{0}
//  {
//    // Nothing to do here.
//  }
//
//  explicit timer_pair(double time)
//    : time_{time}
//  {
//    // Nothing to do here.
//  }
//
//  double time() const
//  {
//    return time_;
//  }
//
//  double time_;
//};
//
//template <typename FuncType, typename... Args>
//auto measure_time(FuncType function, Args&&... args)
//  -> timer_pair<decltype(function(args...))>
//{
//  namespace chr = std::chrono;
//  auto start_time = chr::high_resolution_clock::now();
//  auto return_val = function(std::forward<Args>(args)...);
//  auto end_time = chr::high_resolution_clock::now();
//  return timer_pair<decltype(return_val)>(chr::duration_cast<chr::duration<double>>(
//      chr::high_resolution_clock::now() - start_time).count(), return_val);
//}

#endif //CAPS_MEASURE_TIME_H
