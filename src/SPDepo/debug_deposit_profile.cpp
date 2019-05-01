#include "debug_deposit_profile.h"
#include <cmath>
#include "user_math_const.h"
namespace spdepo {
  double Case1Function(double x);
  void TestDepositProfile::SetProfile(DepositProfile* dep,
      int option) {
    int sec_size;
    sec_size = dep->GetSecSize();
    switch(option) {
      case 0: {
        std::vector<double> ri_vec(sec_size,dep->GetPipeRadius() - 0.001);
        dep->SetRiVec(ri_vec);
        dep->SetFwVec(std::vector<double>(sec_size,0.2));
        
      break;
      }
      case 1: { //skew distribution
        // need sec_size = 16 for short pipe case debugging
        std::vector<double> ri_vec;
        for (int i = 0; i < sec_size; i++) {
          ri_vec.push_back(dep->GetPipeRadius() - Case1Function(i));
        }
        dep->SetFwVec(std::vector<double>(sec_size, 0.2));
        dep->SetRiVec(ri_vec);
      break;
      }
    }
  }
  double Case1Function(double x) {
    return 1.0 / std::sqrt(2.0 * usr::pi * std::pow(3,2)) * 
      std::exp(-1.0 * std::pow(x-6.0,2) / 2.0 / std::pow(3,2))
      * 0.009;
  }
}