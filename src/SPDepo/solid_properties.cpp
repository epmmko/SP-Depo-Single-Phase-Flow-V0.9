#include <iomanip>
#include <iostream>
#include "solid_properties.h"
#include "user_flow_enum.h"
//#include "read_input.h"

namespace spdepo{
Solid::Solid() {}

void Solid::SetConstantProperties(const std::string& name,
  double rho, double cp, double k) {
  name_ = name;
  rho_.SetId(1);
  rho_.SetCoefficient({ 0, rho });
  rho_.SetName(name);
  rho_.SetComment("None");

  cp_.SetId(1);
  cp_.SetCoefficient({ 0, cp });
  cp_.SetName(name);
  cp_.SetComment("None");

  k_.SetId(1);
  k_.SetCoefficient({ 0, k });
  k_.SetName(name);
  k_.SetComment("None");
}

void Solid::SetFunctionalProperties(usr::kSolid var_name,
  const usr::BigEquation& obj) {
  switch (var_name) {
    case usr::kSolid::rho: rho_ = obj; break;
    case usr::kSolid::cp: cp_ = obj; break;
    case usr::kSolid::k: k_ = obj; break;
  }
}

double Solid::GetProperty(usr::kSolid var_name, double T) const {
  switch (var_name) {
  case usr::kSolid::rho: return rho_.At(T); break;
  case usr::kSolid::cp: return cp_.At(T); break;
  case usr::kSolid::k: return k_.At(T); break;
  }
}

const usr::BigEquation& Solid::GetEquation(usr::kSolid var_name) const {
  switch (var_name) {
  case usr::kSolid::rho: return rho_; break;
  case usr::kSolid::cp: return cp_; break;
  case usr::kSolid::k: return k_; break;
  }
}

void Solid::GetName(std::string& name) const {
  name = name_;
}

std::ostream& Solid::Print(std::ostream& os) const {
  os << std::setprecision(5);
  os << "Solid_name = ," << name_ << '\n';
  os << "  rho[kg/m3] at 20C  = ," << rho_.At(20) << '\n';
  os << "  rho_equation\n";
  os << "  " << rho_;
  os << "  cp[J/(kg.K)] at 20C = ," << cp_.At(20) << '\n';
  os << "  co_equation\n";
  os << "  " << cp_;
  os << "  k[W/(m.K)] at 20C = ," << k_.At(20) << '\n';
  os << "  k_equation\n";
  os << "  " << k_;
  return os;
}

std::ostream& operator<< (std::ostream& os, const Solid& obj) {
  return obj.Print(os);
}
} // namespace spdepo