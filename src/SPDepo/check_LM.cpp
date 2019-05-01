#include "check_LM.h"
#include "user_numerical_method.h"
#include "user_short_name.h"
#include <iostream>
#include <vector>
#include <functional>
#include <typeinfo>
#include <chrono>
namespace spdepo{
FunctionForFitting *ptr_function_for_fitting;

double testfn(std::function<double(double)> fn_input, double val) {
  return fn_input(val) + val;
}

double PolynomialFunction(double x, std::vector<double> c) {
  int n = c.size();
  double sum = 0.0;
  for (int i = 0; i < n; i++) {
    sum = sum + c[i] * std::pow(x,n - i - 1);
  }
  return sum;
}

usr::TwoDVector<double> Simulation(std::vector<double> w, double min_x = 0.0, double max_x = 20.0) {
  int n_interval = 20;
  int n_points = n_interval + 1;
  double delta = (max_x - min_x) / n_interval;
  double x;
  usr::TwoDVector<double> xy;
  for (int i = 0; i < n_points; i++) {
    x = i * delta;
    xy.push_back({x,PolynomialFunction(x, w)});
  }
  return xy;
}

std::vector<double> PredictionAtDataPoint(std::vector<double> w, 
    usr::TwoDVector<double> data,
    double min_x = 0.0,
    double max_x = 20.0) {
  usr::TwoDVector<double> xy_predict = Simulation(w, min_x, max_x);
  int n = data.size();
  std::vector<double> x_data;
  std::vector<double> y_data;
  std::vector<double> x_predict;
  std::vector<double> y_predict;
  std::vector<double> y_interpolated;
  for (int i = 0; i < n; i++) {
    x_data.push_back(data[i][0]);
    y_data.push_back(data[i][1]);
  }
  for (int i = 0; i < xy_predict.size(); i++) {
    x_predict.push_back(xy_predict[i][0]);
    y_predict.push_back(xy_predict[i][1]);
  }
  y_interpolated = usr::Interpolation(x_data, x_predict, y_predict);
  return y_interpolated;
}

std::vector<double> ErrorCalculation(std::vector<double> w) {
  return w;
}

void CheckLM2::Run() {
  std::vector<std::vector<double>> data = { { 1, 9 },
                                            { 2,11 },
                                            { 3,19 },
                                            { 4,21 },
                                            { 5,29 },
                                            { 6,31 } };
  usr::LMObject lm_polynomial;
  lm_polynomial.SetDataForFitting(data);
  auto bound_f_predict = std::bind(PredictionAtDataPoint,
    std::placeholders::_1,
    data,
    0.0,
    20.0);
  lm_polynomial.SetFunction(bound_f_predict);
  lm_polynomial.initial_guess_ =
    {1, 2};
  lm_polynomial.SetDefaultEchoCallBack();
  std::vector<double> w_ans;
  lm_polynomial.SetEpsilonForDerivative(0.0001);
  lm_polynomial.SetEpsilonForStop(1.0e-6);
  lm_polynomial.SetMuLMInitial(1);
  w_ans = lm_polynomial.RunLM(L"");
  for (int i = 0; i < w_ans.size(); i++) {
    std::cout << "w[" << i << "] = " << w_ans[i] << '\n';
  }
}

void CheckLM::Run() {
  std::vector<std::vector<double>> A(4, std::vector<double>{});
  std::vector<double> B = {1,2,3,4};
  A[0] = {1,2,3,4};
  A[1] = {2,2,3,3};
  A[2] = {4,2,-1,-5};
  A[3] = {-2, 10, -3, 6};
  std::vector<std::vector<double>> AA = A;
  for (std::size_t i = 0; i < 4; i++) {
    AA[i].insert(AA[i].end(),1,B[i]);
  }
  std::vector<double> x;
  x = usr::GaussianEliminationWithBackSubstitution(&AA);
  for (std::size_t i = 0; i < x.size(); i++) {
    std::cout << x[i] << '\n';
  }

  FunctionForFitting fn1;
  fn1.data_ = {{1, 9},
               {2,11},
               {3,19},
               {4,21},
               {5,29},
               {6,31}};
  ptr_function_for_fitting = &fn1;
    //come from the corresponding header file
  std::vector<double> ans_LM;
  bool success;

  auto start_time = std::chrono::high_resolution_clock::now();
  ans_LM = usr::LevenbergMarquardt(
    WrapFunctionForFitting,
    {10,10},
    fn1.data_,
    success);
  std::cout << "\n\n";
  auto end_time = std::chrono::high_resolution_clock::now();
  std::cout << "time for raw function call [microsecond] = ";
  std::cout << std::chrono::duration_cast<std::chrono::microseconds>(
    end_time - start_time).count() << '\n';


  for (int i = 0; i < ans_LM.size(); i++) {
    std::cout << ans_LM[i] << '\n';
  }

  BindingTest LM1;
  std::cout << "hello from check_LM\n";
  std::cout << LM1.f(3) << '\n';
  std::cout << LM1.f(2) << '\n';
  LM1.y_ = 10;
  auto func1 = std::bind(&BindingTest::f,
    &LM1, std::placeholders::_1);
  std::cout << "bind with the reference " << "LM1.y_ = 10 before bind, func1(10.0) = "
    << func1(10.0) << '\n';
  LM1.y_ = 13;
  std::cout << "bind with the reference LM1.y_ = 10 before bind," 
    << "\n but adjust LM1.y_ = 13 later, func1(10.0) = "
    << func1(10.0) << '\n';
  LM1.y_ = 10;
  auto func2 = std::bind(&BindingTest::f,
    LM1, std::placeholders::_1);
  std::cout << "bind with value " << "LM1.y_ = 10 before bind, func1(10.0) = "
    << func2(10.0) << '\n';
  LM1.y_ = 13;
  std::cout << "bind with value " 
    << "LM1.y_ = 10 but adjust LM1.y_ = 13 later, func1(10.0) = "
    << func2(10.0) << '\n';
  LM1.y_ = 10;
  auto func3 = std::bind(&BindingTest::f,
    LM1, std::placeholders::_1);
  std::cout << "\n\n\n" << testfn(func3,10) << "\n\n";
  std::cout << "end binding test\n\n\n";

  FunctionForFitting fn2;
  fn2.data_ = {{ 1, 9 },
               { 2,11 },
               { 3,19 },
               { 4,21 },
               { 5,29 },
               { 6,31 }}; // not the same, use 31
  auto fn_LM_bind = std::bind(
    &FunctionForFitting::ErrorCalculation,
    &fn2,
    std::placeholders::_1);
  std::vector<double> ans_LM2;
  auto start_time2 = std::chrono::high_resolution_clock::now();
  ans_LM2 = usr::LevenbergMarquardt(
    fn_LM_bind,
    { 10,10 },
    fn2.data_,
    success);
  auto end_time2 = std::chrono::high_resolution_clock::now();
  std::cout << "time for binding call [microsecond] = ";
  std::cout << std::chrono::duration_cast<std::chrono::microseconds>(
    end_time2 - start_time2).count() << '\n';
  std::cout << "\n\n";
  std::cout << "use binding to call\n";
  for (int i = 0; i < ans_LM2.size(); i++) {
    std::cout << ans_LM2[i] << '\n';
  }

  //speed test conclusion
  //run in release mode get
  //
  // use wrapper raw function use 123 microsecond
  // use std::function + bind use  84 microsecond
  // they are similar, function+bind is a little faster
  // most of the time function+bind is faster, (a few time, raw wrapper is faster)

  //remake LM
  std::cout << "remake LM\n\n";
  LMObject_test LM_obj;
  BlackBoxObjectForLM_test bbo_lm;
  bbo_lm.x0_ = 0.0;
  auto bound_bbo_lm = std::bind(
    &BlackBoxObjectForLM_test::f, &bbo_lm,
    std::placeholders::_1,
    std::placeholders::_2,
    std::placeholders::_3);
  LM_obj.SetFunction(bound_bbo_lm);
  std::cout.precision(16);

  LM_obj.SetDataForFitting({{ 1, 9 },
                            { 2,11 },
                            { 3,19 },
                            { 4,21 },
                            { 5,29 },
                            { 6,31 } });
  LM_obj.initial_guess_ = {10.0,10.0};
  std::vector<double> ans_LM_obj;
  LM_obj.SetDefaultEchoCallBack();
  ans_LM_obj = LM_obj.RunLM();
  for (int i = 0; i < ans_LM_obj.size(); i++) {
    std::cout << ans_LM_obj[i] << '\n';
  }
  std::cout << typeid(end_time2).name() << '\n';
  std::cout << "done remake LM\n\n";


  std::cout << "Check interpolation function\n";
  std::vector<double> x_data = {1,2,3,4,5,6,7};
  std::vector<double> y_data = {12,22,32,42,52,62,72};
  std::vector<double> x_inter_input = {10.7,3.5,4.7,5.2,6.43};
  std::vector<double> interpolation_ans = usr::Interpolation(x_inter_input,x_data,y_data);
  for (int i = 0; i < interpolation_ans.size(); i++) {
    std::cout << "at x = " << x_inter_input[i] << "   "
      << "y = " << interpolation_ans[i] << '\n';
  }
}

double FunctionForFitting::f(double x, double c1, double c2) {
  return c1*x + c2;
}

std::vector<double> FunctionForFitting::ErrorCalculation(
    const std::vector<double>& params) {
  int n = data_.size();
  std::vector<double> ans;
  double predict;
  for (int i = 0; i < n; i++) {
    predict = f(data_[i][0], params[0], params[1]);
    ans.push_back(predict - data_[i][1]);
  }
  return ans;
}

std::vector<double> WrapFunctionForFitting(
    const std::vector<double>& adj_params,
    const std::vector<std::vector<double>>& data) {
  ptr_function_for_fitting->data_ = data;
  return ptr_function_for_fitting->ErrorCalculation(adj_params);
}
double BindingTest::f(double x) {
  return x + y_;
}

void LMObject_test::SetFunction(
    std::function<double(double, double, double)> input) {
  f_ = input;
}

std::vector<double> LMObject_test::ErrorCalculation(
    const std::vector<double>& params) {
  int n = data_.size();
  std::vector<double> ans;
  double predict;
  for (int i = 0; i < n; i++) {
    predict = f_(data_[i][0], params[0], params[1]);
    if (!use_scale_vec_) {
      ans.push_back(predict - data_[i][1]);
    } else {
      ans.push_back((predict - data_[i][1])/scale_vec_[i]);
    }
    
  }
  return ans;
}

void LMObject_test::SetDataForFitting(usr::TwoDVector<double> data) {
  data_ = data;
}

usr::TwoDVector<double> LMObject_test::GetDataForFitting() {
  return data_;
}

void LMObject_test::SetScalingVector(std::vector<double> scale_vec) {
  scale_vec_ = scale_vec;
  use_scale_vec_ = true;
}

std::vector<double> LMObject_test::RunLM() {
  std::vector<double> ans;
  auto f = std::bind(&LMObject_test::ErrorCalculation,
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
    mu_lm_initial_);
  return ans;
}

void LMObject_test::EmptyCallBack(const std::vector<double>& input) {
  //Empty. Do not put any code in.
}

void LMObject_test::SetCallBack(
    std::function<void(const std::vector<double>& input)> f) {
  call_back_ = f;
}

LMObject_test::LMObject_test() {
  auto empty_call_back = std::bind(
    &LMObject_test::EmptyCallBack, this, std::placeholders::_1);
  SetCallBack(empty_call_back);
  SetEpsilonForDerivative(1.0e-4);
  SetEpsilonForStop(1.0e-5);
  SetMuLMInitial(1.0);
}

void LMObject_test::SetDefaultEchoCallBack() {
  auto bound_echo_call = std::bind(
    &LMObject_test::EchoCallBack, this, std::placeholders::_1);
  SetCallBack(bound_echo_call);
}

void LMObject_test::SetEmptyCallBack() {
  auto empty_call_back = std::bind(
    &LMObject_test::EmptyCallBack, this, std::placeholders::_1);
  SetCallBack(empty_call_back);
}

void LMObject_test::EchoCallBack(const std::vector<double>& input) {
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

void LMObject_test::SetEpsilonForDerivative(double eps) {
  eps_ = eps;
}

double LMObject_test::GetEpsilonForDerivative() {
  return eps_;
}

void LMObject_test::SetEpsilonForStop(double eps_stop) {
  eps_stop_ = eps_stop;
}

double LMObject_test::GetEpsilonForStop() {
  return eps_stop_;
}

void LMObject_test::SetMuLMInitial(double mu_lm_initial) {
  mu_lm_initial_ = mu_lm_initial;
}

double LMObject_test::GetMuLMInitial() {
  return mu_lm_initial_;
}
double BlackBoxObjectForLM_test::f(double x, double c1, double c2){
  return c1*x + c2 + x0_;
}
} //namespace spdepo