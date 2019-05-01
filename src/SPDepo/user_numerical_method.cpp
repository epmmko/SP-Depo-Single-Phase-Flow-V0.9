#include <math.h>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>
#include <numeric> //for iota
#include <functional>
#include <algorithm>
#include <chrono>
#include "user_numerical_method.h"
#include "user_short_name.h"
#include <omp.h>

namespace spdepo{
namespace usr {
  void EmptyCallBack(const std::vector<double>& input) {
  }

  usr::TwoDVector<double> operator*(double c, 
      const usr::TwoDVector<double>& A) {
    usr::TwoDVector<double> ans = A;
    for (int i = 0; i < ans.size(); i++) {
      std::transform(A[i].begin(), A[i].end(), ans[i].begin(),
        std::bind1st(std::multiplies<double>(), c));
    }
    return ans;
  }
  usr::TwoDVector<double> operator+(const usr::TwoDVector<double>& A,
    const usr::TwoDVector<double>& B) {
    usr::TwoDVector<double> ans = A;
    for (int i = 0; i < ans.size(); i++) {
      for (int j = 0; j < ans[0].size(); j++) {
        ans[i][j] = A[i][j] + B[i][j];
      }
    }
    return ans;
  }
  template <class T, class Q>
  std::vector <T> operator*(const Q c, const std::vector<T> &A) {

    std::vector <T> R(A.size());
    std::transform(A.begin(), A.end(), R.begin(),
      std::bind1st(std::multiplies<T>(), c));
    return R;
  }

  template <class T, class Q>
  std::vector <T> operator/(const std::vector<T> &A, const Q c) {
    std::vector <T> R(A.size());
    std::size_t n = A.size();
    for (long i = 0; i < n; i++) {
      R[i] = A[i] / c;
    }
    return R;
  }

  template <class T>
  std::vector <T> operator/(const std::vector<T> &A, const std::vector<T> &B) {
    std::vector <T> ans(A.size());
    std::size_t n = A.size();
    for (long i = 0; i < n; i++) {
      ans[i] = A[i] / B[i];
    }
    return ans;
  }

  template <class T>
  std::vector<T> operator+ (const std::vector<T>& left,
                            const std::vector<T>& right) {
    std::size_t n = left.size();
    std::vector<T> ans(n);
    for (long i = 0; i < left.size(); i++) {
      ans[i] = left[i] + right[i];
    }
    return ans;
  }

  template <class T>
  std::vector<T> operator- (const std::vector<T>& left,
    const std::vector<T>& right) {
    std::size_t n = left.size();
    std::vector<T> ans(n);
    for (long i = 0; i < n; i++) {
      ans[i] = left[i] - right[i];
    }
    return ans;
  }

  double abs(const std::vector<double> &input) {
    //give norm of the vector
    double sum = 0;
    long n = input.size();
    for (long i = 0; i < n; i++) {
      sum = sum + input[i] * input[i];
    }
    return std::sqrt(sum);
  }

  std::vector<double> abs_vec(const std::vector<double>& input) {
    std::vector<double> ans(input.size());
    long n = input.size();
    for (long i = 0; i < n; i++) {
      ans[i] = std::abs(input[i]);
    }
    return ans;
  }

  std::vector<double> pow_vec(const std::vector<double>& input, double b) {
    std::vector<double> ans(input.size());
    long n = input.size();
    for (long i = 0; i < n; i++) {
      ans[i] = std::pow(input[i], b);
    }
    return ans;
  }

  double BisectionMethod(double(*f)(double), double left, double right,
    bool& success, int& number_of_iteration,
    double& fx_final, double eps) {
    //if f(x) is discontinuous around f(x) = 0, x value is also returned
    double mid;
    double f_mid, f_left, f_right;
    success = false;

    for (number_of_iteration = 0; number_of_iteration <= 100;
         number_of_iteration++) {
      mid = (left + right) / 2;
      f_mid = (*f)(mid);
      if (fabs(f_mid) <= eps) {
        success = true;
        fx_final = f_mid;
        break;
      }
      else if (fabs((right-left)/f_mid) <= eps){
        success = true;
        fx_final = f_mid;
        break;
      }
      f_left = (*f)(left);
      f_right = (*f)(right);
      if ((f_left > 0 && f_right > 0) || (f_left < 0 && f_right < 0)) {
        std::cout << "usr::BisectionMethod:";
        std::cout << "no root exist in this interval\n";
        return std::numeric_limits<double>::signaling_NaN();
      }
      else if ((f_mid <= 0 && f_right > 0) || (f_mid >= 0 && f_right < 0)) {
        left = mid;
      }
      else if ((f_mid <= 0 && f_left > 0) || (f_mid >= 0 && f_left < 0)) {
        right = mid;
      }
    }
    return mid;
  }

  double NewtonMethod(double(*f)(double), double x, bool& success,
                      int& number_of_iteration, double& fx_final,
                      int quit_if_not_improve,
                      double eps)
  {
    double slope, f_val, fabs_f_val, fabs_old_f_val = 1.0e+10;
    int consecutive_unimprovement = 0;
    double eps_finite_difference = eps / 10.0;
    success = false;

    for (number_of_iteration = 0; number_of_iteration <= 100; 
         number_of_iteration++) {
      f_val = (*f)(x);
      fabs_f_val = fabs(f_val);
      if (fabs_f_val <= eps) {
        success = true;
        fx_final = f_val;
        break;
      }
      if (fabs_f_val < fabs_old_f_val) {
        fabs_old_f_val = fabs_f_val;
        consecutive_unimprovement = 0;
      } else {
        consecutive_unimprovement++;
      }
      if (consecutive_unimprovement >= quit_if_not_improve) {
        fx_final = f_val;
        return x;
      }

      slope = ((*f)(x + eps_finite_difference) - f_val) / 
              eps_finite_difference;
      x = x - f_val / slope;

    }
    return x;
  }

  void InitialGuessScan(double(*f)(double), double& left_value,
                        double& right_value, int n_interval) {
    double* f_matrix = new double[n_interval];
    double* x_matrix = new double[n_interval];
    double max_value = -std::numeric_limits<double>::infinity();
    double max_x = -1;
    const double delta(1.0/n_interval);

    for (int i = 1; i < n_interval; i++) {
      x_matrix[i] = delta * i;
      f_matrix[i] = (*f)(x_matrix[i]);
      if (f_matrix[i] <= 0 && max_value < f_matrix[i]) {
        max_value = f_matrix[i];
        max_x = x_matrix[i];
      }
    }

    left_value = max_x;
    if (left_value + delta > (1.0 - 1.0e-6)) {
      right_value = 1.0 - 1.0e-6;
    }
    else {
      right_value = left_value + delta;
    }
    delete [] f_matrix;
    delete [] x_matrix;

  }

