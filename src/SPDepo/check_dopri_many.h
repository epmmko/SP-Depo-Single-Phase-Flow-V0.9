#ifndef CHECK_DOPRI_MANY_H_
#define CHECK_DOPRI_MANY_H_

#include <vector>

namespace spdepo {
namespace usr {
  void CheckDopriMany();
  void CheckDopriMany2();
  void CheckDopriMany3();
  double fn_vector1(double t, std::vector<double> x);
  double fn_vector2(double t, std::vector<double> x);
  double fn_vector3(double t, std::vector<double> x);
  double fn_all(std::vector<double(*)(double,std::vector<double>)>,
                std::string name);
} //namespace usr
} //namespace spdepo

#endif