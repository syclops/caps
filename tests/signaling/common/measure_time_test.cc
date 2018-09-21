//
// Created by smaptas on 26.11.17.
//

#include <chrono>
#include <iostream>
#include <thread>

#include "../../../src/signaling/common/measure_time.h"

int main()
{
  std::cout << "Sleep for 1 second and print 42" << std::endl;
  auto pair = measure_time<int>([](int plus) -> int {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return plus + 42;
  }, 0);
  std::cout << "Answer: " << pair.first << std::endl;
  std::cout << "Took " << pair.second << " seconds" << std::endl;
  return 0;
}
