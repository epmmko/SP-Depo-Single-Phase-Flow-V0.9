#include "Check_library1.h"
#include <vector>
#include "user_numerical_method.h"
#include "user_short_name.h"
#include <fstream>
#include <iostream>
#include <numeric>
#include <iomanip>
namespace spdepo{
namespace test{
  double v_slope(double x, double y);
}
void CheckLibrary1()
{
  std::vector<double> a = { 0, -1, -2, -3, -4, -5, -6 };
  std::vector<double> b = { 8,  9, 10, 11, 12, 13, 14 };
  std::vector<double> c = { -4, -5, -6, -7, -8, -9,  0 };
  std::vector<double> d = { 2,  3,  4,  5,  6,  7,  8 };
  std::vector<double> x(7, 0.0);
  usr::ThomasTridiagonal(a, b, c, d, x, 7);
  std::cout.precision(15);
  for (auto item : x) {
    std::cout << item << '\n';
  }

  usr::TwoDVector<double> y;
  usr::TridiagonalToDense(a, b, c, &y);
  std::cout.fixed;
  for (auto row : y) {
    for (auto item : row) {
      std::cout << std::setw(3) << item;
    }
    std::cout << '\n';
  }

  usr::TwoDVector<double> x_td(7, std::vector<double>(1, 0));
  for (int i = 0; i < x_td.size(); i++) {
    x_td[i][0] = x[i];
  }
  usr::TwoDVector<double> check_ans;
  usr::MatrixMultiply2D(y, x_td, check_ans);

  for (auto row : check_ans) {
    for (auto item : row) {
      std::cout << item << "  ";
    }
    std::cout << '\n';
  }
  std::cout << "\n\n\n";
  //usr::operator<<(std::cout, check_ans);
  std::cout << check_ans;

  usr::TwoDVector<double> mul_1 = {{1,2,3},{4,5,6},{-1,-2,-3}};
  usr::TwoDVector<double> mul_2 = {{7,8},{5,6},{3,4}};
  usr::TwoDVector<double> mul_3;
  usr::MatrixMultiply2D(mul_1,mul_2,mul_3);
  std::cout << "\n\n\n";
  std::cout << mul_3;
  std::vector<int> test1(100);
  std::iota (test1.begin(), test1.end(), 0);
  for (auto i : test1) {
    std::cout << i << '\n';
  }

  std::vector<double> x_out, y_out;
  long num_fn_call;
  usr::DormandPrince1D(test::v_slope,&num_fn_call,&x_out,
      &y_out,1,0,0.999999, 0.3,1.0e-6);
  std::cout << "done < v-discretization with DOPRI54\n";
  std::ofstream debug_v;
  debug_v.open("xy_debug.csv");
  for (int i = 0; i < x_out.size(); i++) {
    debug_v << x_out[i] << "," << y_out[i] << '\n';
  }
  debug_v.close();
}

double test::v_slope(double x, double y) {
  return -1.0 / 7.0 * std::pow((1.0 - x),-6.0/7.0);
}
} // namespace spdepo