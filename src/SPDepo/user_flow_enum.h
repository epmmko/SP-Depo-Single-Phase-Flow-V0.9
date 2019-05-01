#ifndef USER_FLOW_ENUM_H_
#define USER_FLOW_ENUM_H_
namespace spdepo{
namespace usr {
  enum class kTwoPhaseVar { v_sl, v_sg, v_m , inlet_temperature};
  enum class kFluid {rho, mu, cp, k, name};
  enum class kSolid {rho, cp, k};
  enum class kGeometry {d, d_o, eps_p, eps_j, d_j, d_jhy, theta, a_p, a_j,
                        l_total, l_heat_transfer, z_heat_tranfer_start,
                        a_p_tilde};
  enum class kSinglePhaseFlow {re, fanning, v_inlet, n_rmesh, r_mesh};
  enum class kProfile {z_mesh, tc_mesh, th_mesh, nu_h_mesh, 
                       nu_c_mush, ur_inv_mesh};
  enum class kCoolant {flow_counter_currently, volume_flow_rate, 
                       inlet_temperature};
  enum class kRadialTProfile {
      readme_t_zr_mesh_,
      r_mesh_,
      r_ratio_mesh_,
      t_zr_mesh_, 
      c_zr_mesh_,
      z_group_,
      z_mesh_};
  enum class kDepositionOption {
    Lee2008_k_alpha_eq,
    Singh_et_al_2000_k_alpha_eq,
  };
  enum class kSurroundingType {
    flow_loop,
    field
  };
  enum class kDepositionPrintOption {
    delta_mm_profile,
    ri_profile, //section data
    fw_profile, //section data
    delta_mm_average,
    ri_average,
    fw_average,
    t_hr_vec,
    z_sec_vec,
    z_mesh_vec,
    ri_profile_picture,
    delta_profile_picture,
    fw_profile_picture
  };
  enum class kPrecipitationCurveModel {
    user_defined,
    compositional,
    lumped
  };
  enum class kReportOption {
    standard_output,
    moderate_output,
    detailed_output,
    research_level_output
  };
  enum class kPrecipitationRateModel {
    constant_kr,
    temperature_dependent_kr,
    fractional_kr
  };
  enum class kReadInput {
    oil
  };
  enum class kShearDepositionModel {
    none,
    MaximumTolerableShearStressModel
  };
} // namespace usr
} // namespace spdepo

#endif