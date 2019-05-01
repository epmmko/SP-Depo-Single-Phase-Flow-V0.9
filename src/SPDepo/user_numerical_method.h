#ifndef USER_NUMERICAL_METHOD_H_
#define USER_NUMERICAL_METHOD_H_
#include <math.h>
#include <vector>
#include <map>
#include "user_short_name.h"
//https://stackoverflow.com/questions/1380371/
//what-are-the-most-widely-used-c
//-vector-matrix-math-linear-algebra-libraries-a
namespace spdepo{
namespace usr {
  void EmptyCallBack(const std::vector<double>& input);
//  std::function<void(const std::vector<double>& input)> 
//    EmptyCallBack_fn = EmptyCallBack;

  double BisectionMethod(double(*f)(double), double left, double right,
                         bool& success, int& number_of_iteration,
                         double& fx_final, double eps = 1.0e-7);
  double NewtonMethod(double(*f)(double), double x, bool& success,
                      int& number_of_iteration, double& fx_final,
                      int quit_if_not_improve = 4,
                      double eps = 1.0e-7);
    //x is the initial guess
    //This function return the root x that make f(x) = 0;
  void InitialGuessScan(double(*f)(double), double& left_value,
                        double& right_value, int n_interval = 100);
  double Simpson13(std::vector<double> x, std::vector<double> y);
    //return numerical integration from x_start to x_end
    //(x,y) pair is the dot on a graph.
    //Simpson 1/3 works only if size(x) is the odd number
    //  (even number of section). It needs a uniform spacing in x-vector
  double Trapezoidal(std::vector<double> x, std::vector<double> y);
    //return numerical integration from x_start to x_end
    //(x,y) pair is the dot on a graph

  double TryNewtonThenBisection(double(*f)(double), double x, 
      bool& success, int& number_of_iteration, double& fx_final,
      int n_scan = 100, int quit_if_not_improve = 4, double eps = 1.0e-7);
  void ThomasTridiagonal(const std::vector<double>& a_vec,
                         const std::vector<double>& b_vec,
                         const std::vector<double>& c_vec,
                         const std::vector<double>& d_vec,
                         std::vector<double>& x_vec,
                         int n);
    /*Thomas algorithm solve tridiagonal matrix
    The inputs of a, b, and c are defined as
    [b1 c1  0  0 ...    0] [x1]   [d1]
    [a2 b2 c2  0 ...    0] [x2]   [d2]
    [ 0 a3 b3 c3 ...    0] [x3]   [d3]
    [ 0  0  .  . ...    0] [. ] = [ .]
    [ .  .  .  . ...    0] [. ]   [ .]
    [ 0  0  0  . ... cn-1] [. ]   [ .]
    [ 0  0  0  0  an   bn] [xn]   [dn]
    Requirement
    double a[n] = {0, a2, a3, a4, ..., an}
    double b[n] = {b1,b2, b3, b4, ..., bn}
    double c[n] = {c1,c2, c3, ..., cn-1, 0}
    double d[n] = {d1,d2, d3, d4, ..., dn}
    n is the size or the number of row of the square matrix or x

    Ax =b, Known Result for code validation
    A                     b    x
    8 -4  0  0  0  0  0	  2    1.427641369
   -1  9 -5  0  0  0  0	  3    2.355282738
    0 -2 10 -6  0  0  0	  4    3.353980655
    0	 0 -3 11 -7  0  0	  5    4.138206845
    0  0  0 -4 12 -8  0	  6    4.351190476
    0  0  0  0 -5 13 -9	  7    3.707682292
    0  0  0  0  0 -6 14   8    2.160435268

    a = { 0, -1, -2, -3, -4, -5, -6}
    b = { 8,  9, 10, 11, 12, 13, 14}
    c = {-4, -5, -6, -7, -8, -9,  0}
    d = { 2,  3,  4,  5,  6,  7,  8}

    std::vector<double> x(7, 0.0);
    usr::ThomasTridiagonal(a, b, c, d, x, 7);
    */

