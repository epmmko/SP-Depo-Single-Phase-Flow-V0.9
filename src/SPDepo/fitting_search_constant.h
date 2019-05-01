#ifndef SPDEPO_FITTING_SEARCH_CONSTANT_H_
#define SPDEPO_FITTING_SEARCH_CONSTANT_H_
#include <map>

namespace spdepo {
namespace usr{
const std::map<int, std::string> kfitting_bool_to_var_name = {
  { 0,"k_alpha" },
  { 1,"A_alpha" },
  { 2,"k_r constant" },
  { 3,"k_r temperature dependent" },
  { 4,"fk_r fractional precipitation rate" },
  { 5,"msr1" },
  { 6,"msr2" },
  { 7,"usr_alpha_coeff0" },
  { 8,"usr_alpha_coeff1" },
  { 9,"usr_alpha_coeff2" },
  { 10,"usr_alpha_coeff3" },
  { 11,"usr_alpha_coeff4" },
  { 12,"usr_alpha_coeff5" } };
}//namespace usr
}//namespace spdepo
#endif
