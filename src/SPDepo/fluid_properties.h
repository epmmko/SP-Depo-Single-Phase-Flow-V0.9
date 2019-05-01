#ifndef SPDEPO_TWOPHASEFLOW_FLUID_PROPERTIES_H_
#define SPDEPO_TWOPHASEFLOW_FLUID_PROPERTIES_H_

#include <string>
#include "user_equation_format.h"
#include "user_flow_enum.h"

namespace spdepo{
namespace heat_calculation {
  class RadialTProfile;
} // namespace spdepo
namespace mass_calculation {
  class RadialCProfile; //forward declaration
} // namespace mass_calculation
class Fluid {
 public:
  Fluid();
  //Fluid(const Fluid& obj);
  friend std::ostream& operator<< (std::ostream& os,
    const Fluid& obj);
  friend class SinglePhaseHydrodynamics;
  friend class heat_calculation::RadialTProfile;
  friend class spdepo::mass_calculation::RadialCProfile;
  std::ostream& Print(std::ostream& os) const;
  void SetConstantProperties(const std::string& name = "Air",
    double rho = 1.225, double mu = 1.81e-5,
    double cp = 717, double k = 0.0262);
  void SetConstantValueMember(usr::kFluid property_name, double value);
  void SetName(const std::string& name);
  void SetFunctionalProperties(usr::kFluid var_name,
    const usr::BigEquation& obj);
    //Take the complete BigEquation as an input
  double GetProperty(usr::kFluid var_name, double T_degree_c) const;
  const usr::BigEquation& GetEquation(usr::kFluid var_name) const;
  void GetName(std::string* name) const;
  std::string GetName2() const;
  void GetName3(std::string& name) const;
 private:
  usr::BigEquation rho_;
  usr::BigEquation mu_;
  usr::BigEquation k_;
  usr::BigEquation cp_;
  std::string name_;
};
} // namespace spdepo
#endif

