#include <string>
#include <vector>
#include <iostream>
#include <exception>
#include <iomanip>
#include <cstdint>
#include "user_equation_format.h"
#include "user_string_operation.h"

namespace spdepo{
usr::Equation::Equation(){
}

usr::Equation::Equation(double y) {
  c_.push_back(y);
  id_;
}

void usr::Equation::SetId(int id) {
  id_ = id;
}

void usr::Equation::SetName(const std::string& name) {
  name_ = name;
}

void usr::Equation::SetComment(const std::string& comment) {
  comment_ = comment;
}

void usr::Equation::SetCoefficient(const std::vector<double>& coeff) {
  c_ = coeff;
}


std::vector<double> usr::Equation::GetCoefficient() const {
  return c_;
}

void usr::Equation::SetUserDefinedFunction(double(*f)(double)) {
  f_ = f;
}

double usr::Equation::At(double x) const{
  switch (id_) {
    case -1: { //any 1 variable user-defined function in the format of
      // double(*f)(double)
      return f_(x);
      break;
    }
    case 0: { //constant
      return c_[0];
      break;
    }
    case 1: { //linear
      return c_[0] * x + c_[1];
      break;
    }
    case 2: {//polynomial
      std::size_t n = c_.size();
      double sum = 0;
      for (std::size_t i = 0; i < n; i++) {
        sum = sum + c_[i] * std::pow(x, n - i - 1);
      }
      return sum;
      break;
    }
    case 3: {//linear ratio
      return (c_[0] * x + c_[1]) / (c_[2] * x + c_[3]) + c_[4];
      break;
    }
    case 4: {//simple exponential
      return c_[0] * std::exp(c_[1] * x);
      break;
    }
    case 5: {//complex exponential
      return (c_[0] + c_[1] * x) * std::exp(c_[2] * x) + c_[3];
      break;
    }
    case 6: {//simple power function
      return c_[0] * std::pow(x, c_[1]);
      break;
    }
    case 7: {//complex power function
      return (c_[0] + c_[1] * x) * std::pow(x, c_[2]) + c_[3];
      break;
    }
    case 8: {//simple logarithmic function
      return c_[0] * std::log(c_[1] * x);
      break;
    }
    case 9: {//complex logarithmic function
      return (c_[0] + c_[1] * x) * 
             std::log(c_[2] * x + c_[3]) / c_[4] + c_[5];
      break;
    }
    case 10: {//simple trigonometric function
      return c_[0] * std::sin(x) + c_[1] * std::cos(x);
      break;
    }
    case 11: {//complex trigonometric function
      return c_[0] * std::pow(std::sin(c_[1] * x + c_[2]), c_[3]) +
             c_[4] * std::pow(std::cos(c_[5] * x + c_[6]), c_[7]) +
             c_[8];
      break;
    }
    default:
      std::cout << "invalid type of equation entered\n";
      break;
  }
  return 0;
}

std::ostream& usr::Equation::Print(std::ostream& os) const {
  os << std::setprecision(5);
  os << "equation_name = ," << name_ << '\n';
  os << "  comment  = ," << comment_ << '\n';
  for (int i = 0; i < c_.size(); i++) {
    os << "  c_[" << i << "] = ," << c_[i] << '\n';
  }
  return os;
}

std::ostream& usr::operator<< (std::ostream& os, const usr::Equation& obj) {
  return obj.Print(os);
}

usr::BigEquation::BigEquation() {
  usr::UserOperator op;
  user_op_vec_.assign(10, op);
}

void usr::BigEquation::SetName(const std::string& name) {
  name_ = name;
  //To allow a use as a regular equation
  if (eqn_vec_.size() == 1) {
    eqn_vec_[0].SetName(name);
  }
}

void usr::BigEquation::SetId(int id) {
  Equation f_local;
  f_local.SetId(id);
  if (eqn_vec_.empty()) {
    eqn_vec_.push_back(f_local);
  }
  else {
    eqn_vec_.clear();
    eqn_vec_.push_back(f_local);
  }
}

void usr::BigEquation::SetCoefficient(const std::vector<double>& coeff) {
  if (!eqn_vec_.empty()) {
    eqn_vec_[0].SetCoefficient(coeff);
  }
  else {
    std::cout << "need to SetId first\n";
    std::system("pause");
  }
}

void usr::BigEquation::SetComment(const std::string& comment) {
  if (!eqn_vec_.empty()) {
    eqn_vec_[0].SetComment(comment);
  }
  else {
    std::cout << "need to SetId first\n";
    std::system("pause");
  }
}

void usr::BigEquation::AppendEquation(const Equation& eqn) {
  eqn_vec_.push_back(eqn);
}

void usr::BigEquation::AppendOperator(const UserOperator& op) {
  user_op_vec_.push_back(op);
}

void usr::BigEquation::ClearEquation() {
  eqn_vec_.clear();
}

void usr::BigEquation::ClearOperator() {
  user_op_vec_.clear();
}

void usr::BigEquation::SetEquationVector(
    const std::vector<Equation>& eq_vec) {
  eqn_vec_ = eq_vec;
}

void usr::BigEquation::SetOperatorVector(
  const std::vector<usr::UserOperator>& op_vec) {
  user_op_vec_ = op_vec;
}

double usr::BigEquation::At(double x) const{
  int i = 0;
  double ans;
  std::size_t eqn_size;//size of the equation vector
  std::size_t op_size; //size of the UserOperator vector
  eqn_size = eqn_vec_.size();
  op_size = eqn_size - 1;
  if (eqn_size > 1) {
    ans = user_op_vec_[i].Evaluate(eqn_vec_[i + 1], eqn_vec_[i], x);
  }
  else {
    return eqn_vec_[0].At(x);
  }
  for (std::size_t i = 1; i < op_size; i++) {
    ans = user_op_vec_[i].Evaluate(eqn_vec_[i + 1], ans, x);
  }
  return ans;
}

void usr::BigEquation::ParseFromString(const std::string& input_string) {
  std::vector<std::vector<std::string>> eq_elements;
  std::vector<std::vector<double>> eq_elements_dbl;
  eq_elements = usr::ExtractListOfList(input_string);
  eq_elements_dbl = usr::ListOfListOfStringToDouble(eq_elements);
    //parse string to vector<vector<double>>
  std::vector<double> equation_format_list = eq_elements_dbl[0];
  std::size_t number_of_equation = equation_format_list.size();
  std::vector<double> operator_format_list = eq_elements_dbl[1];
  std::vector<std::vector<double>> nested_list_coefficient(
      eq_elements_dbl.begin() + 2, eq_elements_dbl.end());
      //form each elements needed for creating the big-equation
  std::vector<usr::Equation> equations(number_of_equation,usr::Equation());
  for (int i = 0; i < number_of_equation; i++) {
    equations[i].SetId(static_cast<int>(equation_format_list[i]));
    equations[i].SetCoefficient(nested_list_coefficient[i]);
  }
  //put vector of equation into this BigEquation
  this->SetEquationVector(equations);
  if (number_of_equation > 1) {
      //seting up the operator vector
      //if 1 equation use default 10 UserOperator ID 0 in the constructor
    std::vector<usr::UserOperator> operator_list = {};
    for (int i = 0; i < number_of_equation - 1; i++) {
      usr::UserOperator op;
      op.SetId(static_cast<int>(operator_format_list[i]));
      operator_list.push_back(op);
    }
    this->SetOperatorVector(operator_list);
  }
}

usr::UserOperator::UserOperator() {
}

void usr::UserOperator::SetId(int id) {
  id_ = id;
}

double usr::UserOperator::Evaluate(const Equation& eq2,
    const Equation& eq1, double x) const{
  switch (id_) {
    case 0: {
      return eq2.At(eq1.At(x));
      break;
    }
    case 1: {
      return eq1.At(eq2.At(x));
      break;
    }
    case 2: {
      return eq1.At(x) + eq2.At(x);
      break;
    }
    case 3: {
      return eq2.At(x) - eq1.At(x);
      break;
    }
    case 4: {
      return eq1.At(x) - eq2.At(x);
      break;
    }
    case 5: {
      return eq1.At(x) * eq2.At(x);
      break;
    }
    case 6: {
      return eq2.At(x) / eq1.At(x);
      break;
    }
    case 7: {
      return eq1.At(x) / eq2.At(x);
      break;
    }
    case 8: {
      return std::pow(eq2.At(x), eq1.At(x));
      break;
    }
    case 9: {
      return std::pow(eq1.At(x), eq2.At(x));
      break;
    }
  }
}


std::ostream& usr::operator<< (std::ostream& os, const usr::BigEquation& obj) {
  int size;
  size = obj.eqn_vec_.size();
  os << "BigEquation\n";
  os << "  Equation Number = ," << obj.eqn_vec_.size() << '\n';
  for (int i = 0; i < size; i++) {
    os << obj.eqn_vec_[i] << '\n';
  }
  os << "  Operator Number = ," << size - 1 << '\n';
  for (int i = 0; i < size-1; i++) {
    os << obj.user_op_vec_[i] << '\n';
  }
  return os;
}

std::ostream& usr::operator<< (std::ostream& os, const usr::UserOperator& obj) {
  return obj.Print(os);
}

std::ostream& usr::UserOperator::Print(std::ostream& os) const {
  os << std::setprecision(5);
  os << "  op_::id_ = ," << id_ << '\n';
  return os;
}

std::vector<usr::Equation> usr::BigEquation::GetEquationVector() const {
  return eqn_vec_;
}
std::vector<usr::UserOperator> usr::BigEquation::GetOperatorVector() const {
  return user_op_vec_;
}
} // namespace spdepo