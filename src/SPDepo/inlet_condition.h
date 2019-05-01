#ifndef SPDEPO_INLET_CONDITION_H_
#define SPDEPO_INLET_CONDITION_H_

#include <iostream>
#include <string>
#include "user_flow_enum.h"

namespace spdepo{
class InletCondition {
public:
  InletCondition();
  friend std::ostream& operator<< (std::ostream& os,
    const InletCondition& obj);
  std::ostream& Print(std::ostream& os) const;
  void SetProperties(const std::string& name,
    double v_sl, double v_sg, double inlet_temperature);
  double GetProperties(usr::kTwoPhaseVar two_phase_var_name);
  void GetName(std::string* name) const;
protected:
  std::string name_;
  double v_sl_;
  double v_sg_;
  double v_m_;
  double inlet_temperature_;
};
} // namespace spdepo
#endif