  double TryNewtonThenBisection(double(*f)(double), double x, 
      bool& success, int& number_of_iteration, double& fx_final,
      int n_scan, int quit_if_not_improve, double eps) {
    double ans;
    ans = NewtonMethod(*f,  x, success, number_of_iteration,  fx_final,
        quit_if_not_improve = 4, eps = 1.0e-7);
    if (success) {
      return ans;
    }
    double left, right;
    InitialGuessScan(*f,left,right,n_scan);
    ans = BisectionMethod(*f, left, right, success, number_of_iteration,
        fx_final, eps);
    if (success) {
      return ans;
    }
    std::cout << "no answer from both Newton and bisection method\n";
    return std::numeric_limits<double>::signaling_NaN();
  }

  void ThomasTridiagonal(const std::vector<double>& a_vec,
                         const std::vector<double>& b_vec,
                         const std::vector<double>& c_vec,
                         const std::vector<double>& d_vec,
                         std::vector<double>& x,
                         int n){
    std::vector<double> a(a_vec);
    std::vector<double> b(b_vec);
    std::vector<double> c(c_vec);
    std::vector<double> d(d_vec);
    c[0] = c[0]/b[0];
    for (int i = 1; i < n; i++) {
      c[i] = c[i] / (b[i] - a[i] * c[i - 1]);
    }
    d[0] = d[0] / b[0];
    for (int i = 1; i < n; i++) {
      d[i] = (d[i] - a[i] * d[i - 1]) /
        (b[i] - a[i] * c[i - 1]);
    }
    x[n-1] = d[n-1];
    for (int i = n-2; i >= 0; i--) {
      x[i] = d[i] - c[i] * x[i + 1];
    }
  }

  void TridiagonalToDense(const std::vector<double>& a,
                          const std::vector<double>& b,
                          const std::vector<double>& c,
                          std::vector<std::vector<double>>* dense) {
    int n = b.size();
    std::vector<std::vector<double>> ans (n, std::vector<double>(n,0));
    for (int i = 0; i < n; i++) {
      ans[i][i] = b[i];
    }
    for (int i = 1; i < n; i++) {
      ans[i][i-1] = a[i];
    }
    for (int i = 0; i < n - 1; i++) {
      ans[i][i+1] = c[i];
    }
    *dense = ans;
  }