  //get dense matrix from tridiagonal matrix
  void TridiagonalToDense(const std::vector<double>& a,
                          const std::vector<double>& b,
                          const std::vector<double>& c,
                          std::vector<std::vector<double>>* dense);
  //solve c from c = a*b
  void MatrixMultiply2D(const TwoDVector<double>& a,
                        const TwoDVector<double>& b,
                        TwoDVector<double>& c);
  void MatrixMultiply2D(const TwoDVector<double>& a,
                        const std::vector<double>& b,
                        std::vector<double>& c);
  //Transpose N x M matrix to get M x N matrix
  TwoDVector<double> TransposeNbyMMatrix(const TwoDVector<double>& a);
  //solve for y from dy/dx = f(x,y)
  void DormandPrince1D(double(*fn)(double, double), long* num_fn_call,
                       std::vector<double>* x,
                       std::vector<double>* y,
                       double ic = 1, double a = 0, double b = 1,
                       double h = 0.3, double eps = 1.0e-6,
                       int max_num_interval = 50,
                       int max_iteration = 100,
                       double step_ratio_max = 4,
                       double abs_hmax = 0.3,
                       double abs_hmin = 1.0e-6,
                       bool exact_final_point = true,
                       bool suppress_warning = false);
    /*This Dormand Prince 54 solve 1 equation ODE in the form of

    dy/dx = fn(x,y)
  
    fn is a right-hand-side function of dy/dx
    y is the output (require the input of empty vector)
    x is the output (require the input of empty vector)
    ic = initial condition or y value at x = a
    a is the starting point for solving dy/dx
    h is the step size initial guess.
      DPRI54 will adjust this automatically
    max_num_point is the maximum number of point of output
    max_iterature is the maximum number of iteration for
      a step size search.
    */
  void DormandPrinceMany(const std::vector<double(*)(double, const std::vector<double>&)>& fn,
    long* num_fn_call,
    std::vector<double>* x,
    TwoDVector<double>* y,
    std::vector<double> ic, double a, double b,
    double h = 0.1,
    double eps = 1.0e-6,
    int max_num_interval = 50,
    int max_iteration = 100,
    double step_ratio_max = 4,
    double abs_hmax = 0.3,
    double abs_hmin = 1.0e-6,
    bool exact_final_point = true,
    bool suppress_warning = false);

  //overloaded version, take function that return vector<double>
  //used in SP-Depo
  void DormandPrinceMany(
    std::vector<double>(*fn)(double , const std::vector<double>&),
    long number_of_ode,
    long* num_fn_call,
    std::vector<double>* x,
    TwoDVector<double>* y,
    std::vector<double> ic,
    double a,
    double b,
    double h,
    std::vector<double> eps,
    int max_num_interval = 50,
    int max_iteration = 100,
    double step_ratio_max = 4,
    double abs_hmax = 0.3,
    double abs_hmin = 1.0e-6,
    bool exact_final_point = true,
    bool suppress_warning = false, 
    bool show_time_in_process = true);

  usr::TwoDVector<double> MeshToSec(
      const usr::TwoDVector<double>& input_mesh);
  
  std::vector<double> LevenbergMarquardt(
      std::vector<double>(*f)(const std::vector<double>& adj_params,
      const usr::TwoDVector<double>& data),
      const std::vector<double>& ini_vec,
      const usr::TwoDVector<double>& data,
      bool& success,
      double eps_for_derivative = 1.0e-4,
      double eps_stop = 1.0e-5,
      double mu_lm_initial = 1.0);
    //f is the pointer to the wrapper function that
    //  takes vector of the fitting parameters (adjustable parameters)
    //  as the input and take the vector of data
    //
    //data is 2-D vector (matrix)
    //  the first column is x parameter
    //  the rest is the y vector
    //  it has m row, where m is the number of data points
    //
    //return vector of function f is the error column vector
    //  e[    0] to e[      N-1] is the rms error of y0 vs y0_exp
    //  e[    N] to e[     2N-1] is the rms error of y1 vs y1_exp
    //  e[0+i*N] to e[(i+1)*N-1] is the rms of yi vs yi_exp
    //  e.size() is (data[0].size() - 1) * data.size()
    //  data[0].size() - 1 is the number of y-column
    //  data.size() is the number of row
    //  size of vector e is the total number of data point * #y-column
    //
    //ini_vec is the vector of the initial value of the weights
    //  (fitting parameters)
    //
    //return vector of this function (LM) is
    //  the vector of the adjustable parameters that are already
    //  tuned for the given experimental data
  //use in sp-depo
  std::vector<double> LevenbergMarquardt(
    std::function<std::vector<double>(const std::vector<double>& adj_params)> f,
    const std::vector<double>& ini_vec,
    const usr::TwoDVector<double>& data,
    bool& success,
    std::function<void(const std::vector<double>& input)> 
      EmptyCallBack_fn = EmptyCallBack,
    double eps_for_derivative = 1.0e-4,
    double eps_stop = 1.0e-5,
    double mu_lm_initial = 1.0,
    int max_iter = 50,
    int mu_loop_iter = 5,
    std::wstring output_path_with_back_slash = L"");

