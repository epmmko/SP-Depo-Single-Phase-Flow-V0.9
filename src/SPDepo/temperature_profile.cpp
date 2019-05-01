#pragma warning(disable : 4996)
//std::put_time
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
#include "user_flow_enum.h"
#include "user_numerical_method.h"
#include "user_python_script.h"



namespace spdepo{
namespace heat_calculation {
  MacroZHeatBalance* ptr_wrap;
  double WrapNuTurbulentSlope(double x, double y) {
    return ptr_wrap->NuTurbulentSlope(x);
  }
  double WrapTZProfileLoop(double x) {
    return ptr_wrap->TZProfileLoop(x, ptr_wrap->deposit_profile_);
  }
  MacroZHeatBalance::MacroZHeatBalance() {
  }

  void MacroZHeatBalance::SetAll(const SinglePhaseHydrodynamics& h_hydro,
      const CoolantCondition& c_inlet, const Fluid& c_fluid,
      const Solid& pipe_prop) {
    h_hydro.GetInletCondition(&h_inlet_);
    h_hydro.GetFluid(&h_fluid_);
    h_hydro.GetPipeGeometry(&pipe_geometry_);
    h_hydro_ = h_hydro;
    c_inlet_ = c_inlet;
    c_fluid_ = c_fluid;
    pipe_prop_ = pipe_prop;
    rho_h_eq_ = h_fluid_.GetEquation(usr::kFluid::rho);
    mu_h_eq_ = h_fluid_.GetEquation(usr::kFluid::mu);
    k_h_eq_ = h_fluid_.GetEquation(usr::kFluid::k);
    cp_h_eq_ = h_fluid_.GetEquation(usr::kFluid::cp);
    rho_c_eq_ = c_fluid_.GetEquation(usr::kFluid::rho);
    mu_c_eq_ = c_fluid_.GetEquation(usr::kFluid::mu);
    k_c_eq_ = c_fluid_.GetEquation(usr::kFluid::k);
    cp_c_eq_ = c_fluid_.GetEquation(usr::kFluid::cp);

    d_ = pipe_geometry_.GetProperties(usr::kGeometry::d);
    d_o_ = pipe_geometry_.GetProperties(usr::kGeometry::d_o);
    a_p_ = pipe_geometry_.GetProperties(usr::kGeometry::a_p);
    a_j_ = pipe_geometry_.GetProperties(usr::kGeometry::a_j);
    rhw_ = d_ / 2.0;
    rcw_ = d_o_ / 2.0;
    d_jhy_ = pipe_geometry_.GetProperties(usr::kGeometry::d_jhy);
    beta_j_ = pipe_geometry_.GetProperties(usr::kGeometry::d_o) / 
              pipe_geometry_.GetProperties(usr::kGeometry::d_j);
    eps_j_ = pipe_geometry_.GetProperties(usr::kGeometry::eps_j);
    eps_p_ = pipe_geometry_.GetProperties(usr::kGeometry::eps_p);
    l_ht_ = pipe_geometry_.GetProperties(
      usr::kGeometry::l_heat_transfer);
    re_inlet_ = h_hydro_.GetProperties(usr::kSinglePhaseFlow::re);
    v_h_inlet_ = h_hydro_.GetProperties(usr::kSinglePhaseFlow::v_inlet);
    v_c_inlet_ = c_inlet_.GetDouble(usr::kCoolant::volume_flow_rate) /
                 pipe_geometry_.GetProperties(usr::kGeometry::a_j);
    counter_current_ = c_inlet_.GetBool(
      usr::kCoolant::flow_counter_currently);
    t_h_inlet_ = h_inlet_.GetProperties(
      usr::kTwoPhaseVar::inlet_temperature);
    t_c_inlet_ = c_inlet_.GetDouble(usr::kCoolant::inlet_temperature);

    rho_h_inlet_ = rho_h_eq_.At(t_h_inlet_);
    rho_c_inlet_ = rho_c_eq_.At(t_c_inlet_);
    mu_h_inlet_ = mu_h_eq_.At(t_h_inlet_);
    mu_c_inlet_ = mu_c_eq_.At(t_c_inlet_);

    wh_inlet_ = rho_h_inlet_ * v_h_inlet_ * a_p_;

    if (counter_current_) {
      wc_inlet_ = -1.0 * rho_c_inlet_ * v_c_inlet_ * a_j_;
    }
    else {
      wc_inlet_ = rho_c_inlet_ * v_c_inlet_ * a_j_;
    }
    
  }

