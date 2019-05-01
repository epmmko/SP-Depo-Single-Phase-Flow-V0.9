#include "user_equation_format.h"
#include <iostream>
#include <iomanip>
#include <chrono>
namespace spdepo {
  double WrapperFunction(double x);
  double test_function(double x);
  void CheckBigEquation() {
    usr::Equation eq0, eq1, eq2;
    eq0.SetUserDefinedFunction(WrapperFunction);
    eq0.SetId(-1);
    std::cout << std::setprecision(15) << eq0.At(2) << '\n';

    eq1.SetId(1);
    eq1.SetCoefficient({2,1}); // y = 2 * x + 1
    eq2.SetId(2);
    eq2.SetCoefficient({0.5, 0, -1.0}); // y = 0.5 * x^2 + 0 * x - 1.0
    usr::BigEquation beq1;
    usr::UserOperator op1;
    op1.SetId(6); // 
    beq1.ClearOperator();
    beq1.AppendEquation(eq1);
    beq1.AppendEquation(eq2);
    beq1.AppendOperator(op1);  // y = (0.5 * x^2 + 0 * x - 1.0) / (2 * x + 1)
    for (double i = 0; i <= 10; i = i + 0.5) {
      std::cout << std::setw(10) << i << std::setw(18) << std::setprecision(10) << beq1.At(i) << '\n';
    }
    double test;
    double time_bigeq;
    double time_hardcode;
    test = 0;
    std::cout << "Time for Big Equation 10000 Evaluation\n";
    auto start_big_eq = std::chrono::high_resolution_clock::now();
    for (int i = 1; i < 10000; i++) {
      test = beq1.At(i) + test;
    }
    auto end_big_eq = std::chrono::high_resolution_clock::now();
    std::cout << "test value = " << test << '\n';
    std::cout << "10000-loop of big equation took: ";
    time_bigeq = std::chrono::duration_cast<std::chrono::nanoseconds>(
                 end_big_eq - start_big_eq).count();
    std::cout << time_bigeq << '\n';

    test = 0;
    std::cout << "Time for hard-code Equation 10000 Evaluation\n";
    auto start_hc_eq = std::chrono::high_resolution_clock::now();
    for (int i = 1; i < 10000; i++) {
      test = test_function(i) + test;
    }
    auto end_hc_eq = std::chrono::high_resolution_clock::now();
    std::cout << "test value = " << test << '\n';
    std::cout << "10000-loop of hard-code equation took: ";
    time_hardcode = std::chrono::duration_cast<std::chrono::nanoseconds>(
                    end_hc_eq - start_hc_eq).count();
    std::cout << time_hardcode << '\n';
    std::cout << "hard-code is " << time_bigeq / time_hardcode << " times faster than big-eq\n";
  }
  double WrapperFunction(double x) {
    return x + 75.0 + pow(x,0.5);
  }
  double test_function(double x) {
    return (0.5 * x * x + 0 * x - 1.0) / (2.0 * x + 1.0);
  }
} //namespace spdepo