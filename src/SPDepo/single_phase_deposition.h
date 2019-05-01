#ifndef SINGLE_PHASE_DEPOSITION_H_
#define SINGLE_PHASE_DEPOSITION_H_

#include <vector>

#include "coolant_condition.h"
#include "deposit_properties.h"
#include "fluid_properties.h"
#include "inlet_condition.h"
#include "pipe_geometry.h"
#include "temperature_profile.h"
#include "concentration_profile.h"
#include "solid_properties.h"
#include "user_equation_format.h"
#include "simulation_options.h"
#include "program_path.h"
namespace spdepo {
class FittingDepositionCall; //forward declaration for friend

class SinglePhaseDeposition {
 public:
  friend class CoreDepositionEquation;
  friend class FittingDepositionCall;
  void SetAll(const SimulationOptions& sim_dep_options,
              const Fluid& oil,
              const Fluid& c_fluid,
              const InletCondition& inlet,
              const CoolantCondition& c_flow,
              const PipeGeometry& pipe,
              const Solid& pipe_material);
    //take  
    //1) oil proprerties
    //2) either solubility curve or precipitation curve
  void SetProgramPath(ProgramPath p_path);
  void Calculate();
    //give the answer in terms of ri vs t and
    //                            fw vs t
  void SimplePostCalculation();
  void DetailedPostCalculation();
  void PrintStandardOutput(
    std::vector<std::vector<double>> delta_data_s_m = {},
    std::vector<std::vector<double>> fw_data_s_m = {}
    );
  void PrintDetailedOutput();
  void PrintParameters(
    std::ostream& os, usr::kDepositionPrintOption print_option);
  void PrintPicture(usr::kDepositionPrintOption option) const;
  void PrintXY(std::vector<std::vector<std::wstring>> call_string) const;

 private:
  ProgramPath p_path_;
  void PlotEveryCTProfileOneByOne();
  void PlotEveryCTProfile4Tasks();
  void Initialize();
    //Setup the initial condition for wax deposition
  //input member
  usr::BigEquation precipitation_eq_;
  usr::BigEquation dCdT_eq_;
  SimulationOptions sim_dep_options_;
  CoolantCondition coolant_inlet_;
  DepositProfile deposit_profile_;
  Fluid c_fluid_;
  Fluid oil_;
  InletCondition inlet_;
  heat_calculation::MacroZHeatBalance t_z_profile_;
  PipeGeometry pipe_;
  SinglePhaseHydrodynamics h_flow_;
  Solid pipe_material_;
  Solid wax_;
  WaxProperties wax_prop_;
  int n_zmesh_;
  int n_zsec_;
  std::size_t n_rmesh_;
  double Th0_inlet_;
  double initial_total_oil_volume_;
  double initial_total_oil_mass_;
  double initial_wax_concentration_; //[kg/m3]
    //maximum posible available wax for deposition
    //based on Th_inlet and Tc_inlet
  double wax_molar_volume_cm3_per_gmol_;
  double initial_h_solid_fraction_; //[kg/kg]
    //hot side solid fraction t = 0, based on T h_inlet
  double initial_c_solid_fraction_; //[kg/kg]
    //cold side solid fraction t = 0, based on T c_inlet
  double pipe_radius_;
  //output member
  std::vector<double> tau0_sec_vec_;
    //for MSR1 and MSR2 calculation in
    //Panacharoensawad (2012) model
  usr::TwoDVector<double> ans_ri_fw_;
  std::vector<double> ans_t_; //time in second
  //Post-processing simple member
  std::vector<std::vector<double>> delta_mm_vol_ave_;
    //first column is time in hour
    //delta volume average in millimeter
  std::vector<std::vector<double>> ri_m_vol_ave_;
    //first column is time in hour
    //ri volume average in metre
  std::vector<std::vector<double>> fw_vol_ave_;
    //first column is time in hour
    //fw volume average in fraction w/w
  
  //Post-processing detailed member
    std::vector<std::vector<std::vector<double>>> t_tzr_mesh_post_;
      //fluid temperature profile T(t,z,r)
    std::vector<std::vector<double>> ti_tz_mesh_post_;
      //Ti profile Ti(t,z)
    std::vector<std::vector<double>> ti_ave_post_;
      //Ti average Ti_ave(t)
      //two column, t[hr] and Ti_ave[C]
    std::vector<std::vector<double>> tw_tz_sec_post_;
      //Tw profile Tw(t,z)
    std::vector<std::vector<double>> tw_ave_post_;
      //Tw average Tw_ave(t)
      //two column, t[hr] and Tw_ave[C]
    std::vector<std::vector<std::vector<double>>> c_tzr_mesh_post_;
      //concentration profile C(t,z,r)
    std::vector<std::vector<double>> m_tz_post_;
      //wax mass profile M(t,z), in gram
    std::vector<std::vector<double>> m_total_t_post_;
      //wax mass average M(t)
      //two column, t[hr] and mass[gram]     


  //Deposition Equations
  //The main equations are
  //1) d/dt(ri_vec) = f(ri_vec,fw_vec) = f_ri_vec
  //2) d/dt(fw_vec) = f(ri_vec,fw_vec) = f_fw_vec
  //
  // k1_vec = fn_vec(xi, yi_vec)
  // fn_vec(xi, yi_vec){
  //   vec ans_ri
  //   vec ans_fw
  //   
  //   ans_ri[i] = fn_ri(x, yi_vec[i], yi_vec[i+n])
  //   ans_fw[i] = fn_fw(x, yi_vec[i], yi_vec[i+n])
  //   return ans_ri.insert(ans_ri.end(), ans_fw.begin(), ans_fw.end());
  
  double system_wax_mass_;

  //variable use for output calculation
  double r_;
  
  std::vector<std::wstring> file_name_1D_Profile_ = 
    {L"ti_tz_mesh_post_.csv",
     L"tw_tz_sec_post_.csv",
     L"m_tz_post_.csv"};
  std::vector<std::wstring> file_name_ave_2column_ =
    {L"ti_ave_post_.csv",
     L"tw_ave_post_.csv",
     L"m_total_t_post_.csv"
    };

  std::vector<double> r_ratio_mesh_post_;
};

class CoreDepositionEquation {
 public:
  void SetAll1Phase(const SinglePhaseDeposition& dep);
  friend std::vector<double> WrapSolve1P(double t,
    const std::vector<double>& ri_fw_vec_vec);
  friend class SinglePhaseDeposition;
  //output rhs of deposition equation
  std::vector<double> Solve1P(const std::vector<double>& ri,
                              const std::vector<double>& fw);
 private:
  //fix input variables
  SimulationOptions sim_dep_options_;
  Fluid oil_;
  Fluid c_fluid_;
  InletCondition inlet_;
  CoolantCondition c_flow_;
  PipeGeometry pipe_;
  Solid pipe_material_;
  Solid wax_;
  usr::BigEquation precipitation_eq_;
  WaxProperties wax_prop_;
  std::vector<double> tau0_sec_vec_;
  int n_zsec_;
  int n_rmesh_;

  double initial_total_oil_volume_;
  double initial_wax_concentration_; //[kg/m3]
  //non-const input-output variables
  SinglePhaseHydrodynamics h_flow_;
  heat_calculation::MacroZHeatBalance t_z_profile_;
  DepositProfile deposit_profile_;

  //variable input

  //derived variable
  double r_; //inner pipe radius
  double c_ini0_; //initial concentration of dissolved wax [kg/m3]
  double kr_;
  
  //calculated/output variable
  mass_calculation::RadialCProfile c_radial_;
  double T_ri_;
};



} //namespace spdepo

#endif