  void MacroZHeatBalance::Solve(const DepositProfile& dep) {
    double T_c_outlet;
    //3 parameters below are for Newton method 
      //of the counter current case
    bool success;
    int number_of_iteration;
    double fx_final;

    deposit_profile_ = dep;
    if(!called_zDiscretization_){
      std::cout << "ZDiscretization() must be called first\n";
      std::system("pause");
      return;
    }
    
    if (!th_mesh_.empty()) {
      th_mesh_.clear();
      tc_mesh_.clear();
    }
    //specify size of vectors
    cp_h_sec_.assign(sec_size_, 0);
    cp_c_sec_.assign(sec_size_, 0);
    hc_sec_.assign(sec_size_, 0);
    hh_sec_.assign(sec_size_, 0);
    nu_h_sec_.assign(sec_size_, 0);
    nu_c_sec_.assign(sec_size_, 0);
    omega_h_sec_.assign(sec_size_, 0);
    omega_c_sec_.assign(sec_size_, 0);
    omega_ic_sec_.assign(sec_size_, 0);
    omega_pipe_sec_.assign(sec_size_, 0);
    omega_dep_sec_.assign(sec_size_, 0);
    omega_total_sec_.assign(sec_size_, 0);
    absQc_radial_sec_.assign(sec_size_, 0);
    //Initially, assume T != T(z)
    tc_mesh_.assign(mesh_size_, t_c_inlet_);
    tc_sec_.assign(sec_size_, t_c_inlet_);
    tc_sec_chk_.assign(sec_size_, t_c_inlet_);
    tcw_sec_.assign(sec_size_, t_c_inlet_);
    tdep_sec_.assign(sec_size_, t_h_inlet_);
    th_mesh_.assign(mesh_size_, t_h_inlet_);
    th_sec_.assign(sec_size_, t_h_inlet_);
    thw_sec_.assign(sec_size_, t_c_inlet_);
    tri_sec_.assign(sec_size_, t_h_inlet_);

    ptr_wrap = this;
    T_c_outlet = usr::NewtonMethod(WrapTZProfileLoop,t_c_inlet_,
                   success,  number_of_iteration, fx_final, 4, 1.0e-8);
    MeshToSec(th_mesh_, &th_sec_, sec_size_);
    MeshToSec(tc_mesh_, &tc_sec_, sec_size_);
    CalculateSectionT(dep);
  }
  void MacroZHeatBalance::CalculateSectionT(const DepositProfile& dep) {
    std::vector<double> ri = dep.GetRiVec();
    for (int i = 0; i < sec_size_; i++) {
      absQc_radial_sec_[i] = std::abs(2.0 * usr::pi * (z_mesh_[i + 1]
        - z_mesh_[i]) / omega_total_sec_[i] * ((th_mesh_[i + 1]
        - tc_mesh_[i + 1]) - (th_mesh_[i] - tc_mesh_[i]))
        / std::log((th_mesh_[i + 1] - tc_mesh_[i + 1]) / (th_mesh_[i]
        - tc_mesh_[i])));
      tri_sec_[i] = th_sec_[i] - absQc_radial_sec_[i] / 2.0 / usr::pi /
        (z_mesh_[i+1] - z_mesh_[i]) * omega_h_sec_[i];
      thw_sec_[i] = th_sec_[i] - absQc_radial_sec_[i] / 2.0 / usr::pi /
        (z_mesh_[i + 1] - z_mesh_[i]) * (omega_h_sec_[i] + omega_dep_sec_[i]);
      tcw_sec_[i] = th_sec_[i] - absQc_radial_sec_[i] / 2.0 / usr::pi /
        (z_mesh_[i + 1] - z_mesh_[i]) * (omega_h_sec_[i] + omega_dep_sec_[i]
        + omega_pipe_sec_[i]);
      tc_sec_chk_[i] = th_sec_[i] - absQc_radial_sec_[i] / 2.0 / usr::pi /
        (z_mesh_[i + 1] - z_mesh_[i]) * omega_total_sec_[i];
      if (std::abs(rhw_ / ri[i] - 1.0) < 1.0e-14) {
        tdep_sec_[i] = tri_sec_[i];
      }
      else {
        tdep_sec_[i] = tri_sec_[i] - (tri_sec_[i] - thw_sec_[i]) * (
          rhw_ * rhw_ / (rhw_ * rhw_ - ri[i] * ri[i]) -
          0.5 / std::log(rhw_ / ri[i]));
      }


    }
  }

  std::ostream& MacroZHeatBalance::PrintTSectionProfile(std::ostream& os) const {
    os << std::setprecision(15);
    os << "z_sec_[m],Th[C],Ti[C],Thw[C],Tcw[C],Tc[C],Tc_chk_[C]";
    os << ",deposit_delta[m],deposit_Fw[w/w]\n";
    std::vector<double> delta;
    std::vector<double> fw;
    delta = deposit_profile_.GetDeltaVec();
    fw = deposit_profile_.GetFwVec();
    for (int i = 0; i < sec_size_; i++) {
      os << z_sec_[i] << ',' << th_sec_[i] << ',' << tri_sec_[i] << ','
        << thw_sec_[i] << ',' << tcw_sec_[i] << ',' << tc_sec_[i] << ','
        << tc_sec_chk_[i] << ',' << delta[i] << ','
        << fw[i] << '\n';
    }
    return os;
  }

