#ifndef SPWAX_VELOCITY_PROFILE_H_
#define SPWAX_VELOCITY_PROFILE_H_
#include <vector>
#include <iostream>
#include "user_short_name.h"
#include "fluid_properties.h"
#include "inlet_condition.h"
#include "pipe_geometry.h"
#include "user_flow_enum.h"
namespace spdepo{
namespace heat_calculation {
  class RadialTProfile; //forward declaration
} // namespace spdepo
namespace mass_calculation {
  class RadialCProfile; //forward declaration
} // namespace mass_calculation
class SinglePhaseHydrodynamics {
 public:
  void SetAll(const Fluid& fluid, const PipeGeometry& pipe,
    double average_velocity, const InletCondition& inlet);
  void SetFluidProperties(const Fluid& fluid);
  void SetInletFlow(const InletCondition& inlet);
  void SetPipeGeometry(const PipeGeometry& pipe);
  void Solve(double temperature_in_c, double delta = 0.0,
    double roughness = 0.0);
    //This function calculate the required hydrodynamics 
    //  variable. Then, it select either SolveTurbulent(T) or
    //  SolveLaminar(T), as appropriate (based on Reynolds number).
    //  This function update v_ and re_ to be consistent with the
    //  input deposit thickness.
    //The output is the vector v which is corresponding to the
    //  r_ratio_mesh_ vector where the v* used to calculate v
    //  is calculated based on the v_ and re_ that are a function
    //  of the input deposit thickness.
  double SolveOnlyShearStress(double temperature_in_c,
    double ri, double roughness);
    //return the result in Pa
  friend class heat_calculation::RadialTProfile;
    //class RadialTProfile need the direct access to this class
  friend class spdepo::mass_calculation::RadialCProfile;
  //class RadialCProfile need the direct access to this class
  friend std::ostream& operator<< (std::ostream& os,
    const SinglePhaseHydrodynamics& obj);
  friend double WrapOneSeventhSlope(double x, double y);
  friend double WrapLaminarVelocitySlope(double x, double y);
  friend class PrintRMesh;
  friend class PrintVRMesh;
  std::ostream& Print(std::ostream& os) const;
  std::ostream& PrintRProfile(std::ostream& os) const;
  std::ostream& PrintRVProfile(std::ostream& os) const;
  void GetPipeGeometry(PipeGeometry* pipe) const;
  void GetInletCondition(InletCondition* inlet) const;
  void GetFluid(Fluid* fluid) const;
  std::vector<double> GetRMeshRatio() const;
  double GetProperties(usr::kSinglePhaseFlow var_name) const;
  double HaalandEquation(double eps,
    double d, double re) const;
  void DiscretizeR(double T, double delta = 0.0);
    //just take T to calculate Reynolds number
    //All turbulent cases are discretized with 1/7 laws
    //All laminar cases are discretized the same way
 private:
  void DiscretizeRRatioMesh_turbulent();
  void DiscretizeRRatioMesh_laminar();
  double LaminarVelocitySlope_(double r_ratio, double v);
    //d(v/v_max)/d(r/R), used for discretization
  double OneSeventhLaws_(double r_ratio);
    //return v/v_max for turbulent flow with 1/7 law
      //input is r/R
  double OneSeventhLawsSlope_(double r_ratio, double v);
    //d(v/v_max)/d(r/R), used for discretization
  void SolveTurbulent(double temperature_in_c);
  void SolveLaminar(double temperature_in_c);
  double VPlusTurbulent(double y_plus);
  double VLaminar(double r_ratio);

  Fluid fluid_;
  InletCondition inlet_;
  PipeGeometry pipe_;

  std::vector<double> r_ratio_mesh_;
  std::vector<double> v_at_r_mesh_;
  std::vector<double> y_plus_mesh_;
  std::vector<double> v_plus_mesh_;

  int max_num_section = 50;
  int rmesh_size_ = 0;

  double v_; 
  double v_inlet_; //average velocity at the inlet
  double v_star_; //frictional velocity
  double re_; //Reynolds number
  double tau_w_;
  double fanning_;
  double minus_dp_dl_total_;
  double minus_dp_dl_friction_;
  double minus_dp_dl_gravity_;


};
} // namespace spdepo
#endif
