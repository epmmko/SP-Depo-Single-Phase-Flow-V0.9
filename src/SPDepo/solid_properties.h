#ifndef SPDEPO_SOLID_PROPERTIES_H_
#define SPDEPO_SOLID_PROPERTIES_H_

#include <string>
#include "user_equation_format.h"
#include "user_flow_enum.h"

namespace spdepo{
class Solid {
public:
  Solid();
  //Fluid(const Fluid& obj);
  friend std::ostream& operator<< (std::ostream& os,
    const Solid& obj);
  std::ostream& Print(std::ostream& os) const;
  void SetConstantProperties(const std::string& name = "Copper",
    double rho = 8960.0, double cp = 390.0, double k = 400.0);
  void SetFunctionalProperties(usr::kSolid var_name,
    const usr::BigEquation& obj);
  double GetProperty(usr::kSolid var_name, double T) const;
  void GetName(std::string& name) const;
  const usr::BigEquation& GetEquation(usr::kSolid var_name) const;
private:
  usr::BigEquation rho_;
  usr::BigEquation k_;
  usr::BigEquation cp_;
  std::string name_;
};
} // namespace spdepo
#endif



