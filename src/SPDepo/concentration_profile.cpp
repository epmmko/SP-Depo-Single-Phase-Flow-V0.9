#pragma warning(disable : 4996)
#include <cmath> //for fmax
#include <iomanip>
#include <utility>
#include <algorithm>
#include <time.h>
#include <string>
#include <Windows.h>
#include "deposit_properties.h"
#include "solid_properties.h"
#include "temperature_profile.h"
#include "concentration_profile.h"
#include "user_flow_enum.h"
#include "user_numerical_method.h"
#include "user_python_script.h"
#include "simulation_options.h"


namespace spdepo {
namespace mass_calculation {


void RadialCProfile::SetAll(
    double kr,
    const spdepo::heat_calculation::MacroZHeatBalance& macro_heat,
    const SinglePhaseHydrodynamics& h_hydro,
    const spdepo::WaxProperties wax_prop,
    const std::vector<double>& ri_vec,
    const std::vector<double>& fw_vec,
    double initial_concentration,
    double total_oil_volume,
    const spdepo::SimulationOptions& sim_op){
  kr_ = kr;
  macro_heat_ = macro_heat;
  h_hydro_ = h_hydro;
  r_mesh_size_ = h_hydro.rmesh_size_;
  r_ratio_mesh_ = h_hydro.r_ratio_mesh_;
  z_mesh_size_ = macro_heat.mesh_size_;
  z_mesh_ = macro_heat.z_mesh_;
  z_sec_size_ = macro_heat.sec_size_;
  t_r_mesh_.assign(r_mesh_size_, macro_heat.th_mesh_[0]);
  previous_t_r_mesh_.assign(r_mesh_size_, macro_heat.th_mesh_[0]);
  ri_appear_sec_.assign(z_sec_size_, 0.0);
  a_thomas_.assign(r_mesh_size_, 0.0);
  b_thomas_.assign(r_mesh_size_, 0.0);
  c_thomas_.assign(r_mesh_size_, 0.0);

  a_pde_.assign(r_mesh_size_, 0.0);
  b_pde_.assign(r_mesh_size_, 0.0);
  c_pde_.assign(r_mesh_size_, 0.0);
  d_pde_.assign(r_mesh_size_, 0.0);

  dep_ = macro_heat.deposit_profile_;
  deposit_roughness_ = macro_heat_.deposit_profile_.overall_roughness_;
  pipe_radius_ = macro_heat_.d_ * 0.5;

  wax_prop_ = wax_prop;
  initial_concentration_ = initial_concentration;
  total_oil_volume_ = total_oil_volume;

  concentration_inlet_ = (initial_concentration * total_oil_volume -
    dep_.GetTotalWaxMass(macro_heat_.tdep_sec_)) / total_oil_volume;

  c_r_mesh_.assign(r_mesh_size_, concentration_inlet_);
  previous_c_r_mesh_.assign(r_mesh_size_, concentration_inlet_);
  sim_op_ = sim_op;
}
void RadialCProfile::Solve() {
  for (int j = 0; j < 2; j++) { //make T_pde iterate twice
                                //This is because the first iteration,
                                //use T r-profile at i-1
                                //for the fluid properties at i
                                //The second iteration use T r-profile at i
                                //(from the previous iteration)
                                //for the fluid properties at i
    t_bulk_z_mesh_.clear();
    t_bulk_z_mesh_.push_back(macro_heat_.th_mesh_[0]);
    c_bulk_z_mesh_.clear();
    c_bulk_z_mesh_.push_back(concentration_inlet_);
    if (solving_first_time_) {
      usr::TwoDVector<double> temp = { {} };
      t_zr_profile_list_.assign(z_group_size_, temp);
      c_zr_profile_list_.assign(z_group_size_, temp);
    }
    for (int i = 0; i < z_group_size_; i++) {
      //Use the previus-z the T_profile for fluid properties calculation
      //  in T_profile (itself) calculation. Then, iterate it.
      //  until the T_profile input = output.


      
      GroupTCProfileCalculation(i,
        &t_zr_profile_list_[i],
        &c_zr_profile_list_[i]);
    }
    solving_first_time_ = false;
  }

  t_zr_mesh_.clear();
  c_zr_mesh_.clear();
  //convert back from a group format to 
  //t profile in a regular format.
  for (int i = 0; i < z_group_size_; i++) {
    for (int k = 0; k < t_zr_profile_list_[i].size(); k++) {
      t_zr_mesh_.push_back(t_zr_profile_list_[i][k]);
      c_zr_mesh_.push_back(c_zr_profile_list_[i][k]);
    }
  }

  t_zr_sec_ = usr::MeshToSec(t_zr_mesh_);
  c_zr_sec_ = usr::MeshToSec(c_zr_mesh_);

  
}

