#ifndef SPDEPO_SIMULATION_OPTIONS
#define SPDEPO_SIMULATION_OPTIONS

#include "user_flow_enum.h"
#include "user_equation_format.h"
#include "deposit_properties.h"
#include "solid_properties.h"
namespace spdepo{
class SimulationOptions {
 public:
  //If not use any value, set it to 0;
  void SetAlphaOption(usr::kDepositionOption alpha_options);
  void SetAlphaEquation(usr::kDepositionOption option,
    double k_alpha,
    double A,
    double Fw0);
  void SetAlphaEquation(const usr::BigEquation& alpha_eq);
  void SetAlphaEquationLee2008Form(double Fw0);
  void SetAAlpha(double A_alpha);
  void SetPrecipitationRate(double kr);
  void SetPrecipitationRateModel(
    usr::kPrecipitationRateModel precipitation_rate_model);
  usr::kPrecipitationRateModel GetPrecipitationRateModel();
  void SetKAlpha(double k_alpha);
  void SetDepositionDuration(double t_total_second);
  void SetSurroundingType(usr::kSurroundingType surrounding_type);
  double GetDepositionDuration();
  double GetPrecipitationRate() const;
  
  usr::BigEquation GetAlphaEq() const;
  usr::kDepositionOption GetAlphaOption() const;
  void SetKrAsAFunctionOfTemperature(bool yes_no_input);
  bool GetKrAsAFunctionOfTemperatureOption();
  void SetWaxProperties(
    const usr::BigEquation& precipitation_curve,
    const usr::BigEquation& dCdT,
    const Fluid& oil,
    double T_coolant_ini_inlet,
    double wax_molar_volume = 597);
    //this function is to be called when updating the precipitation curve
    //  not implemented yet
  void SetWaxProperties(WaxProperties wax_prop);
    //this function is to be called when initialize a deposition object
  void SetUserDefinedPrecipitationEquation(
    const usr::BigEquation& precipitation_equation);
  usr::BigEquation GetUserDefinedPrecipitationEquation();
  void SetUserDefineddCdTEquation(
    const usr::BigEquation& dCdT_equation);
  usr::BigEquation GetUserDefinededdCdTEquation();
  void SetUserTCReferenceForSolidFraction(double T_coolant_ini_inlet);
  double GetUserTCReferenceForSolidFraction();
  void SetUserWaxMolarVolume(double wax_molar_volume);
  double GetUserWaxMolarVolume();
  void SetInitialTotalOilVolume(double initial_total_oil_volume_);
  double GetInitialTotalOilVolume();
    //for flow loop, wax depletion calculation.
  void SetWaxMaterialProperties(const Solid& wax_material);
  Solid GetWaxMaterialProperties();
  void SetPredictCompositionOption(bool true_or_false);
  bool GetPredictCompositionOption();
  void SetPrecipitationCurveModel(usr::kPrecipitationCurveModel pcurve_model);
  usr::kPrecipitationCurveModel GetPrecipitationCurveModel();
  void SetReportOption(usr::kReportOption option);
  usr::kReportOption GetReportOption();
  void SetTRefForKr(double T_reference);
  double GetTRefForKr();
  void SetDwoIniForKr();
  double GetDwoIniForKr();
  void SetShearDepositionModel(usr::kShearDepositionModel);
  usr::kShearDepositionModel GetShearDepositionModel();
  void SetMSR1(double msr1);
  double GetMSR1() const;
  void SetMSR2(double msr2);
  double GetMSR2() const;
  
 private:
  WaxProperties wax_prop_; //dC/dT, precipitation curve, etc
  Solid wax_material_; //mainly for thermal conductivity
  usr::kDepositionOption dep_options_;
  //crystal aspect ratio option
  usr::kDepositionOption alpha_options_; 
  usr::kSurroundingType surrounding_type_;
  usr::kPrecipitationCurveModel precipitation_curve_model_;
  usr::kPrecipitationRateModel precipitation_rate_model_;
  usr::kReportOption report_option_;
  double initial_total_oil_volume_;
    //for flow loop depletion calculation
  double kr_;
  double dwo_Tref_ini_; //initial Dwo at T_reference for kr_ calculation
  double Tref_for_kr_;
  double A_alpha_;
  double k_alpha_;
  double deposition_duration_;
  usr::BigEquation alpha_eq_;
  bool kr_as_a_function_of_temperature_option_ = false;
    //false means kr is constant at any temperature
    //true means kr will be adjusted with respect to
    //  Dwo(T)/Dwo(WAT) ratio
  bool predict_composition_ = false;
    //if true, use SP-Wax
  //User Defined Precipitation Curve
  usr::BigEquation user_precipitation_equation_;
  usr::BigEquation user_dCdT_equation_;
  double user_T_coolant_ini_inlet_;
    //use for solid fraction at the coldest reference temperature
  double user_wax_molar_volume_; //[cm3/gmol]
  //shear deposition model
  double msr1_;
  double msr2_;
  usr::kShearDepositionModel shear_option_ = usr::kShearDepositionModel::none;
};
} //namespace spdepo
#endif
