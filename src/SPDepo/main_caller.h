#ifndef MAIN_CALLER_H_
#define MAIN_CALLER_H_

#include "fluid_properties.h"
#include "pipe_geometry.h"
#include "inlet_condition.h"
#include "coolant_condition.h"
#include "simulation_options.h"
#include "user_flow_enum.h"
#include "solid_properties.h"
#include "program_path.h"


namespace spdepo {
class FittingDepositionCall; //forward declaration
  //to not include "fitting_search.h"
class Main1PhaseDeposition {
 public:
  void Run(ProgramPath p_path);
};
class Main1PhaseFittingSearch {
 public:
  void Run(ProgramPath p_path);
};

class ReadInput {
 public:
  Fluid ReadLiquid1P();
  void ReadPipe1P(PipeGeometry* pipe_geometry, Solid* pipe_material);
  InletCondition ReadInlet1P();
  void ReadCoolant1P(
    CoolantCondition* coolant,
    Fluid* fluid,
    PipeGeometry* pipe,
    SimulationOptions* sim_option);
  void ReadSimulationOption(SimulationOptions* sim_option,
    const CoolantCondition& coolant_inlet);
    //This call does not set surrounding type in sim_option
  void SetPath(ProgramPath p_path);
  FittingDepositionCall ReadModelTuning();
 private:
  ProgramPath p_path_;
};
}

#endif
