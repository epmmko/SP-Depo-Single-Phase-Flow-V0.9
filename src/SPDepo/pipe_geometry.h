#ifndef SPDEPO_TWOPHASEFLOW_PIPE_GEOMETRY_H_
#define SPDEPO_TWOPHASEFLOW_PIPE_GEOMETRY_H_

#include <string>
#include "user_math_const.h"
#include "user_flow_enum.h"

namespace spdepo{
class PipeGeometry {
public:
  PipeGeometry();
  friend std::ostream& operator<< (std::ostream& os,
    const PipeGeometry& obj);
  friend class SinglePhaseHydrodynamics;
  std::ostream& Print(std::ostream& os) const;
  void SetProperties(const std::string& name = "test_section",
    double inner_diameter = 0.651*0.0254,
    double outer_diameter = 0.75*0.0254,
    double roughness = 0.0,
    double inner_diameter_jacket = 0.995*0.0254,
    double annulus_roughness = 0.0,
    double inclination_angle = 0.0,
    double z_heat_transfer_start = 0.7,
    double heat_transfer_length = 0.84);

  void SetAngleInDegree(const double& degree);
  void SetAngleInRadian(const double& radian);
  double GetProperties(usr::kGeometry var_name);
  void SetProperties(usr::kGeometry var_name, double value);
  void GetName(std::string* name);
protected:
  std::string pipe_name_;
  //geometry
  double a_p_;//pipe cross-section area [m^2]
  double a_j_;//annulus flow cross-section area [m^2]
  double d_; //inner diameter [m]
  double d_o_; //outer_diameter [m]
  double d_j_; //inner diameter of the jacketed pipe [m]
  double d_jhy_; //hydraulic diameter of the jacketed pipe [m]
  double eps_p_; //pipe roughness [m]
  double eps_j_; //annulus/jacketed pipe roughness [m]
  double l_total_;//pipe total length [m]
  double l_heat_transfer_;//pipe length with heat transfer [m]
  double theta_; //inclination_angle [rad]
  double z_heat_transfer_start_;//point / length from the inlet where 
    //heat transfer starts [m]. Default is l_total_ - l_heat_transfer_
  //constant
  double a_p_tilde_ = usr::pi * 0.25;

};
} // namespace spdepo
#endif