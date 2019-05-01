#include "inlet_condition.h"
#include <map>
#include <string>
#include <fstream>
#include "user_equation_format.h"
#include "fluid_properties.h"
namespace spdepo{
void CheckLibrary2() {
  const std::map< char, int> map1 = {
    {'l',1},
    {'e',2}};
  int out1;
  try{
    std::cout << map1.at('k');
  }
  catch (const std::exception& e) {
    std::cout << "error occur\n" << e.what() << '\n';
  }
  catch (const std::string& e) {
    std::cout << "error occur\n" << e;
  }
  catch (...) {
    std::cout << "error occur\n";
  }
  std::vector<double> test_ = {0.1, 0.2, 0.3, 0.4};
  int n = test_.size();
  double sum, x;
  std::ofstream fn_test;
  fn_test.open("fn_debug.csv");
  for (int j = 0; j < 10; j++){
    sum = 0;
    x = j;
    for (int i = 0; i < n; i++) {
      sum = sum + test_[i] * std::pow(x, n - i -1);
    }
    fn_test << x << ',' << sum << '\n';
  }
  fn_test.close();
  usr::Equation test_eq;
  test_eq.SetId(4);
  test_eq.SetCoefficient({ 0.1, 0.2, 0.3, 0.4 });
  std::cout << test_eq.At(1.23) << '\n';
  Fluid water, test_liquid;
  water.SetConstantProperties("water", 1000,0.001,4000,0.76);
  test_liquid = water;
  std::cout << test_liquid;
  std::cout << "\n\n";
  
}
} // namespace spdepo