  double MacroZHeatBalance::TZProfileLoop(double Tc, DepositProfile dep) {
    double abs_err_av;
    double tc_output;
    int i, j;
    std::vector<double> th_mesh_old;
    std::vector<double> tc_mesh_old;

    th_mesh_old.assign(mesh_size_, 0.0);
    tc_mesh_old.assign(mesh_size_, 0.0);
    for(i = 0; i < 100; i++){
      tc_output = ForwardTZProfile(Tc, dep);
      abs_err_av = 0.0;
      for (j = 0; j < mesh_size_; j++) {
        abs_err_av = abs_err_av + std::abs(th_mesh_[j] - th_mesh_old[j]);
        abs_err_av = abs_err_av + std::abs(tc_mesh_[j] - tc_mesh_old[j]);
      }
      abs_err_av = abs_err_av / (2.0 * mesh_size_);
      if (abs_err_av < 1.0e-6) {
        break;
      }
      th_mesh_old = th_mesh_;
      tc_mesh_old = tc_mesh_;
    }
    return tc_output - t_c_inlet_;
  }

  void MacroZHeatBalance::HeatBalanceVerification() {
    std::vector<double> absQh_axial_sec(sec_size_,0.0);
    std::vector<double> absQc_axial_sec(sec_size_, 0.0);

    av_absolute_err_heat_cal_ = 0.0;
    av_relative_err_heat_cal_ = 0.0;

    for (int i = 0; i < sec_size_; i++) {
      absQh_axial_sec[i] =std::abs( wh_inlet_ * cp_h_sec_[i]  *
                        (th_mesh_[i + 1] - th_mesh_[i]));
      absQc_axial_sec[i] = std::abs(wc_inlet_ * cp_c_sec_[i]  *
                        (tc_mesh_[i + 1] - tc_mesh_[i]));
      absQc_radial_sec_[i] = std::abs(2.0 * usr::pi * (z_mesh_[i+1] 
        - z_mesh_[i]) / omega_total_sec_[i] * ((th_mesh_[i+1] 
        - tc_mesh_[i+1]) - (th_mesh_[i] - tc_mesh_[i])) 
        / std::log((th_mesh_[i+1] - tc_mesh_[i+1])/(th_mesh_[i] 
        - tc_mesh_[i])));
      av_absolute_err_heat_cal_ = std::abs(absQh_axial_sec[i] + 
        absQc_axial_sec[i] - 2.0 * absQc_radial_sec_[i]);
      av_relative_err_heat_cal_ = std::abs(absQh_axial_sec[i] + 
        absQc_axial_sec[i] - 2.0 * absQc_radial_sec_[i]) 
        / std::abs(absQc_radial_sec_[i]);
    }
    av_absolute_err_heat_cal_ = av_absolute_err_heat_cal_ /
      sec_size_ / 2.0;
    av_relative_err_heat_cal_ = av_relative_err_heat_cal_ /
      sec_size_ / 2.0;
    if (std::fmax(av_absolute_err_heat_cal_, av_relative_err_heat_cal_) >
      1.0e-3) {
      std::cout << "error occur in the macroscopic";
      std::cout << " heat balance calculation\n";
      std::cout << "average relative error of heat balance = ";
      std::cout << av_relative_err_heat_cal_ << '\n';
      std::cout << "average absolute error of heat balance = ";
      std::cout << av_absolute_err_heat_cal_ << '\n';
      std::cout << "axial enthalpy loss is not equal to the";
      std::cout << " radial heat loss\n";
      std::system("pause");
    }
  }

