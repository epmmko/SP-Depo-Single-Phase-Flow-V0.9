#include <iostream>
#include "coolant_condition.h"
#include "fluid_properties.h"
#include "user_flow_enum.h"
namespace spdepo{
CoolantCondition::CoolantCondition() {
}

void CoolantCondition::SetAll(double volume_flow_rate, 
  bool flow_counter_currently, double inlet_temperature) {
  volume_flow_rate_ = volume_flow_rate;
  flow_counter_currently_ = flow_counter_currently;
  inlet_temperature_ = inlet_temperature;
}

std::ostream& operator<< (std::ostream& os, const CoolantCondition& obj) {
  return obj.Print(os);
}

std::ostream& CoolantCondition::Print(std::ostream& os) const {
  os << "Coolant_Fluid_Properties\n";
  os << " flow_counter_currently = ," << flow_counter_currently_ << '\n';
  os << " volume_flow_rate_[m3/s] = ," << volume_flow_rate_ << '\n';
  os << " coolant_inlet_temperature[C] = ," << inlet_temperature_ << '\n';
  return os;
}
bool CoolantCondition::GetBool(usr::kCoolant var_name) const {
  switch(var_name){
    case usr::kCoolant::flow_counter_currently: return flow_counter_currently_;
      break;
  }
}

double CoolantCondition::GetDouble(usr::kCoolant var_name) const {
  switch (var_name) {
    case usr::kCoolant::inlet_temperature:
      return inlet_temperature_;
      break;
    case usr::kCoolant::volume_flow_rate:
      return volume_flow_rate_;
      break;
  }
}
} // namespace spdepo