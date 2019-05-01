#ifndef SPDEPO_CONCENTRATION_PROFILE_H_
#define SPDEPO_CONCENTRATION_PROFILE_H_
#include <vector>
#include "temperature_profile.h"
#include "deposit_properties.h"
#include "simulation_options.h"
namespace spdepo {
  class SinglePhaseDeposition; //forward declaration
  class CoreDepositionEquation;//forward declaration
namespace mass_calculation {

  class RadialCProfile {
  public:
    friend class spdepo::SinglePhaseDeposition;
    friend class spdepo::CoreDepositionEquation;
    void SetAll(
      double kr,
      const spdepo::heat_calculation::MacroZHeatBalance& macro_heat,
      const SinglePhaseHydrodynamics& h_hydro,
      const spdepo::WaxProperties wax_prop,
      const std::vector<double>& ri_vec,
      const std::vector<double>& fw_vec,
      double initial_concentration,
      double total_oil_volume,
      const spdepo::SimulationOptions& sim_op);
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
    void GroupTCProfileCalculation(int group_number,
                                   usr::TwoDVector<double>* t_zr,
                                   usr::TwoDVector<double>* c_zr);
    int SectionToGroupNumber(int mesh_number);
    void PlotDefaultCProfile(); //need to run StandardOutput() first
    void PlotDefaultTProfile(); //need to run StandardOutput() first
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
    spdepo::heat_calculation::MacroZHeatBalance macro_heat_;
    spdepo::DepositProfile dep_;
    spdepo::SinglePhaseHydrodynamics h_hydro_;
    spdepo::SimulationOptions sim_op_;

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
    std::vector<double> c_bulk_z_mesh_;
    std::vector<double> t_bulk_z_mesh_;
    std::vector<double> c_r_mesh_;
    std::vector<double> t_r_mesh_;
    std::vector<double> previous_c_r_mesh_;
    std::vector<double> previous_t_r_mesh_;
    std::vector<double> v_r_mesh_;
    std::vector<double> z_mesh_;
    std::vector<std::pair<int, int>> z_group_;
    usr::TwoDVector<double> c_zr_mesh_; //c profile within 1 section
    usr::TwoDVector<double> t_zr_mesh_; //t profile within 1 section
    usr::TwoDVector<double> c_zr_sec_; //c profile within 1 section
    usr::TwoDVector<double> t_zr_sec_; //t profile within 1 section

    std::vector<usr::TwoDVector<double>> t_zr_profile_list_;
    std::vector<usr::TwoDVector<double>> c_zr_profile_list_;
    //2D matrix of temperature profile vs z & r
    //t_zr_[i] is the t_r_ vector 
    //or {t@r0, t@r1, ..., tt@rN-1}
    spdepo::WaxProperties wax_prop_;
    double deposit_roughness_;
    double initial_concentration_; //concentration of wax[kg/m3]
    double total_oil_volume_; // [m3]
    double concentration_inlet_;
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
    //Precipitation Parameter
    double kr_;
    double T_ref_of_kr_; //[C]
    double dwo_Tref_ini_;
  };



} //namespace mass_calculation
} //namespace spdepo

#endif
