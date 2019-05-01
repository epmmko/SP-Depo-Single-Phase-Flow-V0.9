#include <iomanip>
#include <iostream>
#include "fluid_properties.h"
#include "user_flow_enum.h"
//#include "read_input.h"

namespace spdepo{
Fluid::Fluid(){}

void Fluid::SetConstantProperties(const std::string& name,
    double rho, double mu, double cp, double k) {
  name_ = name;
  rho_.SetId(1);
  rho_.SetCoefficient({0, rho});
  rho_.SetName(name);
  rho_.SetComment("None");
  
  mu_.SetId(1);
  mu_.SetCoefficient({ 0, mu});
  mu_.SetName(name);
  mu_.SetComment("None");

  cp_.SetId(1);
  cp_.SetCoefficient({ 0, cp});
  cp_.SetName(name);
  cp_.SetComment("None");

  k_.SetId(1);
  k_.SetCoefficient({ 0, k});
  k_.SetName(name);
  k_.SetComment("None");
}

void Fluid::SetName(const std::string& name) {
  name_ = name;
}

void Fluid::SetConstantValueMember(
    usr::kFluid property_name, double value){
  switch (property_name) {
    case usr::kFluid::rho: {
      rho_.SetId(0);
      rho_.SetCoefficient({value});
      rho_.SetComment("None");
      break;
    }
    case usr::kFluid::mu:{
      mu_.SetId(0);
      mu_.SetCoefficient({value});
      mu_.SetComment("None");
      break;
    }
    case usr::kFluid::k:{
      k_.SetId(0);
      k_.SetCoefficient({value});
      k_.SetComment("None");
      break;
    }
    case usr::kFluid::cp:{
      cp_.SetId(0);
      cp_.SetCoefficient({ value });
      cp_.SetComment("None");
    break;
    }
  }
}

void Fluid::SetFunctionalProperties(usr::kFluid var_name,
    const usr::BigEquation& obj){
  switch (var_name) {
    case usr::kFluid::rho: rho_ = obj; break;
    case usr::kFluid::cp: cp_ = obj; break;
    case usr::kFluid::k: k_ = obj; break;
    case usr::kFluid::mu: mu_ = obj; break;
  }
}

double Fluid::GetProperty(usr::kFluid var_name, double T) const{
  switch (var_name) {
    case usr::kFluid::rho: return rho_.At(T); break;
    case usr::kFluid::cp: return cp_.At(T); break;
    case usr::kFluid::k: return k_.At(T); break;
    case usr::kFluid::mu: return mu_.At(T); break;
  }
}

const usr::BigEquation& Fluid::GetEquation(usr::kFluid var_name) const{
  
  switch (var_name) {
  case usr::kFluid::rho: return rho_; break;
  case usr::kFluid::cp: return cp_; break;
  case usr::kFluid::k: return k_; break;
  case usr::kFluid::mu: return mu_; break;
  }
}

void Fluid::GetName(std::string* name) const {
  *name = name_;
}

std::string Fluid::GetName2() const {
  return name_;
}
void Fluid::GetName3(std::string& name) const {
  name = name_;
}

std::ostream& Fluid::Print(std::ostream& os) const {
  os << std::setprecision(5);
  os << "fluid_name = ," << name_ << '\n';
  os << "  rho[kg/m3] at 20C  = ," << rho_.At(20) << '\n';
  os << "  rho_equation\n";
  os << "  " << rho_;
  os << "  mu[Pa.s] at 20C = ," << mu_.At(20) << '\n';
  os << "  mu_equation\n";
  os << "  " << mu_;
  os << "  cp[J/(kg.K)] at 20C = ," << cp_.At(20) << '\n';
  os << "  co_equation\n";
  os << "  " << cp_;
  os << "  k[W/(m.K)] at 20C = ," << k_.At(20) << '\n';
  os <<"  k_equation\n";
  os << "  " << k_;

  return os;
}

std::ostream& operator<< (std::ostream& os, const Fluid& obj) {
  return obj.Print(os);
}

} // namespace spdepo