  class LMObject {
  //This class all the user to indirectly call
  //Levenberg-Marquardt
  //The user need to at least
  //1) Set the data
  //  e.g.  SetDataForFitting({{1,10}, {2,21}, {3, 29}});
  //2) Set the function to be fitted with
  public:
    LMObject();
    std::vector<double> ErrorCalculation(
      const std::vector<double>& params);
    std::vector<double> RunLM(const std::wstring& output_path_with_back_slash);
    //Set the predictive function. This function gives
    //  the prediction at the same location of x
    //  in the data set.
    //The return vector.size() must match to
    //  the data.size()  
    void SetFunction(
      std::function<std::vector<double>(std::vector<double>)>);
    void SetCallBack(
      std::function<void(const std::vector<double>& input)> f);
    std::vector<double> initial_guess_;
    //The data for fitting must combined into 2 column
    //The first column is the independent variable
    //The second column is the dependent variable
    // data = {{ t1, y0_1},
    //         { t2, y0_2},
    //          ...   ...
    //         { tN, y0_N},
    //         { t1, y1_1},
    //         { t2, y1_2},
    //          ...   ...
    //         { tN, y1_N},
    //          ...   ...
    //         { tN, yM_N}}
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
    void SetMaxIteration(int max_lm_iteration);
    int GetMaxIteration() const;
    void SetMaxMuLoopIteration(int max_mu_loop_iteration);
    int GetMaxMuLoopIteration() const;
    void PrintStandardOutput(const std::vector<std::string>& name_vec,
      const std::vector<int>& index_vec) const;
    void SetOutputPath(const std::wstring& output_path);
    std::wstring GetOUtputPath();
  private:
    std::wstring output_path_;
    std::vector<double> lm_ans_;
    std::vector<std::vector<double>> data_;
    //The predictive function.
    //The input vector is the input of the fitting parameters.
    std::function<std::vector<double>(std::vector<double>)> f_;
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
    int max_lm_iteration_ = 50;
    int max_mu_loop_iteration_ = 5;
  };


  std::vector<double> GaussianEliminationWithBackSubstitution(
      usr::TwoDVector<double>* AA);
    //solve A x = B
    //return x, size N vector
    //AA is vector<vector<double>> size N x (N+1)
    //
    //AA = {{A11, A12, A13, ..., A1N, B1},
    //      {A21, A22, A23, ..., A2N, B2},
    //      ...
    //      {AN1, AN2, AN3, ..., ANN, BN}}
    //For example, AA[0][2] = A13
    //B vector must be embedded in A 
    //  as the most right column for each row
    //Verified with
    // A = {{ 1, 2, 3, 4},
    //      { 2, 2, 3, 3},
    //      { 4, 2,-1,-5},
    //      {-2,10,-3, 6}}
    //
    // B = {1,2,3,4}
    // return x = {2.75, -0.8125, -2.375, 1.75}
  double SumSquare(std::vector<double> x);
    //return x[0]*x[0] + ... + x[n] * x[n]
  //x_data and y_data are the given data
  //x_data must be in the ascending order (low to high)
  //x is the vector that the corresponding
  //  y value will be searched for according to the input data
  std::vector<double> Interpolation(
    const std::vector<double>& x,
    const std::vector<double>& x_data,
    const std::vector<double>& y_data);
  int FindFirstLeftIndex(double value,
    std::vector<double> input_vec);
  //return the value (not index) of the maximum value
  //  of the second column
  double MaxSecondColumn(const std::vector<std::vector<double>>& a);
  double AverageSecondColumn(const std::vector<std::vector<double>>& a);
} // namespace usr

std::ostream& operator<< (std::ostream& os,
  const usr::TwoDVector<double>& vec);
} // namespace spdepo
#endif