  double MacroZHeatBalance::ForwardTZProfile(double Tc, 
      DepositProfile dep) {
    //take the guess Tc, return the calculated coolant inlet T 
    //for the case of counter currrent flow
    double AA_sec_i; //exponent component for T calculation
    double BB_sec_i; //ratio of mass flow rate * heat capacity
    double k_pipe_i;
    double k_dep_i;
    double k_h_sec_i;
    double k_c_sec_i;
    double ri_sec_i;
    double Tb_sec_i;
    double Tc1;
    double Tc2;
    double Tc_sec_i;
    double Tcw_sec_i;
    double Thw_sec_i;
    double Th1;
    double Th2;
    double Tpipe_sec_i;
    double Tdep_sec_i;
    double Tri_sec_i;
    double z_i;
    double z_next_i;
    double zc_i;
    double zc_next_i;
    std::vector<double> delta_vec;
    //adjust the outlet (for counter current case)
    // based on the input Tc
    tc_mesh_[0] = Tc;
    //Loop through each section (not mesh)
    delta_vec = dep.GetDeltaVec();
    for (int i = 0; i < sec_size_; i++) {
      Th1 = th_mesh_[i];
      Tc1 = tc_mesh_[i];
      Tb_sec_i = th_sec_[i];
      Tc_sec_i = tc_sec_[i];
      Thw_sec_i = thw_sec_[i];
      Tcw_sec_i = tcw_sec_[i];
      Tri_sec_i = tri_sec_[i];
      Tpipe_sec_i = Thw_sec_i - (Thw_sec_i - Tcw_sec_i) * (
        rcw_ * rcw_ / (rcw_ * rcw_ - rhw_ * rhw_) - 
        0.5 / std::log(rcw_ / rhw_));
      ri_sec_i = rhw_ - delta_vec[i];
      if (std::abs(rhw_ / ri_sec_i - 1.0) < 1.0e-14) {
        Tdep_sec_i = Tri_sec_i;
        k_dep_i = 1.0e10;
        omega_dep_sec_[i] = 0.0;
      }
      else {
        Tdep_sec_i = Tri_sec_i - (Tri_sec_i - Thw_sec_i) * (
          rhw_ * rhw_ / (rhw_ * rhw_ - ri_sec_i * ri_sec_i) -
          0.5 / std::log(rhw_ / ri_sec_i));
        k_dep_i = dep.GetKDeposit(i, Tdep_sec_i);
        omega_dep_sec_[i] = std::log(rhw_ / ri_sec_i) / k_dep_i;
      }
      k_pipe_i = pipe_prop_.GetProperty(usr::kSolid::k, Tpipe_sec_i);
      k_h_sec_i = k_h_eq_.At(Tb_sec_i);
      k_c_sec_i = k_c_eq_.At(Tc_sec_i);
      z_i = z_mesh_[i];
      z_next_i = z_mesh_[i+1];
      nu_h_sec_[i] = TurbulentPipeAverageNusselt(
        Tb_sec_i, Thw_sec_i, z_i, z_next_i, delta_vec[i],
        dep.overall_roughness_);
      zc_i = l_ht_ - z_i;
      zc_next_i = std::fmax(l_ht_ - z_next_i,0.0);
      //Use fmax, because roundoff error can cause "double - double"
      //  to be some small negative value, like -1.0e-16, instead of 0,
      //  and cause the negative value in pow(x, 2.0/3.0).

      nu_c_sec_[i] = TurbulentAnnulusAverageNusselt(
        Tc_sec_i, Tcw_sec_i, zc_next_i, zc_i);
        //next_i is referenced to the hot side index
        // Therefore, zc_i is zb, and zc_next_i is za
        //   in the cool side coordinate.      
      hh_sec_[i] = nu_h_sec_[i] * k_h_sec_i / 2.0 / ri_sec_i;
      hc_sec_[i] = nu_c_sec_[i] * k_c_sec_i / d_jhy_;
      omega_pipe_sec_[i] = std::log(rcw_/rhw_) / k_pipe_i;

      omega_c_sec_[i] = 1.0 / rcw_ / hc_sec_[i];
      omega_h_sec_[i] = 1.0 / ri_sec_i / hh_sec_[i];
      omega_ic_sec_[i] = omega_dep_sec_[i] + omega_pipe_sec_[i] + omega_c_sec_[i];
      omega_total_sec_[i] = omega_h_sec_[i] + omega_c_sec_[i] + omega_dep_sec_[i]
                      + omega_pipe_sec_[i];
      cp_c_sec_[i] = cp_c_eq_.At(Tc_sec_i);
      cp_h_sec_[i] = cp_h_eq_.At(Tb_sec_i);
      BB_sec_i = -1.0 * wc_inlet_ * cp_c_sec_[i] / wh_inlet_ / cp_h_sec_[i];

      if (std::abs(BB_sec_i - 1.0) < 1.0e-14) {
        //no need for A value
        Th2 = Th1 + 2.0 * usr::pi / omega_total_sec_[i] / wh_inlet_ / cp_h_sec_[i]
          * (Tc1 - Th1) * (z_next_i - z_i);
        Tc2 = Tc1 + Th2 - Th1;
      }
      else {
        //calculate A value
        AA_sec_i = std::exp(-2.0 * usr::pi / omega_total_sec_[i] *
          (z_next_i - z_i) * (1.0 / wc_inlet_ / cp_c_sec_[i] +
          1.0 / wh_inlet_ / cp_h_sec_[i]));
        Tc2 = ((1.0 - AA_sec_i) * Th1 + (AA_sec_i - BB_sec_i) * Tc1) /
              (1.0 - BB_sec_i);
        Th2 = Tc2 + AA_sec_i * (Th1 - Tc1);
      }
      th_mesh_[i+1] = Th2;
      tc_mesh_[i+1] = Tc2;
    }
    
  //  CalculateAnnulusNusseltNumber(T);
    return tc_mesh_[mesh_size_ - 1];
  }
    
