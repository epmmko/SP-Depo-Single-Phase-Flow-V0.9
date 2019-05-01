#include "velocity_profile.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <iomanip>
#include "user_numerical_method.h"
#include "user_short_name.h"
#include "user_flow_enum.h"

namespace spdepo{
SinglePhaseHydrodynamics* ptr_wrap;
double WrapOneSeventhSlope(double x, double y) {
  return ptr_wrap->OneSeventhLawsSlope_(x,y);
}

double WrapLaminarVelocitySlope(double x, double y) {
  return ptr_wrap->LaminarVelocitySlope_(x,y);
}

void SinglePhaseHydrodynamics::SetAll(const Fluid& fluid,
    const PipeGeometry& pipe,
    double average_velocity_at_inlet,
    const InletCondition& inlet){
  fluid_ = fluid;
  pipe_ = pipe;
  v_inlet_ = average_velocity_at_inlet; 
  inlet_ = inlet;
}

void SinglePhaseHydrodynamics::SetFluidProperties(const Fluid& fluid) {
  fluid_ = fluid;
}

void SinglePhaseHydrodynamics::SetInletFlow(const InletCondition& inlet) {
  inlet_ = inlet;
}

void SinglePhaseHydrodynamics::SetPipeGeometry(const PipeGeometry& pipe) {
  pipe_ = pipe;
}

void SinglePhaseHydrodynamics::DiscretizeR(double T, double delta) {
  double rho, d, mu;
  rho = fluid_.rho_.At(T);
  mu = fluid_.mu_.At(T);
  d = pipe_.d_ - 2.0 * delta;
  v_ = v_inlet_ * fluid_.rho_.At(
    inlet_.GetProperties(usr::kTwoPhaseVar::inlet_temperature)) /
    rho * std::pow(pipe_.d_ / d, 2);
  re_ = rho * v_ * d / mu;
  if (re_ > 2100) {
    DiscretizeRRatioMesh_turbulent();;
  }
  else {
    DiscretizeRRatioMesh_laminar();
  }
}

void SinglePhaseHydrodynamics::Solve(double T, double delta, 
    double roughness) {
  double rho, d, mu;
  rho = fluid_.rho_.At(T);
  mu = fluid_.mu_.At(T);
  d = pipe_.d_ - 2.0 * delta;
  v_ = v_inlet_ * fluid_.rho_.At(
    inlet_.GetProperties(usr::kTwoPhaseVar::inlet_temperature)) /
    rho * std::pow(pipe_.d_ / d, 2);
  re_ = rho * v_ * d / mu;
  fanning_ = HaalandEquation(roughness, d, re_);
  tau_w_ = 0.5 * fanning_ * rho * v_ * v_;
  minus_dp_dl_friction_ = 2.0 * fanning_ * rho * v_ * v_ / d;
  minus_dp_dl_gravity_ = rho * usr::g_value * std::sin(pipe_.theta_);
  minus_dp_dl_total_ = minus_dp_dl_friction_ + minus_dp_dl_gravity_;
  if (re_ > 2100) {
    SolveTurbulent(T);
  }
  else {
    SolveLaminar(T);
  }
}
double SinglePhaseHydrodynamics::SolveOnlyShearStress(
    double T,
    double ri,
    double roughness) {
  double rho, d, mu;
  rho = fluid_.rho_.At(T);
  mu = fluid_.mu_.At(T);
  d = 2.0 * ri;
  v_ = v_inlet_ * fluid_.rho_.At(
    inlet_.GetProperties(usr::kTwoPhaseVar::inlet_temperature)) /
    rho * std::pow(pipe_.d_ / d, 2);
  re_ = rho * v_ * d / mu;
  fanning_ = HaalandEquation(roughness, d, re_);
  return 0.5 * fanning_ * rho * v_ * v_;
}

void SinglePhaseHydrodynamics::SolveTurbulent(double T) {
  v_star_ = v_ * std::sqrt(fanning_ / 2.0);
  if (r_ratio_mesh_.size() < 3) {
    std::cout << "error occur\n";
    std::cout << "discretize r-mesh before calculate";
    std::cout << " turbulent velocity profile\n";
  }
  else {
    double y_plus;
    double v_plus;
    double v;

    for(int i = 0; i < rmesh_size_; i++){
      y_plus = (1.0 - r_ratio_mesh_[i]) * re_ / 2.0 *
        std::sqrt(fanning_ / 2.0);
      v_plus = VPlusTurbulent(y_plus);
      v = v_plus * v_star_;
      v_at_r_mesh_[i] = v;
      y_plus_mesh_[i] = y_plus;
      v_plus_mesh_[i] = v_plus;
    }
  }
}

void SinglePhaseHydrodynamics::SolveLaminar(double T) {
  double v;
  for (int i = 0; i < rmesh_size_; i++){
    v_at_r_mesh_[i] = VLaminar(r_ratio_mesh_[i]);
  }
}

double SinglePhaseHydrodynamics::VPlusTurbulent(double y_plus) {
  if (y_plus <= 5) {
    return y_plus;
  }
  else if (y_plus <= 30){
    return 5.0 * std::log(y_plus) - 3.05;
  }
  else {
    return 2.5 * std::log(y_plus) + 5.5;
  }
}

double SinglePhaseHydrodynamics::VLaminar(double r_ratio) {
  return 2.0 * v_ * (1 - r_ratio * r_ratio);
}

double SinglePhaseHydrodynamics::HaalandEquation(double eps,
  double d, double re) const {
  double e_over_d = eps / d;
  if (re > 2100) {
    return std::abs(0.25 * std::pow(-1.8 * std::log10(pow(
      (e_over_d / 3.7), 1.11)
      + 6.9 / re), -2));
  }
  else {
    return std::abs(16.0 / re);
  }
}

double SinglePhaseHydrodynamics::OneSeventhLaws_(double r_ratio) {
  return (std::pow((1 - r_ratio), 1.0 / 7.0));
}

double SinglePhaseHydrodynamics::OneSeventhLawsSlope_(double r_ratio,
    double v) {
  return -1.0 / 7.0 * std::pow((1.0 - r_ratio), -6.0 / 7.0);
}

double SinglePhaseHydrodynamics::LaminarVelocitySlope_(double r_ratio,
    double v) {
  return -2.0 * r_ratio;
}

void SinglePhaseHydrodynamics::DiscretizeRRatioMesh_laminar() {
  long num_fn_call;
  std::vector<double> v_fromDOPRI54;
  if (!r_ratio_mesh_.empty()) {
    r_ratio_mesh_.clear();
  }
  double delta = 1.0/max_num_section;
  for (long i = 0; i <= max_num_section; i++) {
    r_ratio_mesh_.push_back(i * delta);
  }
  rmesh_size_ = r_ratio_mesh_.size();
  if (!v_at_r_mesh_.empty()) {
    v_at_r_mesh_.clear();
  }
  v_at_r_mesh_.assign(rmesh_size_, 0.0);
}

void SinglePhaseHydrodynamics::DiscretizeRRatioMesh_turbulent() {
  long num_fn_call;
  std::vector<double> v_fromDOPRI54;
  if (!r_ratio_mesh_.empty()) {
    r_ratio_mesh_.clear();
  }
  usr::DormandPrince1D(WrapOneSeventhSlope, &num_fn_call, &r_ratio_mesh_,
    &v_fromDOPRI54, 1, 0, 0.999999, 0.3, 1.0e-6,
    max_num_section - 1, 100, 4.0, 10);
  r_ratio_mesh_.push_back(1);
  rmesh_size_ = r_ratio_mesh_.size();
  if (!v_at_r_mesh_.empty()) {
    v_at_r_mesh_.clear();
  }
  if (!v_plus_mesh_.empty()) {
    v_plus_mesh_.clear();
  }
  if (!y_plus_mesh_.empty()) {
    y_plus_mesh_.clear();
  }
  v_at_r_mesh_.assign(rmesh_size_, 0.0);
  v_plus_mesh_.assign(rmesh_size_, 0.0);
  y_plus_mesh_.assign(rmesh_size_, 0.0);
}

std::ostream& operator<< (std::ostream& os,
  const SinglePhaseHydrodynamics& obj) {
  return obj.Print(os);
}

std::ostream& SinglePhaseHydrodynamics::Print(
    std::ostream& os) const {
  os << "SinglePhaseHydrodynamics\n";
  os << "  v_[m/s] = ," << v_ << '\n';
  os << "  v_star_[m/s] = ," << v_star_ << '\n';
  os << "  re_[-] = ," << re_ << '\n';
  os << "  tau_w_[pa] = ," << tau_w_ << '\n';
  os << "  fanning_[-] = ," << fanning_ << '\n';
  os << "  minus_dp_dl_total_[Pa] = ," << minus_dp_dl_total_ << '\n';
  os << "  minus_dp_dl_gravity_[Pa] = ," << minus_dp_dl_gravity_ << '\n';
  os << "  minus_dp_dl_friction_[Pa] = ," << minus_dp_dl_friction_ << '\n';
  return os;
}

std::ostream& SinglePhaseHydrodynamics::PrintRProfile(
  std::ostream& os) const {
  os << std::setprecision(15);
  os << "r_ratio_mesh[-]\n";
  for (auto value : r_ratio_mesh_) {
    os << value << '\n';
  }
  return os;
}

std::ostream& SinglePhaseHydrodynamics::PrintRVProfile(
  std::ostream& os) const {
  os << std::setprecision(15);
  if(re_ > 2100){
    os << "turbulent flow: Re = ," << re_ << '\n';
    os << "r_ratio_mesh[-], v_mesh[m/s], y_plus[-], v_plus[-]\n";
    for (long i = 0; i < r_ratio_mesh_.size(); i++) {
      os << r_ratio_mesh_[i] << " ," << v_at_r_mesh_[i] << " ," <<
        y_plus_mesh_[i] << " ," << v_plus_mesh_[i] << '\n';
    }
  }
  else {
    os << "laminar flow: Re = ," << re_ << '\n';
    os << "r_ratio_mesh[-], v_mesh[m/s]\n";
    for (long i = 0; i < r_ratio_mesh_.size(); i++) {
      os << r_ratio_mesh_[i] << " ," << v_at_r_mesh_[i] << '\n';
    }
  }
  return os;
}

void SinglePhaseHydrodynamics::GetPipeGeometry(
    PipeGeometry* pipe) const {
  *pipe = pipe_;
}

void SinglePhaseHydrodynamics::GetInletCondition(
    InletCondition* inlet) const {
  *inlet = inlet_;
}

void SinglePhaseHydrodynamics::GetFluid(
    Fluid* fluid) const {
  *fluid = fluid_;
}

double SinglePhaseHydrodynamics::GetProperties(
    usr::kSinglePhaseFlow var_name) const {
  switch (var_name) {
    case usr::kSinglePhaseFlow::re: return re_; break;
    case usr::kSinglePhaseFlow::fanning: return fanning_; break;
    case usr::kSinglePhaseFlow::v_inlet: return v_; break;
    case usr::kSinglePhaseFlow::n_rmesh: return rmesh_size_; break;
  }
}

std::vector<double> SinglePhaseHydrodynamics::GetRMeshRatio() const {
  return r_ratio_mesh_;
}

} // namespace spdepo