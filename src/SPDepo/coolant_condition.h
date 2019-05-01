#ifndef SPDEPO_COOLANT_CONDITION_H_
#define SPDEPO_COOLANT_CONDITION_H_
#include <iostream>
#include "fluid_properties.h"
namespace spdepo{
class CoolantCondition {
 public:
  CoolantCondition();
  friend std::ostream& operator<< (std::ostream& os,
    const CoolantCondition& obj);
  std::ostream& Print(std::ostream& os) const;
  void SetAll(double volume_flow_rate, bool flow_counter_currently,
    double inlet_temperature);
  bool GetBool(usr::kCoolant var_name) const;
  double GetDouble(usr::kCoolant var_name) const;
 private:
  bool flow_counter_currently_;
  double volume_flow_rate_; //[m3/s]
  double inlet_temperature_; //[C]
};
} // namespace spdepo
#endif
