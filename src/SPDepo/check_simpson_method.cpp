#include <vector>
#include <iostream>
#include "user_numerical_method.h"
#include <iomanip>
namespace spdepo{
void CheckSimpson13() {
  std::vector<double> x, y;
  double delta;
  double ans1;
  double ans2;
  delta = 2 * 0.01;
  for (int i = 0; i < 101; i++) {
    x.push_back(i * delta);
    y.push_back(x[i] * x[i] - 2.0 * x[i] + 1.0);
  }
  ans1 = usr::Simpson13(x,y);
  ans2 = usr::Trapezoidal(x,y);
  std::cout << std::setprecision(15);
  std::cout << std::scientific;
  std::cout << "ans1 = " << ans1 << '\n';
  std::cout << "ans2 = " << ans2 << '\n';

}
} //namespace spdepo