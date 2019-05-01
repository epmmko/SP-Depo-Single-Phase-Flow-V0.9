#ifndef SPDEPO_PARSE_INPUT_H_
#define SPDEPO_PARSE_INPUT_H_

#include "user_flow_enum.h"
#include "fluid_properties.h"

namespace spdepo{
class ParseInput{
 public:
  Fluid ParseFluid(usr::kReadInput input_enum);
};
} //namespace spdepo

#endif
