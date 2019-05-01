#include "simulation_options.h"

namespace spdepo {
  void SimulationOptions::SetPrecipitationRate(double kr) {
    kr_ = kr;
  }
  
  void SimulationOptions::SetDepositionDuration(
      double t_total_second) {
    deposition_duration_ = t_total_second;
  }

  double SimulationOptions::GetDepositionDuration() {
    return deposition_duration_;
  }

  void SimulationOptions::SetAlphaEquation(
      usr::kDepositionOption option,
      double k_alpha,
      double A,
      double Fw0) {
    A_alpha_ = A;
    switch (option) {
      case(usr::kDepositionOption::Singh_et_al_2000_k_alpha_eq):{
        alpha_eq_.SetId(1);
        alpha_eq_.SetCoefficient({k_alpha, 1.0});
        break;
      }
      case(usr::kDepositionOption::Lee2008_k_alpha_eq): {
        usr::Equation eq1;
        usr::Equation eq2;
        usr::Equation eq3;
        usr::UserOperator op1;
        usr::UserOperator op2;
          // eq1: (Fw - Fw0)/A
          // eq2 = 0.5  // op1 = eq1 ^ eq2
          // eq3 = 1    // op2 = 1 + op1_ans
        eq1.SetId(1);
        eq1.SetCoefficient({ 1.0 / A, -Fw0 / A });
        eq2.SetId(0);
        eq2.SetCoefficient({0.5});
        eq3.SetId(0);
        eq3.SetCoefficient({1.0});
        op1.SetId(9);
        op2.SetId(2);
        alpha_eq_.SetEquationVector({eq1, eq2, eq3});
        alpha_eq_.SetOperatorVector({op1, op2});
        break;
      }
    }
  }
  void SimulationOptions::SetAlphaEquation(
      const usr::BigEquation& alpha_eq){
    alpha_eq_ = alpha_eq;
  }
  void SimulationOptions::SetAAlpha(double A_alpha) {
    A_alpha_ = A_alpha;
  }

  void SimulationOptions::SetAlphaOption(
      usr::kDepositionOption alpha_options) {
    alpha_options_ = alpha_options;
  }

  usr::kDepositionOption SimulationOptions::GetAlphaOption() const {
    return alpha_options_;
  }

  usr::BigEquation SimulationOptions::GetAlphaEq() const {
    return alpha_eq_;
  }

  void SimulationOptions::SetAlphaEquationLee2008Form(double Fw0) {
    SetAlphaEquation(
      alpha_options_,
      0.0,
      A_alpha_,
      Fw0);
  }

  void SimulationOptions::SetKAlpha(double k_alpha) {
    k_alpha_ = k_alpha;
  }

  double SimulationOptions::GetPrecipitationRate() const {
    return kr_;
  }
  void SimulationOptions::SetSurroundingType(
      usr::kSurroundingType surrounding_type) {
    surrounding_type_ = surrounding_type;
  }

