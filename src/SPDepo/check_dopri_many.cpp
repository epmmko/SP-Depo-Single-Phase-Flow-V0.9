#include <vector>
#include <cmath>
#include "check_dopri_many.h"
#include "user_numerical_method.h"

namespace spdepo {
namespace usr {
  //solve (D^2 + D - 6)y = sin(x) + x
  //fn0: Dy0 = y1
  //fn1: Dy1 + y1 - 6 y0 = sin(x) + x
  //     Dy1 = sin(x) + x + 6 y0 - y1 
  double fn0(double x, const std::vector<double>& y) {
    return y[1];
  }
  double fn1(double x, const std::vector<double>& y) {
    return std::sin(x) + x + 6.0 * y[0] - y[1];
  }
  std::vector<double> fn_all(double x, const std::vector<double>& y) {
    std::vector<double> ans(2);
    ans[0] = y[1];
    ans[1] = std::sin(x) + x + 6.0 * y[0] - y[1];
    return ans;
  }
  void CheckDopriMany3() {
    std::cout << "\ncheck Dormand Prince Many3\n\n";
    long num_fn_call;
    std::vector<double> ans_x;
    TwoDVector<double> ans_y;
    std::vector<double> err_vec = {1.0e-6, 1.0e-6};
    std::vector<double> ic = { 1, -2.98710141 };
    double a = 0;
    double b = 1;
    
    DormandPrinceMany(fn_all, 2, &num_fn_call, &ans_x, &ans_y, ic, a, b, 0.1, err_vec);
    for (int i = 0; i < ans_x.size(); i++) {
      std::cout << i << "  " << ans_x[i] << "  " << ans_y[i][0]
        << "  " << ans_y[i][1] << '\n';
    }
    std::ofstream fout;
    fout.open("test_dopri_many2.csv");
    if (!fout) {
      std::cout << "cannot open test_dopri_many.csv for writing\n";
      fout.close();
      return;
    }
    fout << "\ncheck Dormand Prince Many3\n\n";
    for (int i = 0; i < ans_x.size(); i++) {
      fout << ans_x[i] << "," << ans_y[i][0]
        << "," << ans_y[i][1] << '\n';
    }
    fout.close();
    return;
  }

  void CheckDopriMany2() {
    std::cout << "\ncheck Dormand Prince Many\n\n";
    long num_fn_call;
    std::vector<double> ans_x;
    TwoDVector<double> ans_y;
    std::vector<double> ic = {1, -2.98710141};
    double a = 0;
    double b = 1;
    std::vector<double(*)(double, const std::vector<double>&)> fn = {fn0, fn1};
    DormandPrinceMany(fn, &num_fn_call, &ans_x, &ans_y, ic, a, b);
    for (int i = 0; i < ans_x.size(); i++) {
      std::cout << i << "  " << ans_x[i] << "  " << ans_y[i][0] 
                     << "  " << ans_y[i][1] << '\n';
    }
    std::ofstream fout;
    fout.open("test_dopri_many.csv");
    if (!fout) {
      std::cout << "cannot open test_dopri_many.csv for writing\n";
      fout.close();
      return;
    }
    for (int i = 0; i < ans_x.size(); i++) {
      fout << ans_x[i] << "," << ans_y[i][0]
        << "," << ans_y[i][1] << '\n';
    }
    fout.close();
    return;
  }



  void CheckDopriMany() {
    std::vector<double> x_vec = {1.0, 2.0};
    double ans;
    double t = 0;
    std::vector<double(*)(double, std::vector<double>)> fn_local_vec =
         {fn_vector1, fn_vector2, fn_vector3};
    ans = fn_all(fn_local_vec,"hello");
    std::cout << ans << '\n';
  }

  double fn_vector1(double t, std::vector<double> x) {
    return t + x.at(0) + x.at(1);
  }
  double fn_vector2(double t, std::vector<double> x) {
    return t + x.at(0) * 2 + x.at(1) * 2;
  }
  double fn_vector3(double t, std::vector<double> x) {
    return t + x.at(0) * 3 + x.at(1) * 3;
  }
  double fn_all(std::vector<double(*)(double, std::vector<double>)> fn,
    std::string name) {
    double sum = 0;
    sum = sum + fn.at(0)(1, { 1,2 });
    sum = sum + fn.at(1)(1, { 1,2 });
    sum = sum + fn.at(2)(1, { 1,2 });
    return sum;
  }
} //namespace usr
} //namespace spdepo