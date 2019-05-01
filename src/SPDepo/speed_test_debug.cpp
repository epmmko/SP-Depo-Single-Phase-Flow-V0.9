#include "speed_test_debug.h"
#include <vector>
#include <chrono>

namespace spdepo{
namespace debug{

void SpeedTestMain() {
  CallVectorSlice1(1000, 100);
  //CallVectorSlice1(1000, 100);
  //take 1330 microsecond for debug  mode (103 times faster)
  //take   49 microsecond for relese mode ( 11 times faster)


  CallVectorSlice2(1000, 100);
  //CallVectorSlice2(1000, 100);
  //take 137547 microsecond for debug  mode
  //take    540 microsecond for relese mode

}

void CallVectorSlice1(long n, long iteration_number) {
  std::vector<double> input_vec(n/2, 2.01);
  std::vector<double> input_temp(n - n / 2, -5.2);
  input_vec.insert(input_vec.end(),input_temp.begin(), input_temp.end());

  auto start_time = std::chrono::high_resolution_clock::now();
  for (long i = 0; i < iteration_number; i++) {
    VectorSlice1(input_vec);
  }
  auto end_time = std::chrono::high_resolution_clock::now();

  std::cout << "time for insert method[micro-second]:" << std::chrono::duration_cast<std::chrono::microseconds>
    (end_time - start_time).count() << '\n';
}

void CallVectorSlice2(long n, long iteration_number) {
  std::vector<double> input_vec(n / 2, 2.01);
  std::vector<double> input_temp(n - n / 2, -5.2);
  input_vec.insert(input_vec.end(), input_temp.begin(), input_temp.end());

  auto start_time = std::chrono::high_resolution_clock::now();
  for (long i = 0; i < iteration_number; i++) {
    VectorSlice2(input_vec);
  }
  auto end_time = std::chrono::high_resolution_clock::now();

  std::cout << "time for for-loop[micro-second]:" << std::chrono::duration_cast<std::chrono::microseconds>
    (end_time - start_time).count() << '\n';
}

void VectorSlice1(const std::vector<double>& input_vec) {
  std::vector<double> ans1(input_vec.begin(), input_vec.begin() + input_vec.size()/2);
  std::vector<double> ans2(input_vec.begin() + input_vec.size() / 2, input_vec.end());
}

void VectorSlice2(const std::vector<double>& input_vec) {
  std::vector<double> ans1;
  std::vector<double> ans2;
  long n = input_vec.size();
  for (long i = 0; i < n/2; i++) {
    ans1.push_back(input_vec[i]/2);
    ans2.push_back(input_vec[i + n/2] / 2);
  }
}

} //namespace debug
} //namespace spdepo