  void RadialCProfile::GroupTCProfileCalculation(
    int group_number,
    usr::TwoDVector<double>* t_zr,
    usr::TwoDVector<double>* c_zr){
  //Group specification
  int start_z_mesh;
  int end_z_mesh;
  int previous_end_z_mesh;
  int size;
  //Group variable
  double ri;
  double delta;
  double t_bulk;
  double y_plus;
  double fanning;
  double re;
  //local radial variable
  double alpha_h;
  double alpha_m;
  double cp;
  double dr_j;
  double dr_jp1;
  double epsilon_over_nu;
  double epsilon_eddy_h;
  double epsilon_eddy_m;
  double k;
  double mu;
  double prandtl;
  double schmidt;
  double prandtl_t; //turbulent Pr
  double schmidt_t; //turbulent Sc
  double rho;
  double t_local; //local radial temperature
                  //variable for A, B, C, and D discretized PDE calculation
  double a_j;   //\alpha_j
  double a_jm1; //\alpha_{j-1}
  double a_jp1; //\alpha_{j+1}
  double e_j;   //\epsilon_j
  double e_jm1; //\epsilon_{j-1}
  double e_jp1; //\epsilon_{j+1}
  int N;
  double r_j;   //r_j
  double r_jm1; //r_{j-1}
  double r_jp1; //r_{j+1}
  double t_im1; //T_{i-1}
  double c_im1;
  double v_ij;  //V_{i,j}
  double dz_i;  //\Delta z_i

  std::vector<double> alpha_h_vec(r_mesh_size_, 0.0);
  std::vector<double> alpha_m_vec(r_mesh_size_, 0.0);
    //alpha_m is D_wo mass diffusivity in oil (binary)
  std::vector<double> epsilon_h_vec(r_mesh_size_, 0.0);
  std::vector<double> epsilon_m_vec(r_mesh_size_, 0.0);
  std::vector<double> fw_vec;
  std::vector<double> t_x_rratio(r_mesh_size_, 0.0);
    // T * r_ratio
  std::vector<double> z_vec;
  // use for <T> calculation
  usr::TwoDVector<double> output;
  //previous_t_r_mesh_ at the inlet has been initialized
  //  in RadialTProfile::SetAll method.
  start_z_mesh = z_group_[group_number].first;
  end_z_mesh = z_group_[group_number].second + 1;
  //+1 here is to get the mesh value at the end of the section
  //value from z_group_ is the section value (not mesh)
  ri = ri_group_[group_number];
  //vector copy constructor does not include the last iterator
  // [first, last). Thus, +1 is needed.
  z_vec = std::vector<double>(z_mesh_.begin() + start_z_mesh,
    z_mesh_.begin() + end_z_mesh + 1);
  delta = pipe_radius_ - ri;
  //initialize the previus T/C profile (use to start the marching algorithm)
  if (group_number == 0) {
    previous_t_r_mesh_.assign(r_mesh_size_, macro_heat_.th_mesh_[0]);
    previous_c_r_mesh_.assign(r_mesh_size_, concentration_inlet_);
    //take the inlet information as the answer
    //do this just at the inlet, not at the start of every group
    (*t_zr)[0] = previous_t_r_mesh_;
    (*c_zr)[0] = previous_c_r_mesh_;
  }
  else {
    previous_end_z_mesh = t_zr_profile_list_[group_number - 1].size() - 1;
    previous_t_r_mesh_ =
      t_zr_profile_list_[group_number - 1][previous_end_z_mesh];
    previous_c_r_mesh_ =
      c_zr_profile_list_[group_number - 1][previous_end_z_mesh];
  }

  //Setting t_r_mesh_ for bulk T and fluid properties
  //at the current z-mesh calculation
  //By default, use the previous mesh fluid properties,
  //for the current mesh calculation.
  //If it is not the first time doing this,
  //use the previus iteration (this adjustment is in z-group loop).
  t_r_mesh_ = previous_t_r_mesh_;
  int start_adjustment;
  if (group_number == 0) {
    start_adjustment = 0;
  }
  else {
    start_adjustment = 1;
  }
  for (int i = start_z_mesh + 1; i <= end_z_mesh; i++) {
    //Calculate flow average temperature
    //If this is not the first iteration,
    //  friction factor, v* and fluid properties are calculated
    //  based on the t_r_mesh_ of the previous iteration
    //  otherwise it is based on the t_r_mesh_ of the previous z

    if (!solving_first_time_) {
      //use for bulk T calculation purpose
      t_r_mesh_ = t_zr_profile_list_[group_number][i - (start_z_mesh + 1)];
    }
  //************* may use flow average properties,
  //      instead of arithmetic average*************

    dz_i = z_mesh_[i] - z_mesh_[i - 1];
    
    for (int j = 0; j < r_mesh_size_; j++) {
      t_x_rratio[j] = t_r_mesh_[j] * r_ratio_mesh_[j];
    }
    t_bulk = usr::Trapezoidal(r_ratio_mesh_, t_x_rratio) * 2.0;
      //multiplication with 2.0 is needed for integrating in r_ratio domain
    //Get radial velocity profile at the current mesh point (v_ij)
    h_hydro_.Solve(t_bulk, delta);
    fanning = h_hydro_.fanning_;
    re = h_hydro_.re_;
    //Calculation of alpha_h and epsilon_h at every r-mesh
    for (int j = 0; j < r_mesh_size_; j++) {
      t_local = t_r_mesh_[j];
      rho = h_hydro_.fluid_.rho_.At(t_local);
      mu = h_hydro_.fluid_.mu_.At(t_local);
      cp = h_hydro_.fluid_.cp_.At(t_local);
      k = h_hydro_.fluid_.k_.At(t_local);
      prandtl = mu * cp / k;
      prandtl_t = 0.85 + 0.015 / prandtl;
      alpha_h = k / rho / cp;
      y_plus = (1.0 - r_ratio_mesh_[j]) * re / 2.0 *
        std::sqrt(fanning / 2.0);
      //See W. M. Deen Analysis of Transport Phenomena page 529
      //  for the formula of epsilon_eddy (eddy diffusivity)
      if (y_plus <= 80.0) {
        epsilon_over_nu = std::pow(0.4 * y_plus, 2) *
          std::pow(1.0 - std::exp(-y_plus / 26.0), 2) *
          DVPlusByDYPlus(y_plus);
      }
      else {
        epsilon_over_nu = 0.4 * y_plus / 6.0 *
          (1.0 + r_ratio_mesh_[j]) *
          (1.0 + 2.0 * std::pow(r_ratio_mesh_[j], 2));
      }
      epsilon_eddy_h = prandtl / prandtl_t * epsilon_over_nu * alpha_h;
      alpha_h_vec[j] = alpha_h;
      epsilon_h_vec[j] = epsilon_eddy_h;
    }
    //Creating A, B, C and D for Thomas algorithm
    //  These A,B, C and D are for the inner r-points
    //  They are not include r[j=0] and r[j=last point]
    a_pde_[0] = 1.0;
    b_pde_[0] = -1.0;
    c_pde_[0] = 0.0;
    d_pde_[0] = 0.0;
    N = r_mesh_size_ - 1;
    a_pde_[N] = 1.0 + (r_ratio_mesh_[N] - r_ratio_mesh_[N - 1]) /
      h_hydro_.fluid_.k_.At(t_r_mesh_[N]) / macro_heat_.omega_ic_sec_[i - 1];
    //use omega at section i-1 because this is the BC for marshing from
    //  mesh i-1 to mesh i.
    b_pde_[N] = 0.0;
    c_pde_[N] = -1.0;
    d_pde_[N] = (a_pde_[N] - 1.0) * macro_heat_.tc_mesh_[i];
    //Creating the inner terms for matrix A and b in Ax = b
    //  where A is tri-diagonal of C-A-B pde matrix, and b is
    //  D matrix.
    for (int j = 1; j < r_mesh_size_ - 1; j++) {
      dr_j = ri * (r_ratio_mesh_[j] - r_ratio_mesh_[j - 1]);
      dr_jp1 = ri * (r_ratio_mesh_[j + 1] - r_ratio_mesh_[j]);
      r_j = ri * r_ratio_mesh_[j];
      r_jm1 = ri * r_ratio_mesh_[j - 1];
      r_jp1 = ri * r_ratio_mesh_[j + 1];
      a_j = alpha_h_vec[j];
      a_jm1 = alpha_h_vec[j - 1];
      a_jp1 = alpha_h_vec[j + 1];
      e_j = epsilon_h_vec[j];
      e_jm1 = epsilon_h_vec[j - 1];
      e_jp1 = epsilon_h_vec[j + 1];
      v_ij = h_hydro_.v_at_r_mesh_[j];
      t_im1 = previous_t_r_mesh_[j];
      a_pde_[j] = v_ij / dz_i + 1.0 / r_j / (dr_jp1 + dr_j) * (
        (r_jp1 * (e_jp1 + a_jp1) + r_j * (e_j + a_j)) / dr_jp1 +
        (r_j   * (e_j + a_j) + r_jm1 * (e_jm1 + a_jm1)) / dr_j);
      b_pde_[j] = -1.0 / r_j / (dr_jp1 + dr_j) *
        (r_jp1 * (e_jp1 + a_jp1) + r_j * (e_j + a_j)) / dr_jp1;
      c_pde_[j] = -1.0 / r_j / (dr_jp1 + dr_j) *
        (r_j   * (e_j + a_j) + r_jm1 * (e_jm1 + a_jm1)) / dr_j;
      d_pde_[j] = v_ij * t_im1 / dz_i;
    }
    usr::ThomasTridiagonal(c_pde_, a_pde_, b_pde_,
      d_pde_, t_r_mesh_, r_mesh_size_);
    //whether solving_first_time_ is true or not
    //  if group_number == 0,
    //   {} is changed to the T_inlet vector {T, T, ..., T}
    //  if group_number != 0, {} stay as the first member,
    //    assignment is needed instead of push_back for
    //    i == 0 and group_number > 0, otherwise,
    //    use push_back
    //if solving_first_time_ is false,
    //  the proper T_zr_profile_list_
    //  must be created already, then use [] instead of push_back
    if (solving_first_time_) {
      if ((group_number != 0) && (i == start_z_mesh + 1 + start_adjustment)) {
        (*t_zr)[0] = t_r_mesh_;
      }
      else {
        t_zr->push_back(t_r_mesh_);
      }
    }
    else {
      if (group_number == 0) {
        (*t_zr)[i - (start_z_mesh)] = t_r_mesh_;
      }
      else {
        (*t_zr)[i - (start_z_mesh + start_adjustment)] = t_r_mesh_;
      }
    }
    
    //calculate t_bulk
    for (int j = 0; j < r_mesh_size_; j++) {
      t_x_rratio[j] = t_r_mesh_[j] * r_ratio_mesh_[j];
    }
    t_bulk = usr::Trapezoidal(r_ratio_mesh_, t_x_rratio) * 2.0;
    t_bulk_z_mesh_.push_back(t_bulk);
    previous_t_r_mesh_ = t_r_mesh_;
    /////////////////////////////////////////////////////////
    //                                                     //
    //  Concentration Profile calculation for ith z-mesh   //
    //                                                     //
    /////////////////////////////////////////////////////////
    for (int j = 0; j < r_mesh_size_; j++) {
      t_local = t_r_mesh_[j];
        //t_r_mesh_ is already updated from Thomas algorithm
      rho = h_hydro_.fluid_.rho_.At(t_local);
      mu = h_hydro_.fluid_.mu_.At(t_local);
      cp = h_hydro_.fluid_.cp_.At(t_local);
      k = h_hydro_.fluid_.k_.At(t_local);
      prandtl = mu * cp / k;
      prandtl_t = 0.85 + 0.015 / prandtl;
      alpha_h = k / rho / cp;
      alpha_m = wax_prop_.CalculateDiffusivity(t_local);
      schmidt = mu / rho / alpha_m;
      schmidt_t = 0.85 + 0.015 / schmidt;
      y_plus = (1.0 - r_ratio_mesh_[j]) * re / 2.0 *
        std::sqrt(fanning / 2.0);
      //See W. M. Deen Analysis of Transport Phenomena page 529
      //  for the formula of epsilon_eddy (eddy diffusivity)
      if (y_plus <= 80.0) {
        epsilon_over_nu = std::pow(0.4 * y_plus, 2) *
          std::pow(1.0 - std::exp(-y_plus / 26.0), 2) *
          DVPlusByDYPlus(y_plus);
      }
      else {
        epsilon_over_nu = 0.4 * y_plus / 6.0 *
          (1.0 + r_ratio_mesh_[j]) *
          (1.0 + 2.0 * std::pow(r_ratio_mesh_[j], 2));
      }
      epsilon_eddy_m = schmidt / schmidt_t * epsilon_over_nu * alpha_m;
      alpha_m_vec[j] = alpha_m;
      epsilon_m_vec[j] = epsilon_eddy_m;
    }
    //Creating A, B, C and D for Thomas algorithm
    //  These A,B, C and D are for the inner r-points
    //  They are not include r[j=0] and r[j=last point]
    a_pde_[0] = 1.0;
    b_pde_[0] = -1.0;
    c_pde_[0] = 0.0;
    d_pde_[0] = 0.0;
    N = r_mesh_size_ - 1;
    a_pde_[N] = 1.0;
    b_pde_[N] = 0.0;
    c_pde_[N] = 0.0;
    d_pde_[N] = wax_prop_.GetC(t_r_mesh_[t_r_mesh_.size()-1]);
    //Creating the inner terms for matrix A and b in Ax = b
    //  for the radial concentration profile calculation
    //  where A is tri-diagonal of C-A-B pde matrix, and b is
    //  D matrix.
    for (int j = 1; j < r_mesh_size_ - 1; j++) {
      double kr_local;
      if (sim_op_.GetKrAsAFunctionOfTemperatureOption()) {
        double dwo_T = wax_prop_.CalculateDiffusivity(t_r_mesh_[j]);
        kr_local = std::abs(kr_ * dwo_T / sim_op_.GetDwoIniForKr());
      } else{
        kr_local = std::abs(kr_);
      }
      dr_j = ri * (r_ratio_mesh_[j] - r_ratio_mesh_[j - 1]);
      dr_jp1 = ri * (r_ratio_mesh_[j + 1] - r_ratio_mesh_[j]);
      r_j = ri * r_ratio_mesh_[j];
      r_jm1 = ri * r_ratio_mesh_[j - 1];
      r_jp1 = ri * r_ratio_mesh_[j + 1];
      a_j = alpha_m_vec[j];
      a_jm1 = alpha_m_vec[j - 1];
      a_jp1 = alpha_m_vec[j + 1];
      e_j = epsilon_m_vec[j];
      e_jm1 = epsilon_m_vec[j - 1];
      e_jp1 = epsilon_m_vec[j + 1];
      v_ij = h_hydro_.v_at_r_mesh_[j];
      c_im1 = previous_c_r_mesh_[j];

      a_pde_[j] = v_ij / dz_i + 1.0 / r_j / (dr_jp1 + dr_j) * (
        (r_jp1 * (e_jp1 + a_jp1) + r_j * (e_j + a_j)) / dr_jp1 +
        (r_j   * (e_j + a_j) + r_jm1 * (e_jm1 + a_jm1)) / dr_j) + kr_local;
      b_pde_[j] = -1.0 / r_j / (dr_jp1 + dr_j) *
        (r_jp1 * (e_jp1 + a_jp1) + r_j * (e_j + a_j)) / dr_jp1;
      c_pde_[j] = -1.0 / r_j / (dr_jp1 + dr_j) *
        (r_j   * (e_j + a_j) + r_jm1 * (e_jm1 + a_jm1)) / dr_j;
      d_pde_[j] = v_ij * c_im1 / dz_i + kr_local * wax_prop_.GetC(t_r_mesh_[j]);
    }
    usr::ThomasTridiagonal(c_pde_, a_pde_, b_pde_,
      d_pde_, c_r_mesh_, r_mesh_size_);
    //Do not do max(kr(C-C(T)),0)
    //Tried and found that it causes the equation to be
    //not differentiable and make it crash.
    
    if (solving_first_time_) {
      if ((group_number != 0) && (i == start_z_mesh + 1 + start_adjustment)) {
        (*c_zr)[0] = c_r_mesh_;
      }
      else {
        c_zr->push_back(c_r_mesh_);
      }
    }
    else {
      if (group_number == 0) {
        (*c_zr)[i - (start_z_mesh)] = c_r_mesh_;
      }
      else {
        (*c_zr)[i - (start_z_mesh + start_adjustment)] = c_r_mesh_;
      }
      
    }
  }

}

int RadialCProfile::SectionToGroupNumber(int sec_number) {
  for (int i = 0; i < z_group_size_; i++) {
    if ((z_group_[i].first <= sec_number) &&
      (sec_number <= z_group_[i].second)) {
      return i;
    }
  }
  return -1;
}

void RadialCProfile::ZDepositGrouping() {
  //
  //  z_group_ indicate section index
  //  it can be repeated
  //  ex, z_group_ = (0,1), (2,2), (3,3), (4,5), (6,7), (8,8), (9,9), (10,11), (12,16)
  //  for the case where mesh index is from 0 to 17
  //  // it shows the section_index that has a similar wall shear stress
  //
  //Scanning for similar thickness
  //Similarity condition: r_av^4 / r^4 - 1 < 0.05
  double ri_av; //average ri value (ri = R - delta)
  double ri_av_old;
  double ri; //ri at any z value
  double r_inner;
  int start_index;
  std::pair<int, int> group;
  std::vector<double> delta;
  delta = macro_heat_.deposit_profile_.GetDeltaVec();
  r_inner = macro_heat_.d_ * 0.5;
  ri_av = r_inner - delta[0];
  start_index = 0;
  z_group_.clear();
  ri_group_.clear();
  for (int i = 1; i < z_sec_size_; i++) {
    ri = r_inner - delta[i];
    ri_av_old = ri_av;
    //update ri_av
    ri_av = (ri_av * (i - start_index) + ri) / (i - start_index + 1);

    bool grouping_option = false;
    if (grouping_option){
      if (std::abs(std::pow(ri_av / ri, 4) - 1.0) > 0.05) {
        //the change in deposit thickness cause more than 
        //5% variation in the wall shear stress (tau at ri)
        //report the start and end section index (they can be the same)
        group = std::make_pair(start_index, i - 1);
        z_group_.push_back(group);
        start_index = i;
        ri_av = ri;
        ri_group_.push_back(ri_av_old);
        std::cout << "debug : using grouping option\n";
      }
    }
  }
  z_group_.push_back(std::make_pair(start_index, z_sec_size_ - 1));
  ri_group_.push_back(ri_av);
  z_group_size_ = z_group_.size();

  for (int i = 0; i < z_sec_size_; i++) {
    ri_appear_sec_[i] = ri_group_[SectionToGroupNumber(i)];
  }
}
double RadialCProfile::DVPlusByDYPlus(double y_plus) {
  if (y_plus <= 5) {
    return 1.0;
  }
  else if (y_plus <= 30) {
    return 5.0 / y_plus;
  }
  else {
    return 2.5 / y_plus;
  }
  return -1;
}
std::ostream& RadialCProfile::PrintParameters(std::ostream& os,
  usr::kRadialTProfile variable) {
  switch (variable) {
  case usr::kRadialTProfile::z_group_: {
    os << "z_group_\n";
    for (auto item : z_group_) {
      os << "  " << item.first << "," << item.second << '\n';
    }
    break;
  }
  case usr::kRadialTProfile::t_zr_mesh_: {
    for (int i = 0; i < t_zr_mesh_.size(); i++) {
      for (int j = 0; j < t_zr_mesh_[i].size(); j++) {
        os << std::setprecision(7) << std::scientific
          << t_zr_mesh_[i][j];
        if (j < t_zr_mesh_[i].size() - 1) {
          os << ',';
        }
      }
      os << '\n';
    }
    break;
  }
  case usr::kRadialTProfile::c_zr_mesh_: {
    for (int i = 0; i < c_zr_mesh_.size(); i++) {
      for (int j = 0; j < c_zr_mesh_[i].size(); j++) {
        os << std::setprecision(7) << std::scientific
          << c_zr_mesh_[i][j];
        if (j < c_zr_mesh_[i].size() - 1) {
          os << ',';
        }
      }
      os << '\n';
    }
    break;
  }
  case usr::kRadialTProfile::z_mesh_: {
    for (int i = 0; i < z_mesh_.size(); i++) {
      os << z_mesh_[i] << '\n';
    }
    break;
  }
  case usr::kRadialTProfile::r_mesh_: {
    for (int i = 0; i < r_ratio_mesh_.size(); i++) {
      os << pipe_radius_ * r_ratio_mesh_[i] << '\n';
    }
    break;
  }
  case usr::kRadialTProfile::r_ratio_mesh_: {
    for (int i = 0; i < r_ratio_mesh_.size(); i++) {
      os << r_ratio_mesh_[i] << '\n';
    }
    break;
  }
  case usr::kRadialTProfile::readme_t_zr_mesh_: {
    std::time_t t = std::time(nullptr);
    os << "Local computer time\n";
    os << "day mm/dd/yy hh:mm:ss = ";
    os << std::put_time(std::localtime(&t), "%a %c") << '\n';
    os << "pipe_radius_ [m]," << pipe_radius_ << '\n';
    os << "tzr.csv gives temperature profile in the format of\n";
    os << "z\r ,   r[0],   r[1],   r[2], ...\n";
    os << "z[0], t[0,0], t[0,1], t[0,2], ...\n";
    os << "z[1], t[1,0], t[1,1], t[1,2], ...\n";
    os << "z[2], t[2,0], t[2,1], t[2,2], ...\n";
    os << " ...,    ...,    ...,    ..., ...\n";
    os << "tzr.csv contains just t[i,j]\n";
    os << "z[i] information is stored in tzr_z.csv\n";
    os << "r[j] information is stored in tzr_r.csv\n";
    break;
  }
  }
  return os;
}
void RadialCProfile::StandardOutput() {
  std::ofstream czr;
  std::ofstream tzr;
  std::ofstream tzr_z;
  std::ofstream tzr_r;
  tzr.open("tzr.csv");
  tzr_z.open("tzr_z.csv");
  tzr_r.open("tzr_r.csv");
  if (!tzr) {
    std::cout << "cannot open tzr.csv\n";
    std::system("pause");
  }
  if (!tzr_z) {
    std::cout << "cannot open tzr_z.csv\n";
    std::system("pause");
  }
  if (!tzr_r) {
    std::cout << "cannot open tzr_r.csv\n";
    std::system("pause");
  }

  czr.open("czr.csv");
  if (!czr) {
    std::cout << "cannot open czr.csv\n";
    std::system("pause");
  }


  PrintParameters(tzr, usr::kRadialTProfile::t_zr_mesh_);
  tzr.close();
  PrintParameters(czr, usr::kRadialTProfile::c_zr_mesh_);
  czr.close();
  PrintParameters(tzr_z, usr::kRadialTProfile::z_mesh_);
  tzr_z.close();
  PrintParameters(tzr_r, usr::kRadialTProfile::r_mesh_);
  tzr_r.close();
}
void RadialCProfile::PlotPColorGraph(
  std::wstring file_rcolumn_zrow,
  std::wstring file_r_column,
  std::wstring file_z_column,
  std::wstring name_title,
  std::wstring name_x_axis,
  std::wstring name_y_axis) {
  std::wstring file_name = L"pcolor.py";
  std::wstring combined = file_name + L" " +
    file_rcolumn_zrow + L" " +
    file_r_column + L" " +
    file_z_column + L" " +
    name_title + L" " +
    name_x_axis + L" " +
    name_y_axis;
  LPCWSTR param = combined.c_str();
  usr::CallPythonScript(param);
}

void RadialCProfile::CheckAxialHeat() {
  double heat_amount; //in Joule
  double heat_amount_c; //in Joule
  double cp_bar;
  double cp_bar_c;
  double delta_t;
  double delta_t_c;

  q_axial_.clear();
  q_axial_coolant_.clear();
  q_axial_difference_.clear();
  for (int i = 0; i < t_bulk_z_mesh_.size() - 1; i++) {
    //-1 to check q over each section  
    cp_bar = 0.5 * (h_hydro_.fluid_.cp_.At(t_bulk_z_mesh_[i]) +
      h_hydro_.fluid_.cp_.At(t_bulk_z_mesh_[i + 1]));
    cp_bar_c = 0.5 * (
      macro_heat_.c_fluid_.cp_.At(macro_heat_.tc_mesh_[i]) +
      macro_heat_.c_fluid_.cp_.At(macro_heat_.tc_mesh_[i + 1]));
    delta_t = t_bulk_z_mesh_[i + 1] - t_bulk_z_mesh_[i];
    delta_t_c = macro_heat_.tc_mesh_[i] - macro_heat_.tc_mesh_[i + 1];
    heat_amount = macro_heat_.wh_inlet_ * cp_bar * delta_t;
    heat_amount_c = macro_heat_.wc_inlet_ * cp_bar_c * delta_t_c;
    q_axial_.push_back(heat_amount);
    q_axial_coolant_.push_back(heat_amount_c);
    q_axial_difference_.push_back(heat_amount - heat_amount_c);
  }
}
void RadialCProfile::CheckRadialHeat() {
  double q_at_ri;
  double q_at_ri_ic;
  double delta_tav;
  double delta_ti;   //\Delta T_{i}
  double delta_tip1; //\Delta T_{i+1}
  double delta_r;
  double t;
  double k;
  double area;
  int n_r_mesh; //last index for r_mesh_ variable
                //r_mesh_[n_r_mesh] = valid number
  n_r_mesh = r_mesh_size_ - 1;
  q_radial_.clear();
  q_radial_ic_.clear();
  check_pde_boundary1_.clear();
  check_pde_boundary2_.clear();
  check_pde_boundary3_.clear();
  for (int i = 0; i < t_bulk_z_mesh_.size() - 1; i++) {
    t = 0.5 * (t_zr_mesh_[i][n_r_mesh] + t_zr_mesh_[i + 1][n_r_mesh]);
    delta_ti = t_zr_mesh_[i][n_r_mesh]
      - t_zr_mesh_[i][n_r_mesh - 1];
    delta_tip1 = t_zr_mesh_[i + 1][n_r_mesh]
      - t_zr_mesh_[i + 1][n_r_mesh - 1];
    delta_tav = 0.5 * (delta_ti + delta_tip1);
    area = 2.0 * usr::pi * ri_appear_sec_[i] *
      (z_mesh_[i + 1] - z_mesh_[i]);
    delta_r = (r_ratio_mesh_[n_r_mesh] - r_ratio_mesh_[n_r_mesh - 1]) *
      ri_appear_sec_[i];
    k = h_hydro_.fluid_.k_.At(t);
    q_at_ri = area * k * delta_tav / delta_r;
    q_radial_.push_back(q_at_ri);
    q_at_ri_ic = (t - macro_heat_.tc_mesh_[i]) /
      macro_heat_.omega_ic_sec_[i] / ri_appear_sec_[i] *
      area;
    q_radial_ic_.push_back(q_at_ri_ic);
  }
  //check if the discretized PDE is fulfilled or not
  for (int i = 1; i < t_bulk_z_mesh_.size(); i++) {
    check_pde_boundary1_.push_back(
      -1.0 * (t_zr_mesh_[i][n_r_mesh] - macro_heat_.tc_mesh_[i]) /
      h_hydro_.fluid_.k_.At(t_r_mesh_[n_r_mesh]) /
      macro_heat_.omega_ic_sec_[i - 1]);
    check_pde_boundary2_.push_back(
      (t_zr_mesh_[i][n_r_mesh] - t_zr_mesh_[i][n_r_mesh - 1]) /
      (r_ratio_mesh_[n_r_mesh] - r_ratio_mesh_[n_r_mesh - 1]));
    check_pde_boundary3_.push_back(check_pde_boundary1_[i - 1] -
      check_pde_boundary2_[i - 1]);
  }

}
void RadialCProfile::HeatBalanceVerification() {
  CheckAxialHeat();
  CheckRadialHeat();
}

void RadialCProfile::PlotDefaultCProfile() {
  PlotPColorGraph(std::wstring(L"czr.csv"),
    std::wstring(L"tzr_r.csv"),
    std::wstring(L"tzr_z.csv"),
    std::wstring(L"C[kg/m3]"),
    std::wstring(L"r[m]"),
    std::wstring(L"z[m]"));
}
void RadialCProfile::PlotDefaultTProfile() {
  PlotPColorGraph();
}

} // namespace mass_calculation
} // namespace spdepo