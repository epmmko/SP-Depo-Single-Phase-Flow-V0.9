#include "deposit_properties.h"
#include <vector>
#include "user_equation_format.h"
#include "user_math_const.h"
#include "fluid_properties.h"
namespace spdepo {
  DepositProfile::DepositProfile() {
  }
  void DepositProfile::Initialize(
      int sec_size, 
      usr::BigEquation k_oil_eq,
      usr::BigEquation k_wax_eq,
      usr::BigEquation rho_oil_eq,
      double pipe_radius,
      double overall_roughness) {
    sec_size_ = sec_size;
    k_oil_eq_ = k_oil_eq;
    k_wax_eq_ = k_wax_eq;
    rho_oil_eq_ = rho_oil_eq;
    pipe_radius_ = pipe_radius;
    overall_roughness_ = overall_roughness;
  }
  void DepositProfile::SetPipeRadius(double pipe_radius) {
    pipe_radius_ = pipe_radius;
  }

  void DepositProfile::SetRiVec(const std::vector<double>& ri_vec) {
    ri_vec_ = ri_vec;
  }
  void DepositProfile::SetFwVec(const std::vector<double>& fw_vec) {
    fw_vec_ = fw_vec;
  }
  void DepositProfile::SetMassVec(const std::vector<double>& mass_vec) {
    mass_vec_ = mass_vec;
    double sum = 0;
    for (double m : mass_vec) {
      sum = sum + m;
    }
    total_wax_mass_ = sum;
  }

  void DepositProfile::SetZMesh(const std::vector<double>& zmesh_vec) {
    zmesh_vec_ = zmesh_vec;
  }


  double DepositProfile::GetKDeposit(int index, double t_celsius) const {
    double fw;
    fw = fw_vec_[index];
    return CalculateKDep(t_celsius, fw);;
  }
  int DepositProfile::GetSecSize() const{
    return sec_size_;
  }
  double DepositProfile::GetRhoDeposit(int index, double t_celsius) const {
    //Assume rho dep ~ rho oil
    return  rho_oil_eq_.At(t_celsius);
  }
  double DepositProfile::GetTotalDepositVolume(const std::vector<double>& z_mesh,
      double R) const{
    double sum = 0;
    double delta_z;
    double ri;
    for (int i = 0; i < sec_size_; i++) {
      delta_z = z_mesh[i+1] - z_mesh[i];
      ri = ri_vec_[i];
      sum = sum + usr::pi * (R*R - ri*ri) * delta_z;
    }
    return sum;
  }

  std::vector<double> DepositProfile::GetDeltaVec() const{
    std::vector<double> delta_vec;
    for (int i = 0; i < ri_vec_.size(); i++) {
      delta_vec.push_back(pipe_radius_ - ri_vec_[i]);
    }
    return delta_vec;
  }
  std::vector<double> DepositProfile::GetFwVec() const{
    return fw_vec_;
  }
  std::vector<double> DepositProfile::GetRiVec() const{
    return ri_vec_;
  }
  std::vector<double> DepositProfile::GetMassVec() const{
    return mass_vec_;
  }

  double DepositProfile::GetPipeRadius() const {
    return pipe_radius_;
  }

  double DepositProfile::GetTotalWaxMass(
      const std::vector<double>& tdep_vec) const {
    double sum = 0;
    double rho;
    for (int i = 0; i < sec_size_; i++) {
      rho = rho_oil_eq_.At(tdep_vec[i]);
      sum = sum + fw_vec_[i] * rho * usr::pi * (pipe_radius_ * pipe_radius_ - 
        ri_vec_[i] * ri_vec_[i]) * (zmesh_vec_[i+1] - zmesh_vec_[i]);
    }
    return sum;
  }

  double DepositProfile::CalculateKDep(double t_celsius, double fw) const {
    double k_oil;
    double k_wax;
    double k_dep;
    k_oil = k_oil_eq_.At(t_celsius);
    k_wax = k_wax_eq_.At(t_celsius);
    //page 281 Bird, Stewart, and Lightfoot Transport Phenomena 2nd ed.
    //and energy Fuels 2017, 31, 2457-2479 (Singh et al 2017)
    //for k1 (wax) embedded in k0 (oil) phase
    k_dep = k_oil * (1.0 + 3.0 * fw / (
      (k_wax + 2.0 * k_oil) / (k_wax - k_oil) - fw));
    return k_dep;
  }

  void WaxProperties::SetAll(
    const usr::BigEquation& precipitation_curve,
    const usr::BigEquation& dCdT,
    const Fluid& oil,
    double T_coolant_ini_inlet,
    double wax_molar_volume) {
    SetPrecipitationCurve(precipitation_curve);
    oil_ = oil;
    dCdT_ = dCdT;
    T_coolant_ini_inlet_ = T_coolant_ini_inlet;
    rho_c_ = oil_.GetProperty(usr::kFluid::rho, T_coolant_ini_inlet);
    sf_c_ = precipitation_curve.At(T_coolant_ini_inlet);
    depositable_wax_molar_volume_ = wax_molar_volume;
  }

  void WaxProperties::SetPrecipitationCurve(
    usr::BigEquation precipitation_curve) {
    precipitation_curve_ = precipitation_curve;
  }

  double WaxProperties::GetC(double T) {
    //return the relative available dissolved wax concentration [kg/m3]
    //T is in degree-C
    double rho_T; //rho_oil at temperature T[C] [kg/m3]
    double sf_h; // solid fraction corresponding to rho_h [kg/kg]

    rho_T = oil_.GetProperty(usr::kFluid::rho, T);
    sf_h = precipitation_curve_.At(T);
    return rho_c_ * sf_c_ - rho_T * sf_h;
  }

  double WaxProperties::CalculateDiffusivity(double T) const{
    //use Hayduk - Minhas (1982) equation with the
    //  oil viscosity (with precipitated wax) to do the
    //  estimation
    //From Correra et al, Applied Mathemtical Modelling 31(2007) 2286-2298
    // the order of magnitude of wax diffusivity is about 4.4e-10 m2/s
    double mu = oil_.GetProperty(usr::kFluid::mu,T);
    double gamma = 10.2 / depositable_wax_molar_volume_ - 0.791;
    double Dwo;
    return (13.3e-12 * std::pow((T + 273.15),1.47) * 
            std::pow((mu * 1000), gamma) / 
            std::pow(depositable_wax_molar_volume_, 0.71));
  }

  double WaxProperties::CalculateEffectiveDiffusivityRatio(
    double alpha, double fw) const {
    return 1.0 / (1.0 + alpha * alpha * fw * fw / (1.0 - fw));
  }

  double WaxProperties::GetDCDT(double T) {
    return dCdT_.At(T);
  }


} // namespace spdepo