  void SimulationOptions::SetKrAsAFunctionOfTemperature(
      bool yes_no_input) {
    kr_as_a_function_of_temperature_option_ = yes_no_input;
  }
  void SimulationOptions::SetTRefForKr(double T_reference) {
    Tref_for_kr_ = T_reference;
  }
  double SimulationOptions::GetTRefForKr() {
    return Tref_for_kr_;
  }
  void SimulationOptions::SetDwoIniForKr() {
    dwo_Tref_ini_ = wax_prop_.CalculateDiffusivity(Tref_for_kr_);
  }
  double SimulationOptions::GetDwoIniForKr() {
    return dwo_Tref_ini_;
  }
  bool SimulationOptions::GetKrAsAFunctionOfTemperatureOption() {
    return kr_as_a_function_of_temperature_option_;
  }
  void SimulationOptions::SetWaxProperties(
      const usr::BigEquation& precipitation_curve,
      const usr::BigEquation& dCdT,
      const Fluid& oil,
      double T_coolant_ini_inlet,
      double wax_molar_volume) {
    wax_prop_.SetAll(
      precipitation_curve,
      dCdT,
      oil,
      T_coolant_ini_inlet,
      wax_molar_volume);
  }
  void SimulationOptions::SetWaxProperties(WaxProperties wax_prop) {
    wax_prop_ = wax_prop;
  }
  void SimulationOptions::SetUserDefinedPrecipitationEquation(
    const usr::BigEquation& precipitation_equation) {
    user_precipitation_equation_ = precipitation_equation;
  }
  usr::BigEquation SimulationOptions::GetUserDefinedPrecipitationEquation() {
    return user_precipitation_equation_;
  }
  void SimulationOptions::SetUserDefineddCdTEquation(
    const usr::BigEquation& dCdT_equation) {
    user_dCdT_equation_ = dCdT_equation;
  }
  usr::BigEquation SimulationOptions::GetUserDefinededdCdTEquation() {
    return user_dCdT_equation_;
  }
  void SimulationOptions::SetUserTCReferenceForSolidFraction(
      double T_coolant_ini_inlet) {
    user_T_coolant_ini_inlet_ = T_coolant_ini_inlet;
  }
  double SimulationOptions::GetUserTCReferenceForSolidFraction() {
    return user_T_coolant_ini_inlet_;
  }
  void SimulationOptions::SetUserWaxMolarVolume(
      double wax_molar_volume) {
    user_wax_molar_volume_ = wax_molar_volume;
  }
  double SimulationOptions::GetUserWaxMolarVolume() {
    return user_wax_molar_volume_;
  }
  void SimulationOptions::SetInitialTotalOilVolume(
      double initial_total_oil_volume) {
    initial_total_oil_volume_ = initial_total_oil_volume;
  }
  double SimulationOptions::GetInitialTotalOilVolume() {
    return initial_total_oil_volume_;
  }
  void SimulationOptions::SetWaxMaterialProperties(
      const Solid& wax_material) {
    wax_material_ = wax_material;
  }
  Solid SimulationOptions::GetWaxMaterialProperties() {
    return wax_material_;
  }
  void SimulationOptions::SetPredictCompositionOption(
      bool true_or_false) {
    predict_composition_ = true_or_false;
  }
  bool SimulationOptions::GetPredictCompositionOption() {
    return predict_composition_;
  }
  void SimulationOptions::SetPrecipitationCurveModel(
      usr::kPrecipitationCurveModel pcurve_model) {
    precipitation_curve_model_ = pcurve_model;
  }
  usr::kPrecipitationCurveModel 
      SimulationOptions::GetPrecipitationCurveModel() {
    return precipitation_curve_model_;
  }
  void SimulationOptions::SetReportOption(usr::kReportOption option) {
    report_option_ = option;
  }
  usr::kReportOption SimulationOptions::GetReportOption() {
    return report_option_;
  }
  void SimulationOptions::SetPrecipitationRateModel(
      usr::kPrecipitationRateModel precipitation_rate_model) {
    precipitation_rate_model_ = precipitation_rate_model;
    if (precipitation_rate_model == 
        usr::kPrecipitationRateModel::temperature_dependent_kr) {
      kr_as_a_function_of_temperature_option_ = true;
    }
  }
  usr::kPrecipitationRateModel
      SimulationOptions::GetPrecipitationRateModel() {
    return precipitation_rate_model_;
  }
  void SimulationOptions::SetShearDepositionModel(usr::kShearDepositionModel option) {
    shear_option_ = option;
  }
  usr::kShearDepositionModel SimulationOptions::GetShearDepositionModel() {
    return shear_option_;
  }
  void SimulationOptions::SetMSR1(double msr1){
    msr1_ = msr1;
  }
  double SimulationOptions::GetMSR1() const {
    return msr1_;
  }
  void SimulationOptions::SetMSR2(double msr2) {
    msr2_ = msr2;
  }
  double SimulationOptions::GetMSR2() const {
    return msr2_;
  }
} //namespace spdepo