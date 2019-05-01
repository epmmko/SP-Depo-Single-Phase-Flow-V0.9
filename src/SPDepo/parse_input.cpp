#include "parse_input.h"
#include "fluid_properties.h"
#include <iostream>
namespace spdepo {
Fluid ParseInput::ParseFluid(usr::kReadInput input_enum) {
  switch (input_enum) {
    case(usr::kReadInput::oil):{
      std::cout << "test\n";
      break;
    }
  }
  Fluid temp;
  return temp;
}
}