  double MacroZHeatBalance::TurbulentPipeAverageNusselt(double Tb,
      double Tw, double za, double zb, double delta, 
      double deposit_roughness) {
      //give average Nu from za to zb (not point Nu)
      //Assumption, Nu_infty_ab can be estimated based on the
      //  average fluid properties and deposit thickness between a and b
    double pr, pr_w; //Prandtl number
    double rho, cp, k, mu, f;
    double re;
    double cp_w, k_w, mu_w;
    double nu_infty; // Nusselt number far away fron the inlet
    double v;
    double ri;

    rho = rho_h_eq_.At(Tb);
    cp = cp_h_eq_.At(Tb);
    k = k_h_eq_.At(Tb);
    mu = mu_h_eq_.At(Tb);
    pr = mu * cp / k;    

    cp_w = cp_h_eq_.At(Tw);
    k_w = k_h_eq_.At(Tw);
    mu_w = mu_h_eq_.At(Tw);
    pr_w = mu_w * cp_w / k_w;
    
    ri = d_ / 2.0 - delta;

    v = v_h_inlet_ * rho_h_inlet_ / rho * d_ * d_ /
        (2.0 * ri) / (2.0 * ri);
    re = rho * v * 2.0 * ri / mu;

    f = h_hydro_.HaalandEquation(deposit_roughness, 2.0 * ri, re);
    if (re > 2100) {
      nu_infty = f/2 * ( re - 1000.0) * pr / 
        (1.0 + 12.7 * std::sqrt(f / 2.0) * (std::pow(pr, 2.0/3.0) - 1.0)) *
        std::pow(pr/pr_w, 0.11);
    }
    return nu_infty * (1.0 + 3.0 * std::pow(d_ , 2.0/3.0) * 
      (std::pow(zb,1.0/3.0) - std::pow(za,1.0/3.0)) / (zb - za));
  }

  double MacroZHeatBalance::TurbulentAnnulusAverageNusselt(
      double Tb, double Tw,
      double za, double zb) {
    double pr, pr_w; //Prandtl number
    double rho, cp, k, mu, f_ann;
    double re, re_star;
    double cp_w, k_w, mu_w;
    double nu_infty; // Nusselt number far away fron the inlet
    double v;
    double coeff_k1, coeff_f_ann, coeff_capital_k;
    // fluid calculation //
    rho = rho_c_eq_.At(Tb);
    cp = cp_c_eq_.At(Tb);
    k = k_c_eq_.At(Tb);
    mu = mu_c_eq_.At(Tb);
    pr = mu * cp / k;

    cp_w = cp_c_eq_.At(Tw);
    k_w = k_c_eq_.At(Tw);
    mu_w = mu_c_eq_.At(Tw);
    pr_w = mu_w * cp_w / k_w;
    // end fluid calculation //
    v = v_c_inlet_ * rho_c_inlet_ / rho;
    re = rho * v * d_jhy_ / mu;
    re_star = re * ((1.0 + beta_j_ * beta_j_) + (1.0 - beta_j_ * beta_j_)
              / std::log(beta_j_)) / ( 1.0 - beta_j_) / (1.0 - beta_j_);
    f_ann = h_hydro_.HaalandEquation(eps_j_, d_jhy_, re_star);
    coeff_k1 = 1.07 + 900.0 / re - 0.63 / (1.0 + 10.0 * pr);
    coeff_f_ann = 0.75 * std::pow(beta_j_, -0.17);
    coeff_capital_k = std::pow(pr/pr_w, 0.11);
    nu_infty = f_ann / 2.0 * re * pr * coeff_f_ann * coeff_capital_k /
      (coeff_k1 + 12.7 * std::sqrt(f_ann/2.0) * (std::pow(pr,2.0/3.0) - 1.0));
    return nu_infty * (1.0 + 3.0 * std::pow(d_jhy_, 2.0 / 3.0) *
      (std::pow(zb, 1.0 / 3.0) - std::pow(za, 1.0 / 3.0)) / (zb - za));
  }

