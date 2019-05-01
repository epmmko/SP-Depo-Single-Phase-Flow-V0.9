#ifndef CHECK_LM_H_
#define CHECK_LM_H_
#include <vector>
#include <chrono>
#include "user_short_name.h"
namespace spdepo{
class CheckLM{
 public:
  void Run();
};
class CheckLM2 {
 public:
  void Run();
};

class FunctionForFitting {
 public:
  double f(double x, double c1, double c2);
  std::vector<double> ErrorCalculation(
    const std::vector<double>& params);
  std::vector<std::vector<double>> data_;
};

std::vector<double> WrapFunctionForFitting(const std::vector<double>& adj_params,
    const std::vector<std::vector<double>>& data);

class BindingTest {
public:
  double f(double);
  double y_ = 10.0;
  usr::TwoDVector<double> data_;
};

class LMObject_test {
 public:
  LMObject_test();
  std::vector<double> ErrorCalculation(
    const std::vector<double>& params);
  std::vector<double> RunLM();
  void SetFunction(std::function<double(double, double, double)>);
  void SetCallBack(std::function<void(const std::vector<double>& input)> f);
  std::vector<double> initial_guess_;
  void SetDataForFitting(usr::TwoDVector<double> data);
  usr::TwoDVector<double> GetDataForFitting();
  void SetScalingVector(std::vector<double> scale_vec);
  void SetDefaultEchoCallBack();
  void SetEmptyCallBack();
  void SetEpsilonForDerivative(double eps);
  double GetEpsilonForDerivative();
  void SetEpsilonForStop(double eps_stop);
  double GetEpsilonForStop();
  void SetMuLMInitial(double mu_lm_initial);
  double GetMuLMInitial();
 private:
  std::vector<std::vector<double>> data_;
  std::function<double(double, double, double)> f_;
  std::function<void(const std::vector<double>& input)> call_back_;
  void EmptyCallBack(const std::vector<double>& input);
  void EchoCallBack(const std::vector<double>& input);
  int number_of_calls = 0;
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
  double eps_;
  double eps_stop_;
  double mu_lm_initial_;
  std::vector<double> scale_vec_;
  bool use_scale_vec_ = false;
};

class BlackBoxObjectForLM_test {
 public:
  double f(double x, double c1, double c2);
  double x0_;
};

} //namespace spdepo
#endif