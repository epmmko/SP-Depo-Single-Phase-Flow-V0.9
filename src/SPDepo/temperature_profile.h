#ifndef SPDEPO_TEMPERATURE_PROFILE_H_
#define SPDEPO_TEMPERATURE_PROFILE_H_
#include <vector>
#include <iostream>
#include <string>
#include <utility>

#include "coolant_condition.h"
#include "deposit_properties.h"
#include "fluid_properties.h"
#include "solid_properties.h"
#include "user_short_name.h"
#include "velocity_profile.h"

namespace spdepo{
  class SinglePhaseDeposition; //forward declaration
  class CoreDepositionEquation;
namespace mass_calculation {
  class RadialCProfile; //forward declaration
} // namespace mass_calculation

namespace heat_calculation{
class MacroZHeatBalance {
  //This class perform a macroscopic heat balance
  //to get bulk T in pipe and annulus as a function of z
  //based on a counter current flow (by default)
 public:
  MacroZHeatBalance();
  friend class spdepo::SinglePhaseDeposition;
  friend class spdepo::CoreDepositionEquation;
  friend class spdepo::mass_calculation::RadialCProfile;
  friend class RadialTProfile;
  friend double WrapNuTurbulentSlope(
    double x, double y);
  friend double WrapTZProfileLoop(double x);
  friend std::ostream& operator<<(std::ostream& os,
    const MacroZHeatBalance);
  std::ostream& Print(std::ostream& os) const;
  std::ostream& PrintTSectionProfile(std::ostream& os) const;
  void SetAll(const SinglePhaseHydrodynamics& h_hydro,
              const CoolantCondition& c_flow, const Fluid& c_fluid,
              const Solid& pipe_prop_);
  void Solve(const DepositProfile& dep); 
    //Solve temperature profile for a certain deposit profile
    //This function update deposit_profile_ with dep.
    //It calls Newton Method to solve for the correct T_c_outlet,
    //  with the provided ForwardTZProfile function (stay inside 
    //  TZProfileLoop which is wrapped inside WrapTZProfileLoop that 
    //  is sent to the Newton Method function.
    //Within this T_c_outlet calculation, th_mesh_ and tc_mesh_ are updated.
    //Then, th_sec_ and tc_sec_ are calculated based
    //  on the th and tc mesh data.
    //The deposit thickness information is used in the hot side Nu
    //  calculation (in TurbulentPipeAverageNusselt function that is located
    //  inside the ForwardTZProfile function). This is to calculate
    //  Nu_infinity.
    //The deposit wax fraction and thickness data are also used in
    //  calculating the heat transfer resistance (Omega) in 
    //  ForwardTZProfile function.
    //This function does not alter h_hydro_.
  void GetProfile(usr::kProfile var_name, std::vector<double>* output) const;
  void MeshToSec(const std::vector<double>& input_mesh,
                 std::vector<double>* output_sec, int n_sec);
                 //get section value by average 2 adjacent mesh values
  void ZDiscretization();
  void HeatBalanceVerification();
  void CalculateSectionT(const DepositProfile& dep);
  int GetMeshSize();
  int GetSecSize();
 private:

  double NuTurbulentSlope(double z_ratio);
  double ForwardTZProfile(double T, DepositProfile dep);
    // calculate profile based on
    //the initial guess of T, just once
  double TZProfileLoop(double T, DepositProfile dep);
    //Call ForwardTZProfile until it converges
  double TurbulentPipeAverageNusselt(double Tb, double Tw,
    double za, double zb, double delta, double deposit_roughness);
  double TurbulentAnnulusAverageNusselt(double Tb, double Tw,
                                     double za, double zb);
  CoolantCondition c_inlet_;
  Fluid h_fluid_;
  Fluid c_fluid_;
  InletCondition h_inlet_;
  PipeGeometry pipe_geometry_;
  SinglePhaseHydrodynamics h_hydro_;
  Solid pipe_prop_;
  DepositProfile deposit_profile_;
  usr::BigEquation rho_h_eq_;
  usr::BigEquation cp_h_eq_;
  usr::BigEquation mu_h_eq_;
  usr::BigEquation k_h_eq_;
  usr::BigEquation rho_c_eq_;
  usr::BigEquation cp_c_eq_;
  usr::BigEquation mu_c_eq_;
  usr::BigEquation k_c_eq_;

  std::vector<double> z_mesh_; //[m] mesh in flow direction over
                                //heat transfer / jacketed section
  std::vector<double> z_sec_; //[m] section mesh in z-direction
//    ___________________________________________________ ...
//    |                    |                    |
//    |       z_sec_[0]    |       z_sec_[1]    |
//    |                    |                    |
//    |____________________|____________________|________ ...
//    |                    |                    |
// z_mesh_[0]           z_mesh_[1]           z_mesh_[2]   