  void MacroZHeatBalance::ZDiscretization() {
    double l_d_ratio; //ratio of heat_transfer_length over ID [-]
    
    long num_fn_call;

    std::vector<double> y_temp;
    
    l_d_ratio = l_ht_ / d_;
    if (re_inlet_ > 2100){
      usr::DormandPrince1D(WrapNuTurbulentSlope, &num_fn_call,
        &z_mesh_, &y_temp, 1, l_d_ratio, 1, -l_d_ratio/10.0,
        1.0e-7, 30, 100, 4.0, l_d_ratio/10.0, 1, false, true);
    }
    else {
      std::cout << "did not make the laminar flow discretization yet\n";
      std::system("pause");
    }
    if (z_mesh_.back() > 1) {
      z_mesh_.push_back(1.0);
    }
    z_mesh_.push_back(0.0);
    
    mesh_size_ = z_mesh_.size();
    sec_size_  = mesh_size_ - 1;

    //change from vector of z/d [-] to z[m]
    for (int i = 0; i < mesh_size_; i++) {
      z_mesh_[i] = z_mesh_[i] * d_;
    }

    for (int i = 0; i < mesh_size_ / 2; i++) {
      std::swap(z_mesh_[i],z_mesh_[mesh_size_ - 1 - i]);
    }
    MeshToSec(z_mesh_, &z_sec_, sec_size_);
    called_zDiscretization_ = true;
  }

  void MacroZHeatBalance::MeshToSec(const std::vector<double>& input_mesh,
      std::vector<double>* output_sec, int n_sec) {
    double temp;
    std::vector<double> vtemp;
    if (!(*output_sec).empty()) {
      for (int i = 0; i < n_sec; i++) {
        (*output_sec)[i] = (input_mesh[i] + input_mesh[i + 1]) / 2.0;
      }
    }
    else {
      for (int i = 0; i < n_sec; i++) {
        (*output_sec).push_back((input_mesh[i] + input_mesh[i + 1]) / 2.0);
      }
    }
  }

  double MacroZHeatBalance::NuTurbulentSlope(double zr) {
    return -2.0 / 3.0 * std::pow(zr, -5.0/3.0);
  }

  void MacroZHeatBalance::GetProfile(usr::kProfile var_name,
                                std::vector<double>* output) const{
    switch (var_name) {
      case usr::kProfile::z_mesh: 
        *output = z_mesh_;
        break;
    }
  }

  int MacroZHeatBalance::GetMeshSize() {
    return mesh_size_;
  }
  int MacroZHeatBalance::GetSecSize() {
    return sec_size_;
  }

  void RadialTProfile::SetAll(const MacroZHeatBalance& macro_heat,
      const SinglePhaseHydrodynamics& h_hydro) {
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

    deposit_roughness_ = macro_heat_.deposit_profile_.overall_roughness_;
    pipe_radius_ = macro_heat_.d_ * 0.5;
  }
  void RadialTProfile::Solve() {
    double start_z_mesh;
    t_zr_mesh_.clear();
    for (int j = 0; j < 2; j++){ //make T_pde iterate twice
      //This is because the first iteration, use T r-profile at i-1
      //for the fluid properties at i
      //The second iteration use T r-profile at i
      //(from the previous iteration) for the fluid properties at i
      t_bulk_z_mesh_.clear();
      for (int i = 0; i < z_group_size_; i++) {
        //Use the previus-z the T_profile for fluid properties calculation
        //  in T_profile (itself) calculation. Then, iterate it.
        //  until the T_profile input = output.
        start_z_mesh = z_group_[i].first;
        if (solving_first_time_) {
          t_zr_profile_list_.push_back(GroupTProfileCalculation(i));
        }
        else {
          t_zr_profile_list_[i] = GroupTProfileCalculation(i);
        }
        
        if(!solving_first_time_){
          t_zr_mesh_.clear();
          for (int j = 0; j < t_zr_profile_list_[i].size(); j++){
            t_zr_mesh_.push_back(t_zr_profile_list_[i][j]);
          }
        }
      }
      solving_first_time_ = false;
    }

  }

