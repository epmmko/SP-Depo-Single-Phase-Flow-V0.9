#ifndef SPDEPO_USER_EQUATION_FORMAT
#define SPDEPO_USER_EQUATION_FORMAT

#include <string>
#include <vector>
#include <iostream>

namespace spdepo{
namespace usr {
class Equation {
  public:
  Equation();
  Equation(double y); //conversion constructor
  friend class NestedEquation;
  //id = -1 for user defined function, need to SetUserDefinedFunction f_
  //id = 0 for constant, return c_[0]
  //id = 1 for linear, return c_[0]*x + c_[1]
  //id = 2 for polynomial, return c_[0]*x^(n-1) + c_[1]*x^n + ... + c_[n-1]
  //id = 3 for linear ratio,
  //  return (c_[0] * x + c_[1]) / (c_[2] * x + c_[3]) + c_[4]
  //id = 4 for simple exponential, return c_[0] * std::exp(c_[1] * x)
  //id = 5 for complex exponential,
  //  return (c_[0] + c_[1] * x) * std::exp(c_[2] * x) + c_[3]
  //id = 6 for simple power function,
  //  return c_[0] * std::pow(x, c_[1])
  //id = 7 for complex power function
  //  return (c_[0] + c_[1] * x) * std::pow(x, c_[2]) + c_[3]
  //id = 8 for simple logarithmic function
  //  return c_[0] * std::log(c_[1] * x)
  //id = 9 for complex logarithmic function, return
  //  (c_[0] + c_[1] * x) * std::log(c_[2] * x + c_[3]) / c_[4] + c_[5]
  //id = 10 for simple trigonometric function, return
  //  c_[0] * std::sin(x) + c_[1] * std::cos(x)
  //id = 11 for complex trigonometric function, return
  //  c_[0] * std::pow(std::sin(c_[1] * x + c_[2]), c_[3]) +
  //  c_[4] * std::pow(std::cos(c_[5] * x + c_[6]), c_[7]) +
  //  c_[8];
  void SetId(int id);
  void SetName(const std::string& name);
  void SetCoefficient(const std::vector<double>& coeff);
  void SetComment(const std::string& comment);
  std::vector<double> GetCoefficient() const;
  double At(double x) const;
  friend std::ostream& operator<< (std::ostream& os,
    const Equation& obj);
  std::ostream& Print(std::ostream& os) const;
  void SetUserDefinedFunction(double(*f)(double));
  private:
  std::string name_;
  std::string comment_;
  std::vector<double> c_;
  double(*f_)(double);
  //c_[0] = c0, c_[1] = c1, ...
  int id_ = 0;
};

class UserOperator {
public:
  friend std::ostream& operator<< (std::ostream& os,
    const UserOperator& obj);
  UserOperator();

  //id = 0 return eq2.At(eq1.At(x));
  //id = 1 return eq1.At(eq2.At(x));
  //id = 2 return eq1.At(x) + eq2.At(x);
  //id = 3 return eq2.At(x) - eq1.At(x);
  //id = 4 return eq1.At(x) - eq2.At(x);
  //id = 5 return eq1.At(x) * eq2.At(x);
  //id = 6 return eq2.At(x) / eq1.At(x);
  //id = 7 return eq1.At(x) / eq2.At(x);
  //id = 8 return std::pow(eq2.At(x), eq1.At(x));
  //id = 9 return std::pow(eq1.At(x), eq2.At(x));
  void SetId(int id);
  std::ostream& Print(std::ostream& os) const;
  double Evaluate(const Equation& eq2, const Equation& eq1, double x) const;
private:
  int id_ = 0; 
    //co-function is the default mode.
    //This is automatically added via BigEquation constructor
    //default is nested equation mode (up to 10 equations)
};

class BigEquation {
 public:
  friend std::ostream& operator<< (std::ostream& os,
    const BigEquation& obj);
  BigEquation();
  BigEquation(const std::string& input_string);
  void ClearEquation();
  void AppendEquation(const Equation& eqn);
  void ClearOperator();
  void AppendOperator(const UserOperator& op);
    //Be default (via constructor), 10 mode-0 operators
    // are in place, initially.
    //You should ClearOperator() before append to avoid
    // any unexpected result
  void ParseFromString(const std::string& input_string);
  void SetEquationVector(const std::vector<Equation>& eq_vec);
  std::vector<Equation> GetEquationVector() const;
  void SetOperatorVector(const std::vector<UserOperator>& op_vec);
  std::vector<UserOperator> GetOperatorVector() const;
  double At(double x) const;

  //To use it as a regular equation
  void SetName(const std::string& name);
  //id = -1 for user defined function, need to SetUserDefinedFunction f_
  //id = 0 for constant, return c_[0]
  //id = 1 for linear, return c_[0]*x + c_[1]
  //id = 2 for polynomial, return c_[0]*x^(n-1) + c_[1]*x^n + ... + c_[n-1]
  //id = 3 for linear ratio,
  //  return (c_[0] * x + c_[1]) / (c_[2] * x + c_[3]) + c_[4]
  //id = 4 for simple exponential, return c_[0] * std::exp(c_[1] * x)
  //id = 5 for complex exponential,
  //  return (c_[0] + c_[1] * x) * std::exp(c_[2] * x) + c_[3]
  //id = 6 for simple power function,
  //  return c_[0] * std::pow(x, c_[1])
  //id = 7 for complex power function
  //  return (c_[0] + c_[1] * x) * std::pow(x, c_[2]) + c_[3]
  //id = 8 for simple logarithmic function
  //  return c_[0] * std::log(c_[1] * x)
  //id = 9 for complex logarithmic function, return
  //  (c_[0] + c_[1] * x) * std::log(c_[2] * x + c_[3]) / c_[4] + c_[5]
  //id = 10 for simple trigonometric function, return
  //  c_[0] * std::sin(x) + c_[1] * std::cos(x)
  //id = 11 for complex trigonometric function, return
  //  c_[0] * std::pow(std::sin(c_[1] * x + c_[2]), c_[3]) +
  //  c_[4] * std::pow(std::cos(c_[5] * x + c_[6]), c_[7]) +
  //  c_[8];
  void SetId(int id);
  void SetCoefficient(const std::vector<double>& coeff);
  void SetComment(const std::string& comment);
 private:
  std::string name_;
  std::vector<Equation> eqn_vec_;
  std::vector<UserOperator> user_op_vec_;
};

} // namespace usr
} // namespace spdepo
#endif