  std::vector<double> cp_h_sec_;
  std::vector<double> cp_c_sec_;
  std::vector<double> hc_sec_;
  std::vector<double> hh_sec_;
  std::vector<double> nu_h_sec_; //pipe side Nusselt number from correlation
  std::vector<double> nu_c_sec_; //annulus Nusselt number from correlation
  std::vector<double> omega_h_sec_;
  std::vector<double> omega_c_sec_;
  std::vector<double> omega_ic_sec_; //used as the boundary condition in
                                     // class RadialTProfile 
  std::vector<double> omega_pipe_sec_;
  std::vector<double> omega_dep_sec_;
  std::vector<double> omega_total_sec_;
  std::vector<double> absQc_radial_sec_;
  std::vector<double> tc_mesh_; //[C] coolant temperature vs z
  std::vector<double> tc_sec_; //[C] hot fluid temperature vs z
  std::vector<double> tc_sec_chk_; //tc based on the heat transfer with av th
  std::vector<double> tcw_sec_;
  std::vector<double> tdep_sec_;
  std::vector<double> th_mesh_; //[C] hot fluid temperature vs z
  std::vector<double> th_sec_;
  std::vector<double> thw_sec_;
  std::vector<double> tri_sec_; //[C] deposit interface temperature vs z
  std::vector<double> ur_inv_mesh_; // 1/(U*r) along z

  
  bool counter_current_;
  bool called_zDiscretization_ = false;
  int mesh_size_;
  int sec_size_;

  double a_p_; //pipe cross section area for flow [m2]
  double a_j_; //jacket cross section area for flow [m2]
  double beta_j_; //beta for jacketed Re* calculation [-];
  double d_;
  double d_o_;
  double d_jhy_;
  double eps_j_; //annulus/jacketed pipe roughness [m]
  double eps_p_; //pipe roughness [m]
  double av_absolute_err_heat_cal_;
  double av_relative_err_heat_cal_;
  double l_ht_; //heat transfer length [m]
  double mu_h_inlet_;
  double mu_c_inlet_;
  double re_inlet_;
  double rho_h_inlet_;
  double rho_c_inlet_;
  double rcw_; //radius at cold wall of the pipe
  double rhw_; //radius at hot wall of the pipe
  double t_h_inlet_;
  double t_c_inlet_;
  double v_h_inlet_;
  double v_c_inlet_;
  double wc_inlet_; //cold side mass flow rate [kg/s]
  double wh_inlet_; //hot side mass flow rate [kg/s]
};

class RadialTProfile {
 public:
  void SetAll(const MacroZHeatBalance& macro_heat,
    const SinglePhaseHydrodynamics& h_hydro);
  std::ostream& PrintParameters(std::ostream& os,
    usr::kRadialTProfile variable);
  void Solve();
    //Requirement: SinglePhaseHydrodynamics.DiscretizeR 
    //               must be run already
    //             MacroZHeatBalance.Solve is run, already
  void StandardOutput();
  void ZDepositGrouping(); 
    //group similar deposit thickness together for
    //calculating radial T profile based on the group
    //average thickness
    //The returned group number is based on section variable (not mesh)
  usr::TwoDVector<double> GroupTProfileCalculation(int group_number);
  int SectionToGroupNumber(int mesh_number);
  void PlotPColorGraph(std::wstring file_rcolumn_zrow = L"tzr.csv",
                       std::wstring file_r_column = L"tzr_r.csv",
                       std::wstring file_z_column = L"tzr_z.csv",
                       std::wstring name_title = L"T[C]",
                       std::wstring name_x_axis = L"r[m]",
                       std::wstring name_y_axis = L"z[m]");
  void HeatBalanceVerification();
 private:
  double DVPlusByDYPlus(double y_plus);
  void CheckRadialHeat();
  void CheckAxialHeat();
  MacroZHeatBalance macro_heat_;
  SinglePhaseHydrodynamics h_hydro_;

  std::vector<double> a_pde_; //coefficient vector for PDE
  std::vector<double> b_pde_; //coefficient vector for PDE
  std::vector<double> c_pde_; //coefficient vector for PDE
  std::vector<double> d_pde_; //coefficient vector for PDE

  std::vector<double> a_thomas_; //vector for Thomas Algorithm
  std::vector<double> b_thomas_; //coefficient vector for PDE
  std::vector<double> c_thomas_; //coefficient vector for PDE
    //defined with respect to the format in Thomas algorithm or
    //ThomasTridiagonal in "User_numerical_method.h"
    //a_Thomas_ is the coefficient C for T_{i,j-1}
  std::vector<double> r_ratio_mesh_;
  std::vector<double> ri_group_; //size of z_group_size_
  std::vector<double> ri_appear_sec_;
  std::vector<double> t_bulk_z_mesh_;
  std::vector<double> t_r_mesh_;
  std::vector<double> previous_t_r_mesh_;
  std::vector<double> v_r_mesh_;
  std::vector<double> z_mesh_;
  std::vector<std::pair<int,int>> z_group_;
  usr::TwoDVector<double> t_zr_mesh_; //t profile within 1 section
  std::vector<usr::TwoDVector<double>> t_zr_profile_list_;
    //2D matrix of temperature profile vs z & r
    //t_zr_[i] is the t_r_ vector 
      //or {t@r0, t@r1, ..., tt@rN-1}
  double deposit_roughness_;
  double pipe_radius_;
  bool solving_first_time_ = true;
  int r_mesh_size_;
  int z_mesh_size_;
  int z_sec_size_;
  int z_group_size_;
  //Checking the result
  std::vector<double> q_radial_;
  std::vector<double> q_axial_;
  std::vector<double> q_axial_coolant_;
  std::vector<double> q_axial_difference_;
  std::vector<double> q_radial_ic_;
  std::vector<double> check_pde_boundary1_;
  std::vector<double> check_pde_boundary2_;
  std::vector<double> check_pde_boundary3_;
};
} // namespace heat_calculation
} // namespace spdepo
#endif