  void MatrixMultiply2D(const TwoDVector<double>& a, 
      const TwoDVector<double>& b,
      TwoDVector<double>& c) {
    //multiply m x n with n x p to get m x p matrix
    /*validate with
    [1  2   3]   [7 8]   [ 26   32]
    [4  5   6] * [5 6] = [ 71   86]
    [-1 -2 -3]   [3 4]   [-26  -32]
    */
    int m,n,p;
    m = a.size();
    n = a[0].size();
    p = b[0].size();
    c.resize(m,std::vector<double> (p,0));
    if (n != b.size()) {
      std::cout << "mismatch dimension. Cannot do multiplication\n";
    }
    else {
      double sum;
      for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
          sum = 0.0;
          for (int k = 0; k < n; k++) {
            sum = sum + a[i][k] * b[k][j];
          }
          c[i][j] = sum;
        }
      }
    }
  }
  void MatrixMultiply2D(const TwoDVector<double>& a,
                        const std::vector<double>& b,
                        std::vector<double>& c) {
    //multiply m x n with n x 1 to get m x 1 matrix
    //output is c
    int m, n;
    m = a.size();
    n = a[0].size();
    c.resize(m, 0.0);
    if (n != b.size()) {
      std::cout << "mismatch dimension. Cannot do multiplication\n";
      std::cout << "pause\n";
    }
    else {
      double sum;
      for (int i = 0; i < m; i++) {
        sum = 0.0;
        for (int k = 0; k < n; k++) {
          sum = sum + a[i][k] * b[k];
        }
        c[i] = sum;
      }
    }
  }
  void DormandPrince1D(double(*fn)(double, double),
      long* num_fn_call,
      std::vector<double>* x,
      std::vector<double>* y,
      double ic, double a, double b, double h,
      double eps, int max_num_interval,
      int max_iteration,
      double step_ratio_max,
      double abs_hmax,
      double abs_hmin,
      bool exact_final_point,
      bool suppress_warning){

    double c2,c3,c4,c5,c6,c7;
    double a21, a31, a32, a41, a42, a43;
    double a51, a52, a53, a54;
    double a61, a62, a63, a64, a65;
    double a71, a72, a73, a74, a75, a76;
    double b11, b12, b13, b14, b15, b16, b17;
    double b21, b22, b23, b24, b25, b26, b27;
    double s = 1.0; //step size
    double yi, xi;
    double err, y5, y4;
    double k1,k2,k3,k4,k5,k6,k7;
    bool success_s_search;
    bool end_all_loop = false;
    *num_fn_call = 0; //total number of the function call
    c2 = 1.0 / 5;
    c3 = 3.0 / 10;
    c4 = 4.0 / 5;
    c5 = 8.0 / 9.0;
    c6 = 1.0;
    c7 = 1.0;
    a21 = 1.0 / 5;
    a31 = 3.0 / 40;
    a32 = 9.0 / 40;
    a41 = 44.0 / 45;
    a42 = -56.0 / 15;
    a43 = 32.0 / 9;
    a51 = 19372.0 / 6561;
    a52 = -25360.0 / 2187;
    a53 = 64448.0 / 6561;
    a54 = -212.0 / 729;
    a61 = 9017.0 / 3168;
    a62 = -355.0 / 33;
    a63 = 46732.0 / 5247;
    a64 = 49.0 / 176;
    a65 = -5103.0 / 18656;
    a71 = 35.0 / 384;
    a72 = 0.0;
    a73 = 500.0 / 1113;
    a74 = 125.0 / 192;
    a75 = -2187.0 / 6784;
    a76 = 11.0 / 84;
    b11 = 35.0 / 384;
    b12 = 0.0;
    b13 = 500.0 / 1113;
    b14 = 125.0 / 192;
    b15 = -2187.0 / 6784;
    b16 = 11.0 / 84;
    b17 = 0.0;
    b21 = 5179.0 / 57600;
    b22 = 0.0;
    b23 = 7571.0 / 16695;
    b24 = 393.0 / 640;
    b25 = -92097.0 / 339200;
    b26 = 187.0 / 2100;
    b27 = 1.0 / 40;
    
    y->push_back(ic);
    x->push_back(a);
    yi = (*y)[0];
    xi = (*x)[0];
    k1 = fn(xi,yi);
    *num_fn_call = 1;
    for (int i = 0; i < max_num_interval; i++) {
      yi = (*y)[i];
      xi = (*x)[i];
      if (std::abs(xi - b) < 1.0e-14) {
        break;
      }
      success_s_search = false;
      for (int j = 0; j < max_iteration; j++) {
        if (!exact_final_point) { //skip the last point
          //do the the check based on previous h (before h*s)
          if (h < 0) {            
            if (xi + h < b) {
              end_all_loop = true;
              break;
            }
          }
          else {
            if (xi + h > b) {
              end_all_loop = true;
              break;
            }
          }
        }
        h = h * s;
        if (h < 0) {
          if (xi + h < b) {
            h = b - xi;
          }
          if (h < -abs_hmax) {
            h = -abs_hmax;
          }
        }
        else {
          if (xi + h > b) {
            h = b - xi;
          }
          if (h > abs_hmax) {
            h = abs_hmax;
          }
        }
        if (std::abs(h) < abs_hmin) {
          h = h / std::abs(h) * abs_hmin;
        }

        k2 = fn(xi + c2 * h, yi + h * (a21*k1));
        k3 = fn(xi + c3 * h, yi + h * (a31*k1 + a32 * k2));
        k4 = fn(xi + c4 * h, yi + h * (a41*k1 + a42 * k2 + a43 * k3));
        k5 = fn(xi + c5 * h, yi + h * (a51*k1 + a52 * k2 + a53 * k3
          + a54 * k4));
        k6 = fn(xi + c6 * h, yi + h * (a61*k1 + a62 * k2 + a63 * k3
          + a64 * k4 + a65 * k5));
        k7 = fn(xi + c7 * h, yi + h * (a71*k1 + a72 * k2 + a73 * k3
          + a74 * k4 + a75 * k5 + a76 * k6));
        *num_fn_call = *num_fn_call + 6;
        y5 = yi + h * (b11*k1 + b12 * k2 + b13 * k3 + b14 * k4
          + b15 * k5 + b16 * k6);
        y4 = yi + h * (b21*k1 + b22 * k2 + b23 * k3 + b24 * k4
          + b25 * k5 + b26 * k6 + b27 * k7);

        err = std::abs(y5 - y4);
        if (err < 1.0e-20) {
          s = step_ratio_max;
        }
        else {
          s = std::pow(std::abs(eps*h / 2.0 / err),0.2);
        }
        if (s >= 1) {// accept the solution
          if (s > step_ratio_max) {
            s = step_ratio_max;
          }
          y->push_back(y5);
          x->push_back(xi+h);
          k1 = k7;
          success_s_search = true;
          break;
        }
        if (s < 0.01) {
          s = 0.01;
        }
      }
      if (end_all_loop) {
        break;
      }
      if (!success_s_search) {
        if(!suppress_warning){
        std::cout << "cannot find the answer within ";
        std::cout << max_iteration << " iteration\n";
        }
        y->push_back(y5);
        x->push_back(xi+h);
      }
    }
  }
  //first version, take vector of function that return double
  void DormandPrinceMany(
    const std::vector<double(*)(double,const std::vector<double>&)>& fn,
    long* num_fn_call,
    std::vector<double>* x,
    TwoDVector<double>* y,
    std::vector<double> ic, double a, double b, double h,
    double eps, int max_num_interval,
    int max_iteration,
    double step_ratio_max,
    double abs_hmax,
    double abs_hmin,
    bool exact_final_point,
    bool suppress_warning) {

    long n_eq = fn.size();
    double c2, c3, c4, c5, c6, c7;
    double a21, a31, a32, a41, a42, a43;
    double a51, a52, a53, a54;
    double a61, a62, a63, a64, a65;
    double a71, a72, a73, a74, a75, a76;
    double b11, b12, b13, b14, b15, b16, b17;
    double b21, b22, b23, b24, b25, b26, b27;
    double s = 1.0; //step size
    std::vector<double> yi;
    double xi;
    double err;
    std::vector<double> y5, y4;
    std::vector<double> k1(n_eq, 0.0);
    std::vector<double> k2(n_eq, 0.0);
    std::vector<double> k3(n_eq, 0.0);
    std::vector<double> k4(n_eq, 0.0);
    std::vector<double> k5(n_eq, 0.0);
    std::vector<double> k6(n_eq, 0.0);
    std::vector<double> k7(n_eq, 0.0);

    bool success_s_search;
    bool end_all_loop = false;
    *num_fn_call = 0; //total number of the function call
    c2 = 1.0 / 5;
    c3 = 3.0 / 10;
    c4 = 4.0 / 5;
    c5 = 8.0 / 9.0;
    c6 = 1.0;
    c7 = 1.0;
    a21 = 1.0 / 5;
    a31 = 3.0 / 40;
    a32 = 9.0 / 40;
    a41 = 44.0 / 45;
    a42 = -56.0 / 15;
    a43 = 32.0 / 9;
    a51 = 19372.0 / 6561;
    a52 = -25360.0 / 2187;
    a53 = 64448.0 / 6561;
    a54 = -212.0 / 729;
    a61 = 9017.0 / 3168;
    a62 = -355.0 / 33;
    a63 = 46732.0 / 5247;
    a64 = 49.0 / 176;
    a65 = -5103.0 / 18656;
    a71 = 35.0 / 384;
    a72 = 0.0;
    a73 = 500.0 / 1113;
    a74 = 125.0 / 192;
    a75 = -2187.0 / 6784;
    a76 = 11.0 / 84;
    b11 = 35.0 / 384;
    b12 = 0.0;
    b13 = 500.0 / 1113;
    b14 = 125.0 / 192;
    b15 = -2187.0 / 6784;
    b16 = 11.0 / 84;
    b17 = 0.0;
    b21 = 5179.0 / 57600;
    b22 = 0.0;
    b23 = 7571.0 / 16695;
    b24 = 393.0 / 640;
    b25 = -92097.0 / 339200;
    b26 = 187.0 / 2100;
    b27 = 1.0 / 40;

    y->push_back(ic);
    x->push_back(a);
    yi = (*y)[0];
    xi = (*x)[0];

    for (long i = 0; i < n_eq; i++) {
      k1[i] = fn[i](xi, yi);
    }

    *num_fn_call = 1;
    for (int i = 0; i < max_num_interval; i++) {
      yi = (*y)[i];
      xi = (*x)[i];
      if (std::abs(xi - b) < 1.0e-14) {
        break;
      }
      success_s_search = false;
      for (int j = 0; j < max_iteration; j++) {
        if (!exact_final_point) { //skip the last point
                                  //do the the check based on previous h (before h*s)
          if (h < 0) {
            if (xi + h < b) {
              end_all_loop = true;
              break;
            }
          }
          else {
            if (xi + h > b) {
              end_all_loop = true;
              break;
            }
          }
        }
        h = h * s;
        if (h < 0) {
          if (xi + h < b) {
            h = b - xi;
          }
          if (h < -abs_hmax) {
            h = -abs_hmax;
          }
        }
        else {
          if (xi + h > b) {
            h = b - xi;
          }
          if (h > abs_hmax) {
            h = abs_hmax;
          }
        }
        if (std::abs(h) < abs_hmin) {
          h = h / std::abs(h) * abs_hmin;
        }
        for (long i = 0; i < n_eq; i++) {
          k2[i] = fn[i](xi + c2 * h, yi + h * (a21*k1));
        }
        for (long i = 0; i < n_eq; i++) {
          k3[i] = fn[i](xi + c3 * h, yi + h * (a31*k1 + a32 * k2));
        }
        for (long i = 0; i < n_eq; i++) {
          k4[i] = fn[i](xi + c4 * h, yi + h * (a41*k1 + a42 * k2 + a43 * k3));
        }
        for (long i = 0; i < n_eq; i++) {
          k5[i] = fn[i](xi + c5 * h, yi + h * (a51*k1 + a52 * k2 + a53 * k3
            + a54 * k4));
        }
        for (long i = 0; i < n_eq; i++) {
          k6[i] = fn[i](xi + c6 * h, yi + h * (a61*k1 + a62 * k2 + a63 * k3
            + a64 * k4 + a65 * k5));
        }
        for (long i = 0; i < n_eq; i++){
          k7[i] = fn[i](xi + c7 * h, yi + h * (a71*k1 + a72 * k2 + a73 * k3
            + a74 * k4 + a75 * k5 + a76 * k6));
        }
        *num_fn_call = *num_fn_call + 6;
        y5 = yi + h * (b11*k1 + b12 * k2 + b13 * k3 + b14 * k4
          + b15 * k5 + b16 * k6);
        y4 = yi + h * (b21*k1 + b22 * k2 + b23 * k3 + b24 * k4
          + b25 * k5 + b26 * k6 + b27 * k7);

        err = spdepo::usr::abs(y5 - y4);
        if (err < 1.0e-20) {
          s = step_ratio_max;
        }
        else {
          s = std::pow(std::abs(eps*h / 2.0 / err), 0.2);
        }
        if (s >= 1) {// accept the solution
          if (s > step_ratio_max) {
            s = step_ratio_max;
          }
          y->push_back(y5);
          x->push_back(xi + h);
          k1 = k7;
          success_s_search = true;
          break;
        }
        if (s < 0.01) {
          s = 0.01;
        }
      }
      if (end_all_loop) {
        break;
      }
      if (!success_s_search) {
        if (!suppress_warning) {
          std::cout << "cannot find the answer within ";
          std::cout << max_iteration << " iteration\n";
        }
        y->push_back(y5);
        x->push_back(xi + h);
      }
    }


  }

  //overloaded version, take function that return vector<double>
  //This one is used in SP-Depo
  void DormandPrinceMany(
    std::vector<double>(*fn)(double, const std::vector<double>&),
    long number_of_ode,
    long* num_fn_call,
    std::vector<double>* x,
    TwoDVector<double>* y,
    std::vector<double> ic, double a, double b, double h,
    std::vector<double> eps,
      //vector of eps use in the comparison
      //this is needed because fw and ri
      //has different unit / magnitude of error

    int max_num_interval,
    int max_iteration,
    double step_ratio_max,
    double abs_hmax,
    double abs_hmin,
    bool exact_final_point,
    bool suppress_warning,
    bool show_time_in_process) {

    long n_eq = number_of_ode;
    double c2, c3, c4, c5, c6, c7;
    double a21, a31, a32, a41, a42, a43;
    double a51, a52, a53, a54;
    double a61, a62, a63, a64, a65;
    double a71, a72, a73, a74, a75, a76;
    double b11, b12, b13, b14, b15, b16, b17;
    double b21, b22, b23, b24, b25, b26, b27;
    double s = 1.0; //step size
    std::vector<double> yi;
    double xi;
    double err;
    std::vector<double> y5, y4;
    std::vector<double> s_vec(n_eq);
    std::vector<double> err_vec(n_eq, 0.0);
    std::vector<double> k1(n_eq, 0.0);
    std::vector<double> k2(n_eq, 0.0);
    std::vector<double> k3(n_eq, 0.0);
    std::vector<double> k4(n_eq, 0.0);
    std::vector<double> k5(n_eq, 0.0);
    std::vector<double> k6(n_eq, 0.0);
    std::vector<double> k7(n_eq, 0.0);

    bool success_s_search;
    bool end_all_loop = false;

    
    auto start_time = std::chrono::high_resolution_clock::now();
    

    *num_fn_call = 0; //total number of the function call
    c2 = 1.0 / 5;
    c3 = 3.0 / 10;
    c4 = 4.0 / 5;
    c5 = 8.0 / 9.0;
    c6 = 1.0;
    c7 = 1.0;
    a21 = 1.0 / 5;
    a31 = 3.0 / 40;
    a32 = 9.0 / 40;
    a41 = 44.0 / 45;
    a42 = -56.0 / 15;
    a43 = 32.0 / 9;
    a51 = 19372.0 / 6561;
    a52 = -25360.0 / 2187;
    a53 = 64448.0 / 6561;
    a54 = -212.0 / 729;
    a61 = 9017.0 / 3168;
    a62 = -355.0 / 33;
    a63 = 46732.0 / 5247;
    a64 = 49.0 / 176;
    a65 = -5103.0 / 18656;
    a71 = 35.0 / 384;
    a72 = 0.0;
    a73 = 500.0 / 1113;
    a74 = 125.0 / 192;
    a75 = -2187.0 / 6784;
    a76 = 11.0 / 84;
    b11 = 35.0 / 384;
    b12 = 0.0;
    b13 = 500.0 / 1113;
    b14 = 125.0 / 192;
    b15 = -2187.0 / 6784;
    b16 = 11.0 / 84;
    b17 = 0.0;
    b21 = 5179.0 / 57600;
    b22 = 0.0;
    b23 = 7571.0 / 16695;
    b24 = 393.0 / 640;
    b25 = -92097.0 / 339200;
    b26 = 187.0 / 2100;
    b27 = 1.0 / 40;

    y->push_back(ic);
    x->push_back(a);
    yi = (*y)[0];
    xi = (*x)[0];

    k1 = fn(xi, yi);

    *num_fn_call = 1;
    for (int i = 0; i < max_num_interval; i++) {
      yi = (*y)[i];
      xi = (*x)[i];
      if (std::abs(xi - b) < 1.0e-14) {
        break;
      }
      success_s_search = false;
      for (int j = 0; j < max_iteration; j++) {
        if (!exact_final_point) { //skip the last point
                                  //do the the check based on previous h (before h*s)
          if (h < 0) {
            if (xi + h < b) {
              end_all_loop = true;
              break;
            }
          }
          else {
            if (xi + h > b) {
              end_all_loop = true;
              break;
            }
          }
        }
        h = h * s;
        if (h < 0) {
          if (xi + h < b) {
            h = b - xi;
          }
          if (h < -abs_hmax) {
            h = -abs_hmax;
          }
        }
        else {
          if (xi + h > b) {
            h = b - xi;
          }
          if (h > abs_hmax) {
            h = abs_hmax;
          }
        }
        if (std::abs(h) < abs_hmin) {
          h = h / std::abs(h) * abs_hmin;
        }
        k2 = fn(xi + c2 * h, yi + h * (a21*k1));
        k3 = fn(xi + c3 * h, yi + h * (a31*k1 + a32 * k2));
        k4 = fn(xi + c4 * h, yi + h * (a41*k1 + a42 * k2 + a43 * k3));
        k5 = fn(xi + c5 * h, yi + h * (a51*k1 + a52 * k2 + a53 * k3
            + a54 * k4));
        k6 = fn(xi + c6 * h, yi + h * (a61*k1 + a62 * k2 + a63 * k3
            + a64 * k4 + a65 * k5));
        k7 = fn(xi + c7 * h, yi + h * (a71*k1 + a72 * k2 + a73 * k3
            + a74 * k4 + a75 * k5 + a76 * k6));

        *num_fn_call = *num_fn_call + 6;
        y5 = yi + h * (b11*k1 + b12 * k2 + b13 * k3 + b14 * k4
          + b15 * k5 + b16 * k6);
        y4 = yi + h * (b21*k1 + b22 * k2 + b23 * k3 + b24 * k4
          + b25 * k5 + b26 * k6 + b27 * k7);

        err_vec = usr::abs_vec(y5 - y4);
        s_vec = usr::pow_vec(usr::abs_vec(h*eps / 2.0 / err_vec), 0.2);
        s = *std::min_element(s_vec.begin(), s_vec.end());

        if (s >= 1) {// accept the solution
          if (s > step_ratio_max) {
            s = step_ratio_max;
          }
          y->push_back(y5);
          x->push_back(xi + h);
          k1 = k7;
          success_s_search = true;
          if (show_time_in_process) {
            auto now_time = std::chrono::high_resolution_clock::now();
            std::cout 
              << "run-time [millisecond] = "
              << std::chrono::duration_cast<std::chrono::milliseconds>(
              now_time - start_time).count()
              << "t[hr] = " << (xi + h) / 3600.0
              << "   i = " << i << " j = " << j << '\n';
          }
          break;
        }
        if (s < 0.01) {
          s = 0.01;
        }
      }
      if (end_all_loop) {
        break;
      }
      if (!success_s_search) {
        if (!suppress_warning) {
          std::cout << "cannot find the answer within ";
          std::cout << max_iteration << " iteration\n";
        }
        y->push_back(y5);
        x->push_back(xi + h);
      }
    }
  }




  double Simpson13(std::vector<double> x, std::vector<double> y) {
    int size = x.size();
    int sec_size = size - 1;
    int start_point;
    double delta = x[1] - x[0];
    double sum;
    
    if (size != y.size()) {
      std::cout << "error in Simpson 1/3 method\n";
      std::cout << "x.size() != y.size()\n";
      std::system("pause");
      return -1;
    }
    if (!((size - 1) % 2 == 0)) {
      std::cout << "error in Simpson 1/3 method\n";
      std::cout << "the input vector does not have an even number of";
      std::cout << " the section (x.size() - 1 must be an even number)\n";
      std::system("pause");
      return -1;
    }
    sum = 0;
    for (int group = 0; group < sec_size/2; group++) {
      start_point = group * 2;
      sum = sum + y[start_point] + 4.0 * y[start_point + 1] +
            y[start_point + 2];
    }
    sum = sum * delta / 3.0;
    return sum;
  }
  double Trapezoidal(std::vector<double> x, std::vector<double> y) {
    double sum = 0.0;
    for (int i = 0; i < x.size() - 1; i++) {
      sum = sum + 0.5 * (y[i] + y[i+1]) * (x[i+1] - x[i]);
    }
    return sum;
  }

  usr::TwoDVector<double> MeshToSec(const usr::TwoDVector<double>& input_mesh) {
    //input vector has the size of (Nz,Nr) 
    //Nz row and each row has Nr column or
    //vector[size Nz][size Nr]
    //The average is over the first dimension (Nz)
    long Nz = input_mesh.size();
    long Nr = input_mesh[0].size();
    usr::TwoDVector<double> ans(Nz-1);


    for (long i = 0; i < Nz - 1; i++) {
      ans[i].assign(Nr, 0.0);
      for (long j = 0; j < Nr; j++) {
        ans[i][j] = (input_mesh[i][j] + input_mesh[i + 1][j]) / 2.0;
      }
    }
    return ans;
  }

  std::vector<double> GaussianEliminationWithBackSubstitution(
      usr::TwoDVector<double>* A) {
    int N = A->size();
    std::vector<double> x(N);
    double sum;
    //A must be N rows and N+1 columns
    
    double r;
    //Gaussian Elimination
    for (int k = 0; k < N - 1; k++) {
      for (int i = k + 1; i < N; i++) {
        r = -1.0 * (*A)[i][k] / (*A)[k][k];
        for (int j = k + 1; j < N + 1; j++) {
          (*A)[i][j] = (*A)[i][j] + r * (*A)[k][j];
        }
      }
    }
    //Back substitution
    x[N-1] = (*A)[N-1][N] / (*A)[N-1][N-1];
    for (int i = N - 2; i >= 0; i--) {
      sum = 0.0;
      for (int j = i + 1; j < N; j++) {
        sum = sum + (*A)[i][j] * x[j];
      }
      x[i] = ((*A)[i][N] - sum) / (*A)[i][i];
    }
    return x;
  }
  //LM first version, communicate with a function pointer
  std::vector<double> LevenbergMarquardt(
      std::vector<double>(*f)(const std::vector<double>& adj_params,
      const usr::TwoDVector<double>& data),
      const std::vector<double>& ini_vec,
      const usr::TwoDVector<double>& data,
      bool& success,
      double eps_for_derivative,
      double eps_stop_input,
      double mu_lm_initial) {
    double mu_lm = mu_lm_initial;
    int number_of_accepted_iteration = 0;
    double eps = eps_for_derivative;
    double eps_stop = eps_stop_input;
    double h_sse_old;
    //half sum-square-error old
    double h_sse;
      //half sum-square-error
    std::vector<double> w = ini_vec;
      //w = answer is the adjustable parameter vector
    std::vector<double> wPEps = ini_vec;
    std::vector<double> w_test = ini_vec;
    std::vector<double> dw = ini_vec;
      //w plus epsilon (only on the Jacobian calculation part)
    std::size_t n_params = ini_vec.size();
    std::size_t row_data = data.size();
    std::size_t n_y_data = data[0].size() - 1;
      //number of y column
    std::size_t m_data = row_data * n_y_data;
      //total number of data combined into 1 column
    std::vector<double> grad(n_params);
    
    usr::TwoDVector<double> jtj(
      n_params, std::vector<double>(n_params, 0.0));
    usr::TwoDVector<double> jacobian(
      m_data, std::vector<double>(n_params,0.0));
      //Jacobian is defined as
      //j = [de1/dw1, de1/dw2, ..., de1/dwn]
      //    [de2/dw1, de2/dw2, ..., de2/dwn]
      //    [    ...,     ..., ...,    ...,]
      //    [dem/dw1, dem/dw2, ..., dem/dwn]
      //  d here is a partial derivative
    usr::TwoDVector<double> jacobian_transposed(
      n_params, std::vector<double>(m_data, 0.0));
    std::vector<double> e_mat(m_data);
    std::vector<double> e_mat0(m_data);
    TwoDVector<double> solve_jtj_plus_mu_i;
      //nx(n+1) matrix to solve for Dw in
      //(jtj+mu*i) * Dw = -g
      //The most right column is -g
    usr::TwoDVector<double> identity_matrix(
      n_params, std::vector<double>(n_params,0.0));
    for (int i = 0; i < n_params; i++) {
      identity_matrix[i][i] = 1.0;
    }
    h_sse_old = 0.0;
    e_mat = (*f)(w, data);
    
    h_sse_old = SumSquare(e_mat) * 0.5;

    //outer loop for adjusting ans
    for (int i = 1; i <= 1000; i++) {
      //Jacobian calculation, start
      e_mat0 = (*f)(w, data);

      for (int j = 0; j < n_params; j++) {
        //initially, wPEps is already equal to w
        wPEps[j] = wPEps[j] + eps;
        e_mat = (*f)(wPEps, data);
        jacobian_transposed[j] = (e_mat - e_mat0)/eps;
        wPEps[j] = w[j];
      }
      jacobian = TransposeNbyMMatrix(jacobian_transposed);
      //Jacobian calculation, end
      //Grad e: g = J^T*e calculation
      MatrixMultiply2D(jacobian_transposed, e_mat0, grad);
      //Calculate JTJ (JTJ changes as the direction changes)
      MatrixMultiply2D(jacobian_transposed, jacobian, jtj);
      for (int j = 1; j <= 20; j++) { //Mu loop
        //if error decreases, decreases Mu and exit loop
        //High mu means it becomes gradient descent more
        
        //Prepare matrix for solving
        //  H * Dw = -g
        //  w_{k+1} = w_k - (H^-1 * g)_k
        //  Dw = w_{k+1} - w_k
        //  H ~ JTJ + mu * I
        solve_jtj_plus_mu_i = jtj + mu_lm * identity_matrix;
        for (int i = 0; i < n_params; i++) {
          //append grad to the right of jtj + mu * i
          solve_jtj_plus_mu_i[i].insert(
            solve_jtj_plus_mu_i[i].end(), 1, -1.0 * grad[i]);
        }
        dw = GaussianEliminationWithBackSubstitution(&solve_jtj_plus_mu_i);
        w_test = w + dw;
        e_mat0 = (*f)(w_test, data);
        
        h_sse = SumSquare(e_mat0) * 0.5;
        if (h_sse < h_sse_old) {
          //accept the new weight answer
          number_of_accepted_iteration += 1;
          w = w_test;
          wPEps = w;
/* turn off for speed test
          for(int i = 0; i < w.size(); i++){
            std::cout << "w[" << i << "] = " << w[i] << '\n';
          }
          std::cout << "mu_lm = " << mu_lm << '\n';
          std::cout << "SSE = " << h_sse * 2.0 << '\n';
          std::cout << "abs(dw) = " << usr::abs(dw) << '\n';
          std::cout << "number of accepted iteration = "
            << number_of_accepted_iteration << '\n';
*/
          mu_lm = mu_lm / 10.0;
          break;
        } else {
          //do not accept the new weight answer
          //adjust toward gradient descent more
          mu_lm = mu_lm * 10.0;
        }
      }
      //calculate step size (size of dw)
      if (usr::abs(dw) < eps_stop) {
        break;
      }
    }
    return w;
  }
  double SumSquare(std::vector<double> x) {
    std::size_t n = x.size();
    double ans = 0;
    for (std::size_t i = 0; i < n; i++) {
      ans = ans + x[i] * x[i];
    }
    return ans;
  }
  TwoDVector<double> TransposeNbyMMatrix(const TwoDVector<double>& a) {
    std::size_t row_input = a.size();
    std::size_t col_input = a[0].size();
    TwoDVector<double> ans(col_input, std::vector<double>(row_input));
    for (std::size_t i = 0; i < col_input; i++) {
      for (std::size_t j = 0; j < row_input; j++) {
        ans[i][j] = a[j][i];
      }
    }
    return ans;
  }
  //LM communicate with std::function + std::bind (in a function call)
  //This is used in spdepo
  std::vector<double> LevenbergMarquardt(
      std::function<std::vector<double>(
        const std::vector<double>& adj_params)> f,
      const std::vector<double>& ini_vec,
      const usr::TwoDVector<double>& data,
      bool& success,
      std::function<void(const std::vector<double>& input)>
        CallBack_fn,
      double eps_for_derivative,
      double eps_stop_input,
      double mu_lm_initial,
      int max_iter,
      int mu_loop_iter,
      std::wstring output_path_with_back_slash) {
    auto lm_start_time = std::chrono::high_resolution_clock::now();
    double mu_lm = mu_lm_initial;
    int number_of_accepted_iteration = 0;
    double eps_relative = eps_for_derivative;
    double eps_derivative;
    double eps_stop = eps_stop_input;
    double h_sse_old;
    //half sum-square-error old
    double h_sse;
    //half sum-square-error
    std::vector<double> w = ini_vec;
    //w = answer is the adjustable parameter vector
    std::vector<double> wPEps = ini_vec;
    std::vector<double> w_test = ini_vec;
    std::vector<double> dw = ini_vec;
    //w plus epsilon (only on the Jacobian calculation part)
    std::size_t n_params = ini_vec.size();
    std::size_t row_data = data.size();
    std::size_t n_y_data = data[0].size() - 1;
    //number of y column
    std::size_t m_data = row_data * n_y_data;
    //total number of data combined into 1 column
    std::vector<double> grad(n_params);
    std::wofstream LM_log;
    LM_log.open(output_path_with_back_slash + L"LM_log.csv");
    LM_log.close();
    usr::TwoDVector<double> jtj(
      n_params, std::vector<double>(n_params, 0.0));
    usr::TwoDVector<double> jacobian(
      m_data, std::vector<double>(n_params, 0.0));
    //Jacobian is defined as
    //j = [de1/dw1, de1/dw2, ..., de1/dwn]
    //    [de2/dw1, de2/dw2, ..., de2/dwn]
    //    [    ...,     ..., ...,    ...,]
    //    [dem/dw1, dem/dw2, ..., dem/dwn]
    //  d here is a partial derivative
    usr::TwoDVector<double> jacobian_transposed(
      n_params, std::vector<double>(m_data, 0.0));
    std::vector<double> e_mat(m_data);
    std::vector<double> e_mat0(m_data);
    TwoDVector<double> solve_jtj_plus_mu_i;
    //nx(n+1) matrix to solve for Dw in
    //(jtj+mu*i) * Dw = -g
    //The most right column is -g
    usr::TwoDVector<double> identity_matrix(
      n_params, std::vector<double>(n_params, 0.0));
    for (int i = 0; i < n_params; i++) {
      identity_matrix[i][i] = 1.0;
    }

    e_mat0 = f(w);
    h_sse = SumSquare(e_mat0) * 0.5;
    h_sse_old = h_sse;

    CallBack_fn(w);
    //create default log file
    LM_log.open(output_path_with_back_slash + L"LM_log.csv", std::ofstream::app);
    for (int i = 0; i < n_params; i++) {
      LM_log << "w[" << i << "] = ," << w[i] << '\n';
    }
    LM_log << "hsse = ," << h_sse << '\n';
    LM_log << "mu_lm = ," << mu_lm << '\n';
    auto lm_point_time = std::chrono::high_resolution_clock::now();
    LM_log << "elapsed time [milliseconds] = ," <<
      std::chrono::duration_cast<std::chrono::milliseconds>(
        lm_point_time - lm_start_time).count() << '\n';
    LM_log.close();
    std::cout << "half-sum-squared = " << h_sse << '\n';
    std::cout << "mu_lm = " << mu_lm << '\n';



    //outer loop for adjusting ans
    for (int i = 1; i <= max_iter; i++) {
      //Jacobian calculation, start

      for (int j = 0; j < n_params; j++) {
        //initially, wPEps is already equal to w
        if (wPEps[j] != 0.0) {
          eps_derivative = wPEps[j] * eps_relative;
          wPEps[j] = wPEps[j] * (1.0 + eps_relative);
        }
        else {
          eps_derivative = eps_relative;
          wPEps[j] = wPEps[j] + eps_relative;
        }

        e_mat = f(wPEps);
        jacobian_transposed[j] = (e_mat - e_mat0) / eps_derivative;
        wPEps[j] = w[j];
      }
      jacobian = TransposeNbyMMatrix(jacobian_transposed);
      //Jacobian calculation, end
      //Grad e: g = J^T*e calculation
      MatrixMultiply2D(jacobian_transposed, e_mat0, grad);
      //Calculate JTJ (JTJ changes as the direction changes)
      MatrixMultiply2D(jacobian_transposed, jacobian, jtj);
      for (int j = 1; j <= mu_loop_iter; j++) { //Mu loop
                                      //if error decreases, decreases Mu and exit loop
                                      //High mu means it becomes gradient descent more

                                      //Prepare matrix for solving
                                      //  H * Dw = -g
                                      //  w_{k+1} = w_k - (H^-1 * g)_k
                                      //  Dw = w_{k+1} - w_k
                                      //  H ~ JTJ + mu * I
        //Marquardt option
        usr::TwoDVector<double> diag_jtj = jtj;
        for (int i = 0; i < jtj.size(); i++) {
          for (int j = 0; j < jtj[0].size(); j++) {
            if (i != j) {
              diag_jtj[i][j] = 0.0;
            }
          }
        }
        solve_jtj_plus_mu_i = jtj + mu_lm * diag_jtj;

        for (int i = 0; i < n_params; i++) {
          //append grad to the right of jtj + mu * i
          solve_jtj_plus_mu_i[i].insert(
            solve_jtj_plus_mu_i[i].end(), 1, -1.0 * grad[i]);
        }
        dw = GaussianEliminationWithBackSubstitution(&solve_jtj_plus_mu_i);
        w_test = w + dw;
        std::vector<double> e_mat0_test = e_mat0;
        e_mat0_test = f(w_test);

        h_sse = SumSquare(e_mat0_test) * 0.5;
        if (h_sse < h_sse_old) {
          //accept the new weight answer
          number_of_accepted_iteration += 1;
          w = w_test;
          wPEps = w;
          h_sse_old = h_sse;
          e_mat0 = e_mat0_test;
          CallBack_fn(w);
          //create default log file
          LM_log.open(output_path_with_back_slash + L"LM_log.csv", std::ofstream::app);
          for (int i = 0; i < n_params; i++) {
            LM_log << "w[" << i << "] = ," << w[i] << '\n';
          }
          LM_log << "hsse = ," << h_sse << '\n';
          LM_log << "mu_lm = ," << mu_lm << '\n';
          auto lm_point_time = std::chrono::high_resolution_clock::now();
          LM_log << "elapsed time [milliseconds] = ," <<
            std::chrono::duration_cast<std::chrono::milliseconds>(
              lm_point_time - lm_start_time).count() << '\n';
          LM_log.close();
          std::cout << "half-sum-squared = " << h_sse << '\n';
          std::cout << "mu_lm = " << mu_lm << '\n';
          mu_lm = mu_lm / 10.0;
          break;
        }
        else {
          //do not accept the new weight answer
          //adjust toward gradient descent more
          mu_lm = mu_lm * 10.0;
        }
      }
      //calculate step size (size of dw)
      if (usr::abs(dw) / usr::abs(w) < eps_stop) {
        break;
      }
    }
    return w;
  }

  void LMObject::SetFunction(
    std::function<std::vector<double>(std::vector<double>)> input) {
    f_ = input;
  }

  std::vector<double> LMObject::ErrorCalculation(
      const std::vector<double>& params) {
      int n = data_.size();
      std::vector<double> ans;
      std::vector<double> predict;
    
    predict = f_(params);
 
    if (!use_scale_vec_) {
      for (int i = 0; i < n; i++) {
        ans.push_back((predict[i] - data_[i][1]) / data_[i][1]);
      }
    }
    else {
      for (int i = 0; i < n; i++){
        ans.push_back((predict[i] - data_[i][1]) / scale_vec_[i]);
      }
    }
    return ans;
  }

  void LMObject::SetDataForFitting(usr::TwoDVector<double> data) {
    data_ = data;
  }

  usr::TwoDVector<double> LMObject::GetDataForFitting() {
    return data_;
  }

  void LMObject::SetScalingVector(std::vector<double> scale_vec) {
    scale_vec_ = scale_vec;
    use_scale_vec_ = true;
  }

  std::vector<double> LMObject::RunLM(const std::wstring& output_path_with_back_slash) {
    std::vector<double> ans;
    auto f = std::bind(&LMObject::ErrorCalculation,
      this, std::placeholders::_1);
    bool success;
    start_time_ = std::chrono::high_resolution_clock::now();
    ans = usr::LevenbergMarquardt(
      f,
      initial_guess_,
      data_,
      success,
      call_back_,
      eps_,
      eps_stop_,
      mu_lm_initial_,
      max_lm_iteration_,
      max_mu_loop_iteration_,
      output_path_with_back_slash);
    lm_ans_ = ans;
    return ans;
  }

  void LMObject::PrintStandardOutput(
      const std::vector<std::string>& name_vec,
      const std::vector<int>& index_vec) const {
    std::ofstream ofs;
    ofs.open(output_path_ + L"LM_result.csv");
    for (int i = 0; i < index_vec.size(); i++) {
      ofs << index_vec[i] << "," << name_vec[i] << "," << lm_ans_[i];
      if (i < index_vec.size() - 1) {
        ofs << "\n";
      }
    }
    ofs.close();
  }
  void LMObject::SetOutputPath(const std::wstring& output_path) {
    output_path_ = output_path;
  }
  std::wstring LMObject::GetOUtputPath() {
    return output_path_;
  }
  void LMObject::EmptyCallBack(const std::vector<double>& input) {
    //Empty. Do not put any code in.
  }

  void LMObject::SetCallBack(
    std::function<void(const std::vector<double>& input)> f) {
    call_back_ = f;
  }

  LMObject::LMObject() {
    auto empty_call_back = std::bind(
      &LMObject::EmptyCallBack, this, std::placeholders::_1);
    SetCallBack(empty_call_back);
    SetEpsilonForStop(1.0e-5);
    SetMuLMInitial(1.0);
  }

  void LMObject::SetDefaultEchoCallBack() {
    auto bound_echo_call = std::bind(
      &LMObject::EchoCallBack, this, std::placeholders::_1);
    SetCallBack(bound_echo_call);
  }

  void LMObject::SetEmptyCallBack() {
    auto empty_call_back = std::bind(
      &LMObject::EmptyCallBack, this, std::placeholders::_1);
    SetCallBack(empty_call_back);
  }

  void LMObject::EchoCallBack(const std::vector<double>& input) {
    auto point_time = std::chrono::high_resolution_clock::now();
    std::cout << "i = " << number_of_calls << '\n';
    std::cout << "elapsed time [milliseconds] = " <<
      std::chrono::duration_cast<std::chrono::milliseconds>(
        point_time - start_time_).count() << '\n';
    number_of_calls += 1;
    for (int i = 0; i < input.size(); i++) {
      std::cout << "w[" << i << "] = " << input[i] << '\n';
    }
  }

  void LMObject::SetEpsilonForDerivative(double eps) {
    eps_ = eps;
  }

  double LMObject::GetEpsilonForDerivative() {
    return eps_;
  }

  void LMObject::SetEpsilonForStop(double eps_stop) {
    eps_stop_ = eps_stop;
  }

  double LMObject::GetEpsilonForStop() {
    return eps_stop_;
  }

  void LMObject::SetMuLMInitial(double mu_lm_initial) {
    mu_lm_initial_ = mu_lm_initial;
  }

  double LMObject::GetMuLMInitial() {
    return mu_lm_initial_;
  }
  void LMObject::SetMaxIteration(int max_lm_iteration){
    max_lm_iteration_ = max_lm_iteration;
  }
  int LMObject::GetMaxIteration() const{
    return max_lm_iteration_;
  }
  void LMObject::SetMaxMuLoopIteration(int max_mu_loop_iteration){
    max_mu_loop_iteration_ = max_mu_loop_iteration;
  }
  int LMObject::GetMaxMuLoopIteration() const{
    return max_mu_loop_iteration_;
  }
  

  std::vector<double> Interpolation(
    const std::vector<double>& x,
    const std::vector<double>& x_data,
    const std::vector<double>& y_data) {
    //  x - x0     x1 - x0
    //  ------  =  -------
    //  y - y0     y1 - y0
    //  y = y0 + (x - x0) * (y1 - y0) / ( x1 - x0)
    int n_ans = x.size();
    int n_data = x_data.size();
    double x0, x1, y0, y1;
    double y; //correspond to x and y, respectively
    std::vector<double> ans;
    int left_index;
    for (int i = 0; i < n_ans; i++) {
      left_index = FindFirstLeftIndex(x[i], x_data);
      if (left_index == -1) { //need extrapolation
        ans.push_back(
          y_data[0] + (x[i] - x_data[0]) * (y_data[1] - y_data[0])
            / (x_data[1] - x_data[0]));
        continue;
      } else if (left_index == n_data - 1) { //need extrapolation
        ans.push_back(
          y_data[n_data - 2] + (x[i] - x_data[n_data - 2]) 
          * (y_data[n_data - 1] - y_data[n_data - 2])
          / (x_data[n_data - 1] - x_data[n_data - 2]));
        continue;
      }
      ans.push_back(
        y_data[left_index] + (x[i] - x_data[left_index]) 
        * (y_data[left_index + 1] - y_data[left_index])
        / (x_data[left_index + 1] - x_data[left_index]));
    }
    return ans;
  }

  int FindFirstLeftIndex(double value, 
    std::vector<double> input_vec) {
    int n_data = input_vec.size();
    int ans_index = 0;
    if (value < input_vec[0]) {
      return -1;
    } else if (value > input_vec[n_data-1]) {
      return n_data - 1;
    }
    for (int i = 0; i < n_data - 1; i++) {
      if (value < input_vec[i + 1]) {
        if (value >= input_vec[i]) {
          ans_index = i;
          return ans_index;
        }
      }
    }
    return n_data - 1;
  }

  double MaxSecondColumn(const std::vector<std::vector<double>>& a) {
  //a has to be 2column vector {{t,x1}, {t,x2}, ..., {t,xn}}
    int n = a.size();
    double max_val = a[0][1];
    if (n > 1) {
      for (int i = 1; i < n; i++) {
        if (max_val < a[i][1]) {
          max_val = a[i][1];
        }
      }
    }
    return max_val;
  }

  double AverageSecondColumn(const std::vector<std::vector<double>>& a) {
    //a has to be 2column vector {{t,x1}, {t,x2}, ..., {t,xn}}
    int n = a.size();
    double average = a[0][1];
    if (n > 1) {
      for (int i = 1; i < n; i++) {
        average = average + a[i][1];
      }
      average = average / (double)n;
    }
    return average;
  }


} // namespace usr

std::ostream& operator<< (std::ostream& os,
  const usr::TwoDVector<double>& vec) {
  for (auto row : vec) {
    for (auto item : row) {
      os << item << "  ";
    }
    os << '\n';
  }
  return os;
}

} // namespace spdepo