  usr::TwoDVector<double> RadialTProfile::GroupTProfileCalculation(
      int group_number) {
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
    double cp;
    double dr_j;
    double dr_jp1;
    double epsilon_over_nu;
    double epsilon_eddy_h;
    double k;
    double mu;
    double prandtl;
    double prandtl_t;
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
    double v_ij;  //V_{i,j}
    double dz_i;  //\Delta z_i

    std::vector<double> alpha_h_vec(r_mesh_size_,0.0);
    std::vector<double> epsilon_h_vec(r_mesh_size_, 0.0);
    std::vector<double> fw_vec;
    std::vector<double> t_x_rratio(r_mesh_size_,0.0);
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
    z_vec = std::vector<double>(z_mesh_.begin()+ start_z_mesh,
      z_mesh_.begin() + end_z_mesh + 1);
    delta = pipe_radius_ - ri;
    if (group_number == 0) {
      if(solving_first_time_){
        t_r_mesh_.assign(r_mesh_size_, macro_heat_.th_mesh_[0]);
      }
      else {
        t_r_mesh_ = t_zr_profile_list_[0][1];
      }
      previous_t_r_mesh_.assign(r_mesh_size_, macro_heat_.th_mesh_[0]);
      output.push_back(previous_t_r_mesh_);
      //calculate t_bulk
      for (int j = 0; j < r_mesh_size_; j++) {
        t_x_rratio[j] = t_r_mesh_[j] * r_ratio_mesh_[j];
      }
      t_bulk = usr::Trapezoidal(r_ratio_mesh_, t_x_rratio) * 2.0;
      t_bulk_z_mesh_.push_back(t_bulk);
    }
    else {
      //otherwise, use the last T_{r,mesh} of the previous group
      previous_end_z_mesh = t_zr_profile_list_[group_number -1].size() - 1;
      if (solving_first_time_) {
        t_r_mesh_ = 
          t_zr_profile_list_[group_number -1][previous_end_z_mesh];
      }
      else {
        t_r_mesh_ = t_zr_profile_list_[group_number][1];
      }
      previous_t_r_mesh_ = 
        t_zr_profile_list_[group_number - 1][previous_end_z_mesh];
    }
    for (int i = start_z_mesh + 1; i <= end_z_mesh; i++) {
      dz_i = z_mesh_[i] - z_mesh_[i - 1];
      //Calculate flow average temperature
      //If this is not the first iteration,
      //  friction factor, v* and fluid properties are calculated
      //  based on the t_r_mesh_ of the previous iteration
      //  otherwise it is based on the t_r_mesh_ of the previous z
      for (int j = 0; j < r_mesh_size_; j++) {
        t_x_rratio[j] = t_r_mesh_[j] * r_ratio_mesh_[j];
      }
      t_bulk = usr::Trapezoidal(r_ratio_mesh_,t_x_rratio) * 2.0;
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
            std::pow(1.0 - std::exp(-y_plus / 26.0),2) * 
            DVPlusByDYPlus(y_plus);
        }
        else {
          epsilon_over_nu = 0.4 * y_plus / 6.0 *
            (1.0 + r_ratio_mesh_[j]) * 
            (1.0 + 2.0 * std::pow(r_ratio_mesh_[j],2));
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
        h_hydro_.fluid_.k_.At(t_r_mesh_[N]) / macro_heat_.omega_ic_sec_[i-1];
        //use omega at section i-1 because this is the BC for marshing from
        //  mesh i-1 to mesh i.
      b_pde_[N] = 0.0;
      c_pde_[N] = -1.0;
      d_pde_[N] = (a_pde_[N] - 1.0) * macro_heat_.tc_mesh_[i];
      //Creating the inner terms for matrix A and b in Ax = b
      //  where A is tri-diagonal of C-A-B pde matrix, and b is
      //  D matrix.
      for (int j = 1; j < r_mesh_size_ - 1; j++) {
        dr_j = ri * (r_ratio_mesh_[j] - r_ratio_mesh_[j-1]);
        dr_jp1 = ri * (r_ratio_mesh_[j+1] - r_ratio_mesh_[j]);
        r_j = ri * r_ratio_mesh_[j];
        r_jm1 = ri * r_ratio_mesh_[j - 1];
        r_jp1 = ri * r_ratio_mesh_[j+1];
        a_j = alpha_h_vec[j];
        a_jm1 = alpha_h_vec[j-1];
        a_jp1 = alpha_h_vec[j+1];
        e_j = epsilon_h_vec[j];
        e_jm1 = epsilon_h_vec[j-1];
        e_jp1 = epsilon_h_vec[j+1];
        v_ij = h_hydro_.v_at_r_mesh_[j];
        t_im1 = previous_t_r_mesh_[j];
        a_pde_[j] = v_ij / dz_i + 1.0 / r_j / (dr_jp1 + dr_j) * (
          (r_jp1 * (e_jp1 + a_jp1) + r_j   * (e_j   + a_j  )) / dr_jp1 +
          (r_j   * (e_j   + a_j  ) + r_jm1 * (e_jm1 + a_jm1)) / dr_j);
        b_pde_[j] = -1.0 / r_j / (dr_jp1 + dr_j) *
          (r_jp1 * (e_jp1 + a_jp1) + r_j   * (e_j   + a_j  )) / dr_jp1;
        c_pde_[j] = -1.0 / r_j / (dr_jp1 + dr_j) *
          (r_j   * (e_j   + a_j  ) + r_jm1 * (e_jm1 + a_jm1)) / dr_j;
        d_pde_[j] = v_ij * t_im1 / dz_i;
      }
      usr::ThomasTridiagonal(c_pde_,a_pde_,b_pde_,
        d_pde_,t_r_mesh_, r_mesh_size_);
      output.push_back(t_r_mesh_);
      //calculate t_bulk
      for (int j = 0; j < r_mesh_size_; j++) {
        t_x_rratio[j] = t_r_mesh_[j] * r_ratio_mesh_[j];
      }
      t_bulk = usr::Trapezoidal(r_ratio_mesh_, t_x_rratio) * 2.0;
      t_bulk_z_mesh_.push_back(t_bulk);
      previous_t_r_mesh_ = t_r_mesh_;
    }
    return output;
  }

