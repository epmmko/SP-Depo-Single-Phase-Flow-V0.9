#include "inlet_condition.h"
#include <iostream>
#include <iomanip>
#include <string>
#include "user_flow_enum.h"

namespace spdepo{
InletCondition::InletCondition(){
}

void InletCondition::SetProperties(const std::string& name, 
      double v_sl, double v_sg, double inlet_temperature) {
  name_ = name;
  v_sl_ = v_sl;
  v_sg_ = v_sg;
  v_m_ = v_sl + v_sg;
  inlet_temperature_ = inlet_temperature;
}

void InletCondition::GetName(std::string* name) const {
  *name = name_;
}


double InletCondition::GetProperties(usr::kTwoPhaseVar two_phase_var_name) {
  switch (two_phase_var_name) {
    case usr::kTwoPhaseVar::v_sl: return v_sl_; break;
    case usr::kTwoPhaseVar::v_sg: return v_sg_; break;
    case usr::kTwoPhaseVar::v_m: return v_m_; break;
    case usr::kTwoPhaseVar::inlet_temperature: return inlet_temperature_;
      break;
  }
}

std::ostream& InletCondition::Print(std::ostream& os) const {
  os << std::setprecision(5);
  os << "name[-] = ," << name_ << '\n';
  os << "v_sl[m/s] = ," << v_sl_ << '\n';
  os << "v_sg[m/s] = ," << v_sg_ << '\n';
  os << "v_m[m/s] = ," << v_m_ << '\n';
  os << "inlet_temperature[C] = ," << inlet_temperature_ << '\n';
  return os;
}

std::ostream& operator<< (std::ostream& os, const InletCondition& obj) {
  return obj.Print(os);
}
} // namespace spdepo