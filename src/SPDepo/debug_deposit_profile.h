#ifndef DEBUG_DEPOSIT_PROFILE_H_
#define DEBUG_DEPOSIT_PROFILE_H_

#include "deposit_properties.h"

namespace spdepo {
class TestDepositProfile{
 public:
  void SetProfile(DepositProfile* dep, int option);
};
} // namespace spdepo
#endif