  int RadialTProfile::SectionToGroupNumber(int sec_number) {
    for (int i = 0; i < z_group_size_; i++) {
      if ((z_group_[i].first <= sec_number) && 
           (sec_number <= z_group_[i].second)) {
         return i;
      }
    }
    return -1;
  }

  void RadialTProfile::ZDepositGrouping() {
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
      if (std::abs(std::pow(ri_av / ri, 4) - 1.0) > 0.05) {
        //the change in deposit thickness cause more than 
          //5% variation in the wall shear stress (tau at ri)
          //report the start and end section index (they can be the same)
          group = std::make_pair(start_index, i - 1);
          z_group_.push_back(group);
          start_index = i;
          ri_av = ri;
          ri_group_.push_back(ri_av_old);
      }
    }
    z_group_.push_back(std::make_pair(start_index, z_sec_size_ - 1));
    ri_group_.push_back(ri_av);
    z_group_size_ = z_group_.size();
    
    for (int i = 0; i < z_sec_size_; i++) {
      ri_appear_sec_[i] = ri_group_[SectionToGroupNumber(i)];
    }
  }
  double RadialTProfile::DVPlusByDYPlus(double y_plus) {
    if (y_plus <= 5) {
      return 1.0;
    } 
    else if(y_plus <= 30){
      return 5.0 / y_plus;
    }
    else {
      return 2.5 / y_plus;
    }
    return -1;
  }
  std::ostream& RadialTProfile::PrintParameters(std::ostream& os,
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
  void RadialTProfile::StandardOutput() {
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
    PrintParameters(tzr,usr::kRadialTProfile::t_zr_mesh_);
    tzr.close();
    PrintParameters(tzr_z, usr::kRadialTProfile::z_mesh_);
    tzr_z.close();
    PrintParameters(tzr_r, usr::kRadialTProfile::r_mesh_);
    tzr_r.close();
  }
  void RadialTProfile::PlotPColorGraph(
      std::wstring file_rcolumn_zrow,
      std::wstring file_r_column,
      std::wstring file_z_column,
      std::wstring name_title,
      std::wstring name_x_axis,
      std::wstring name_y_axis){
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
  
  void RadialTProfile::CheckAxialHeat() {
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
                      h_hydro_.fluid_.cp_.At(t_bulk_z_mesh_[i+1]));
      cp_bar_c = 0.5 * (
        macro_heat_.c_fluid_.cp_.At(macro_heat_.tc_mesh_[i]) +
        macro_heat_.c_fluid_.cp_.At(macro_heat_.tc_mesh_[i+1]));
      delta_t = t_bulk_z_mesh_[i+1] - t_bulk_z_mesh_[i];
      delta_t_c = macro_heat_.tc_mesh_[i] - macro_heat_.tc_mesh_[i+1];
      heat_amount = macro_heat_.wh_inlet_ * cp_bar * delta_t;
      heat_amount_c = macro_heat_.wc_inlet_ * cp_bar_c * delta_t_c;
      q_axial_.push_back(heat_amount);
      q_axial_coolant_.push_back(heat_amount_c);
      q_axial_difference_.push_back(heat_amount - heat_amount_c);
    }
  }
  void RadialTProfile::CheckRadialHeat() {
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
      delta_ti =   t_zr_mesh_[i][n_r_mesh] 
                 - t_zr_mesh_[i][n_r_mesh - 1];
      delta_tip1 = t_zr_mesh_[i+1][n_r_mesh]
                 - t_zr_mesh_[i+1][n_r_mesh - 1];
      delta_tav = 0.5 * (delta_ti + delta_tip1);
      area = 2.0 * usr::pi * ri_appear_sec_[i] * 
             (z_mesh_[i+1] - z_mesh_[i]);
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
      check_pde_boundary3_.push_back(check_pde_boundary1_[i-1] - 
         check_pde_boundary2_[i-1]);
    }
    
  }
  void RadialTProfile::HeatBalanceVerification() {
    CheckAxialHeat();
    CheckRadialHeat();
  }
} // namespace heat_calculation
} // namespace spdepo