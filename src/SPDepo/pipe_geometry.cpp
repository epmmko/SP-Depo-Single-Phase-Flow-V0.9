#include "pipe_geometry.h"
#include <iostream>
#include <iomanip>
#include "user_math_const.h"

namespace spdepo{
PipeGeometry::PipeGeometry(){
}

void PipeGeometry::SetProperties(const std::string& name,
  double inner_diameter,
  double outer_diameter,
  double roughness,
  double inner_diameter_jacket,
  double annulus_roughness,
  double inclination_angle,
  double z_heat_transfer_start,
  double heat_transfer_length){
  pipe_name_ = name;
  d_ = inner_diameter;
  d_o_ = outer_diameter;
  eps_p_ = roughness;
  d_j_ = inner_diameter_jacket;
  eps_j_ = annulus_roughness;
  theta_ = inclination_angle;
  z_heat_transfer_start_ = z_heat_transfer_start;
  l_heat_transfer_ = heat_transfer_length;
  l_total_ = z_heat_transfer_start + heat_transfer_length;
  a_p_ = usr::pi * d_ * d_ * 0.25;
  a_j_ = usr::pi * 0.25 * (d_j_ * d_j_ - d_o_ * d_o_);
  d_jhy_ = d_j_ - d_o_;
}

void PipeGeometry::SetAngleInDegree(const double& degree) {
  theta_ = degree * usr::pi / 180.0;
}

void PipeGeometry::SetAngleInRadian(const double& radian) {
  theta_ = radian;
}

std::ostream& PipeGeometry::Print(std::ostream& os) const {
  os << std::setprecision(5);
  os << "pipe_name = ," << pipe_name_ << '\n';
  os << "  innter_diameter[m] = ," << d_ << '\n';
  os << "  outer_diameter[m] = ," << d_o_ << '\n';
  os << "  roughness[m] = ," << eps_p_ << '\n';
  os << "  inner_diameter_jacket[m] = ," << d_j_ << '\n';
  os << "  annulus_roughness[m] = ," << eps_j_ << '\n';
  os << "  inclination_angle[rad] = ," << theta_ << '\n';
  os << "  z_heat_transfer_start[m] = ," << z_heat_transfer_start_ << '\n';
  os << "  heat_transfer_length[m] = ," << l_heat_transfer_ << '\n';
  os << "  l_total_[m] = ," << l_total_ << '\n';
  os << "  flow_in_pipe_cross_section_area[m^2] =," << a_p_ << '\n';
  os << "  flow_in_annulus_cross_section_area[m^2] =," << a_j_ << '\n';
  return os;
}

std::ostream& operator<< (std::ostream& os, const PipeGeometry& obj) {
  return obj.Print(os);
}

double PipeGeometry::GetProperties(usr::kGeometry var_name) {
  switch (var_name) {
    case usr::kGeometry::d: return d_; break;
    case usr::kGeometry::d_o: return d_o_; break;
    case usr::kGeometry::eps_p: return eps_p_; break;
    case usr::kGeometry::eps_j: return eps_j_; break;
    case usr::kGeometry::d_j: return d_j_; break;
    case usr::kGeometry::d_jhy: return d_jhy_; break;
    case usr::kGeometry::theta: return theta_; break;
    case usr::kGeometry::a_p: return a_p_; break;
    case usr::kGeometry::a_j: return a_j_; break;
    case usr::kGeometry::l_total: return l_total_; break;
    case usr::kGeometry::l_heat_transfer: return l_heat_transfer_; break;
    case usr::kGeometry::z_heat_tranfer_start:
      return z_heat_transfer_start_; break;
    case usr::kGeometry::a_p_tilde: return a_p_tilde_; break;
  }
}
void PipeGeometry::SetProperties(usr::kGeometry var_name, double value) {
  switch (var_name) {
    case usr::kGeometry::d:{
      d_ = value;
      a_p_ = usr::pi * d_ * d_ * 0.25;
      break;
    }
    case usr::kGeometry::d_o:{
      d_o_ = value;
      a_j_ = usr::pi * 0.25 * (d_j_ * d_j_ - d_o_ * d_o_);
      d_jhy_ = d_j_ - d_o_;
      break;
    }
    case usr::kGeometry::eps_p:{
      eps_p_ = value;
      break;
    }
    case usr::kGeometry::eps_j:{
      eps_j_ = value;
      break;
    }
    case usr::kGeometry::d_j:{
      d_j_ = value;
      a_j_ = usr::pi * 0.25 * (d_j_ * d_j_ - d_o_ * d_o_);
      d_jhy_ = d_j_ - d_o_;
      break;
    }
    case usr::kGeometry::theta:{
      theta_ = value;
      break;
    }
    case usr::kGeometry::l_heat_transfer:{
      l_heat_transfer_ = value;
      l_total_ = z_heat_transfer_start_ + l_heat_transfer_;
      break;
    }
    case usr::kGeometry::z_heat_tranfer_start:{
      z_heat_transfer_start_ = value;
      l_total_ = z_heat_transfer_start_ + l_heat_transfer_;
      break;
    }
  }
}

void PipeGeometry::GetName(std::string* name) {
  *name = pipe_name_;
}
} // namespace spdepo