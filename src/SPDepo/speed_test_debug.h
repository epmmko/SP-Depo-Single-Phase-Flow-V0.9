#ifndef SPEED_TEST_DEBUG
#define SPEED_TEST_DEBUG

#include <vector>
namespace spdepo {
namespace debug {

void SpeedTestMain();

void CallVectorSlice1(long n, long iteration_number);
void CallVectorSlice2(long n, long iteration_number);

void VectorSlice1(const std::vector<double>& input_vec);
void VectorSlice2(const std::vector<double>& input_vec);

} //namespace debug
} //namespace spdepo
#endif
