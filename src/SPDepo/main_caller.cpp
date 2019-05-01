#include "main_caller.h"
#include "coolant_condition.h"
#include "deposit_properties.h"
#include "fluid_properties.h"
#include "inlet_condition.h"
#include "single_phase_deposition.h"
#include "solid_properties.h"
#include "pipe_geometry.h"
#include "user_equation_format.h"
#include "simulation_options.h"
#include "user_flow_enum.h"
#include "fitting_search.h"
#include <algorithm>
#include "user_numerical_method.h"
#include <filesystem>
#include "program_path.h"
#include "user_string_operation.h"

namespace spdepo {

void Main1PhaseDeposition::Run(ProgramPath p_path) {
  //create input for deposition object
  ReadInput read_input;
  read_input.SetPath(p_path);
  Fluid oil = read_input.ReadLiquid1P();
  Solid pipe_material;
  PipeGeometry pipe;
  read_input.ReadPipe1P(&pipe, &pipe_material);
    //Read pipe properties set the default value for
    //lenght from the inlet that the jacket start
    //This has to be called before ReadSurrounding
  InletCondition inlet = read_input.ReadInlet1P();
  CoolantCondition coolant_inlet;
  Fluid coolant_fluid;
  SimulationOptions sim_option;
  read_input.ReadCoolant1P(&coolant_inlet,&coolant_fluid,&pipe,&sim_option);
  read_input.ReadSimulationOption(&sim_option, coolant_inlet);
  //make wax material properties
  Solid wax;
  wax.SetConstantProperties("wax", -950, -2000, 0.25);
  sim_option.SetWaxMaterialProperties(wax);
  SinglePhaseDeposition deposition1;
  deposition1.SetAll(sim_option,
    oil, coolant_fluid, inlet, coolant_inlet, pipe,
    pipe_material);
  FittingDepositionCall fitting_call;
  fitting_call = read_input.ReadModelTuning();
  std::vector<std::vector<double>> delta_data_s_m = fitting_call.GetGraphPlottingDeltaData();
  std::vector<std::vector<double>> fw_data_s_f = fitting_call.GetGraphPlottingFwData();
  deposition1.SetProgramPath(p_path);
  std::experimental::filesystem::remove_all(p_path.output_path_ + L"profile_data_pic\\");
  std::experimental::filesystem::remove_all(p_path.output_path_ + L"profile_data\\");
  std::experimental::filesystem::remove_all(p_path.output_path_ + L"data_1d\\");
  std::experimental::filesystem::remove_all(p_path.output_path_ + L"data_ave\\");

  deposition1.Calculate();
  deposition1.SimplePostCalculation();
  deposition1.PrintStandardOutput(delta_data_s_m, fw_data_s_f);
  
  usr::kReportOption report_option = sim_option.GetReportOption();
  if ((report_option
        == usr::kReportOption::moderate_output) ||
      (report_option
        == usr::kReportOption::detailed_output) ||
      (report_option
        == usr::kReportOption::research_level_output)) {
    deposition1.DetailedPostCalculation();
    deposition1.PrintDetailedOutput();
  }
}

Fluid ReadInput::ReadLiquid1P() {
  std::ifstream ifs;
  std::wstring ifname = p_path_.output_path_ + L"inputs\\default_Dep1L_.HyLiq";
  Fluid output;
  std::vector<std::string> reading_vec;
  std::size_t n = 5;
  ifs.open(ifname);
    //known that the input has 5 entries
  for (std::size_t i = 0; i < n; i++) {
    std::string reading;
    std::getline(ifs,reading);
    reading_vec.push_back(reading);
  }
  ifs.close();
  output.SetName(reading_vec[0]);
   
  std::vector<usr::kFluid> input_list_vec =
    {usr::kFluid::name, //1st member is just a place holder
     usr::kFluid::rho,
     usr::kFluid::mu,
     usr::kFluid::k,
     usr::kFluid::cp};

  for (std::size_t i = 1; i < n; i++) {
    if (reading_vec[i] == "") {
      std::cerr << "oil_[" << i << "] input is given as an empty string \"\"\n";
      std::system("pause");
    }
    if (reading_vec[i][0] == '*') {
      usr::BigEquation output_eq;
      output_eq.ParseFromString(reading_vec[i]);
      output.SetFunctionalProperties(input_list_vec[i], output_eq);
    } else {
      output.SetConstantValueMember(input_list_vec[i],
        std::stod(reading_vec[i]));
    }
  }
  return output;
}

void ReadInput::ReadPipe1P(PipeGeometry* pipe_geometry,
    Solid* pipe_material) {
  std::ifstream ifs;
  std::wstring ifname = p_path_.output_path_ + L"inputs\\default_Dep1L_.HyPip";
  std::vector<std::string> reading_vec;
  std::size_t n = 8;
  ifs.open(ifname);
  for (std::size_t i = 0; i < n; i++) {
    std::string reading;
    std::getline(ifs,reading);
    reading_vec.push_back(reading);
  }
  ifs.close();
  pipe_geometry->SetProperties(
    reading_vec[0],           //name
    std::stod(reading_vec[1]),//inner_diameter
    std::stod(reading_vec[2]),//outer_diameter
    std::stod(reading_vec[3]),//roughness
    -1.0,                     //inner_diameter_jacket
    -1.0,                     //annulus_roughness
    std::stod(reading_vec[4]),//inclination_angle
    0.0,                      //z_heat_transfer_start
    std::stod(reading_vec[5]));//heat_transfer_length
  double pipe_k_value; //pipe thermal conductivity in W/m/K
  if (reading_vec[7][0] != '*') {
    pipe_material->SetConstantProperties(
      reading_vec[6],
      -1.0, //pipe rho
      -1.0, //pipe cp
      std::stod(reading_vec[7]));
  }
  else {
    usr::BigEquation output_eq;
    output_eq.ParseFromString(reading_vec[7]);
    pipe_material->SetConstantProperties(
      reading_vec[6],
      -1.0, //pipe rho [kg/m3]
      -1.0, //pipe cp  [J/kg/K]
      -1.0);//pipe k   [W/m/K]
    pipe_material->SetFunctionalProperties(usr::kSolid::k, output_eq);
  }
  
}
InletCondition ReadInput::ReadInlet1P() {
  std::ifstream ifs;
  std::wstring ifname = p_path_.output_path_ + L"inputs\\default_Dep1L_.HyInL";
  InletCondition output;
  std::vector<std::string> reading_vec;
  std::size_t n = 3;
  ifs.open(ifname);
  for (std::size_t i = 0; i < n; i++) {
    std::string reading;
    std::getline(ifs,reading);
    reading_vec.push_back(reading);
  }
  ifs.close();
  output.SetProperties(
    reading_vec[0],
    std::stod(reading_vec[1]),
    0.0,
    std::stod(reading_vec[2]));
  return output;
}
void ReadInput::ReadCoolant1P(
    CoolantCondition* coolant,
    Fluid* fluid,
    PipeGeometry* pipe,
    SimulationOptions* sim_option) {
  std::ifstream ifs;
  std::wstring ifname = p_path_.output_path_ + L"inputs\\default_Dep1L_.SurFl";
  std::vector<std::string> reading_vec;
  std::size_t n = 14;
  ifs.open(ifname);
  for (std::size_t i = 0; i < n; i++) {
    std::string reading;
    std::getline(ifs,reading);
    reading_vec.push_back(reading);
  }
  ifs.close();
  bool is_counter_current;
  if (std::stod(reading_vec[2]) == 0) {
    is_counter_current = true;
  } else {
    is_counter_current = false;
  }
  coolant->SetAll
  (
    std::stod(reading_vec[3]), //volume_flow_rate
    is_counter_current, //flow_counter_currently
    std::stod(reading_vec[1])  //inlet_temperature
  );
  usr::kSurroundingType s_type;
  if (reading_vec[0] == "true") {
    s_type = usr::kSurroundingType::flow_loop;
  } else {
    s_type = usr::kSurroundingType::field;
  }
  sim_option->SetSurroundingType(s_type);
  pipe->SetProperties(usr::kGeometry::d_j,std::stod(reading_vec[4]));
  pipe->SetProperties(usr::kGeometry::eps_j,std::stod(reading_vec[5]));
  pipe->SetProperties(usr::kGeometry::z_heat_tranfer_start,
    std::stod(reading_vec[6]));
  pipe->SetProperties(usr::kGeometry::l_heat_transfer,
    std::stod(reading_vec[7]));
  sim_option->SetInitialTotalOilVolume(std::stod(reading_vec[8]));
  //set fluid
  fluid->SetName(reading_vec[9]);
  std::vector<usr::kFluid> input_list_vec =
    {usr::kFluid::rho,
     usr::kFluid::mu,
     usr::kFluid::k,
     usr::kFluid::cp };
  for (std::size_t i = n-4; i < n; i++) {
    if (reading_vec[i] == "") {
      std::cerr << "Surrounding_Flow_Loop_[" 
        << i << "] input is given as an empty string \"\"\n";
      std::system("pause");
    }
    if (reading_vec[i][0] == '*') {
      usr::BigEquation output_eq;
      output_eq.ParseFromString(reading_vec[i]);
      fluid->SetFunctionalProperties(input_list_vec[i-(n-4)], output_eq);
    }
    else {
      fluid->SetConstantValueMember(input_list_vec[i- (n - 4)], std::stod(reading_vec[i]));
    }
  }
}
void ReadInput::ReadSimulationOption(SimulationOptions* sim_option,
    const CoolantCondition& coolant_inlet) {
  sim_option->SetUserTCReferenceForSolidFraction(
    coolant_inlet.GetDouble(usr::kCoolant::inlet_temperature));
  std::ifstream ifs;
  std::wstring ifname = p_path_.output_path_ + L"inputs\\default_Dep1L_.SimOp";
  std::vector<std::string> reading_vec;
  std::size_t n = 16;
  ifs.open(ifname);
  for (std::size_t i = 0; i < n; i++) {
    std::string reading;
    std::getline(ifs, reading);
    //getline is needed here. It cannot be replaced with ifs >> reading
    //because getline also store line with nothing,
    //but ifs >> reading does not
    reading_vec.push_back(reading);
  }
  ifs.close();
  sim_option->SetDepositionDuration(std::stod(reading_vec[0]) * 3600.0);
  if (reading_vec[1] == "0") { 
    //the first option in the combobox is false (cbb index 0)
    sim_option->SetPredictCompositionOption(true);
  } else {
    sim_option->SetPredictCompositionOption(false);
  }
  //set crystal aspect ratio formula
  //index 0 = Singh et al. 2000
  //index 1 = Lee 2008
  //index 2 = User define function
  int alpha_option = std::stoi(reading_vec[2]);
  switch (alpha_option) {
    case 0: {
      usr::kDepositionOption option_temp;
      option_temp = usr::kDepositionOption::Singh_et_al_2000_k_alpha_eq;
      sim_option->SetAlphaOption(option_temp);
      double k_alpha_temp = std::stod(reading_vec[8]);
      sim_option->SetAlphaEquation(option_temp, k_alpha_temp,0.0,0.0);
      break;
    }
    case 1: {
      usr::kDepositionOption option_temp;
      option_temp = usr::kDepositionOption::Lee2008_k_alpha_eq;
      sim_option->SetAlphaOption(option_temp);
      double a_alpha_temp = std::stod(reading_vec[8]);
      sim_option->SetAlphaEquation(option_temp, 0.0, a_alpha_temp, 0.0);
        //the value of Fw0 will be set in the initialization step in the 
        //  SinglePhaseDeposition object
      break;
    }
    case 2: {
      usr::BigEquation eq_parse;
      eq_parse.ParseFromString(reading_vec[8]);
      sim_option->SetAlphaEquation(eq_parse);
      break;
    }
  }
  //set precipitation curve model
  //index 0 = user defined function
  //index 1 = compositional model
  //index 2 = lumped component model
  int prep_curve_model = std::stoi(reading_vec[3]);
  switch (prep_curve_model) {
    case 0: {
      sim_option->SetPrecipitationCurveModel(
        usr::kPrecipitationCurveModel::user_defined);
      usr::BigEquation parse_eq_temp;
      parse_eq_temp.ParseFromString(reading_vec[5]);
      sim_option->SetUserDefinedPrecipitationEquation(parse_eq_temp);
      parse_eq_temp.ParseFromString(reading_vec[6]);
      sim_option->SetUserDefineddCdTEquation(parse_eq_temp);
      sim_option->SetUserWaxMolarVolume(std::stod(reading_vec[7]));
      break;
    }
    case 1: {
      sim_option->SetPrecipitationCurveModel(
        usr::kPrecipitationCurveModel::compositional);
      break;
    }
    case 2: {
      sim_option->SetPrecipitationCurveModel(
        usr::kPrecipitationCurveModel::lumped);
      break;
    }
  }
  //set output setup (amount/quality)
  //index 0 = standard_output,
  //index 1 = moderate_output,
  //index 2 = detailed_output,
  //index 3 = research_level_output
  int output_option = std::stoi(reading_vec[4]);
  switch (output_option) {
    case 0: {
      sim_option->SetReportOption(usr::kReportOption::standard_output);
      break;
    }
    case 1: {
      sim_option->SetReportOption(usr::kReportOption::moderate_output);
      break;
    }
    case 2: {
      sim_option->SetReportOption(usr::kReportOption::detailed_output);
      break;
    }
    case 3: {
      sim_option->SetReportOption(usr::kReportOption::research_level_output);
      break;
    }
  }
  int precipitation_rate_model = std::stoi(reading_vec[9]);
  switch (precipitation_rate_model) {
    case 0: {
      sim_option->SetPrecipitationRateModel(
        usr::kPrecipitationRateModel::constant_kr);
      sim_option->SetPrecipitationRate(std::stod(reading_vec[10]));
      break;
    }
    case 1: {
      sim_option->SetPrecipitationRateModel(
        usr::kPrecipitationRateModel::temperature_dependent_kr);
      sim_option->SetPrecipitationRate(std::stod(reading_vec[11]));
        //for this case kr_ is set to be kr_ at WAT 
        //based on the reading_vec[11]
      break;

    }
    case 2: {
      sim_option->SetPrecipitationRateModel(
        usr::kPrecipitationRateModel::fractional_kr);
      break;
    }
  }
  int shear_model = std::stoi(reading_vec[13]);
  switch (shear_model) {
    case 0: {
      sim_option->SetShearDepositionModel(usr::kShearDepositionModel::none);
      break;
    }
    case 1: {
      sim_option->SetShearDepositionModel(
        usr::kShearDepositionModel::MaximumTolerableShearStressModel);
      sim_option->SetMSR1(std::stod(reading_vec[14]));
      sim_option->SetMSR2(std::stod(reading_vec[15]));
      break;
    }
  }
}

FittingDepositionCall ReadInput::ReadModelTuning() {
  FittingDepositionCall output;
  std::ifstream ifs;
  std::wstring ifname = p_path_.output_path_ + L"inputs\\default_Dep1L_.MTune";
  std::vector<std::string> reading_vec;
  std::vector<std::vector<std::string>> data_string_raw;
  std::vector<std::vector<double>> time_delta_2column;
    //two column data: time[s], thickness [m]
  std::vector<std::vector<double>> time_fw_2column;
    //two column data: time[s], fw [-]
  std::vector<std::vector<double>> data_for_tuning;

  ifs.open(ifname);
  if (ifs) {
    while (!ifs.eof()) {
      std::string reading;
      std::getline(ifs, reading);
      if (reading != "") {
        reading_vec.push_back(reading);
      }
    }
  }
  else {
    std::wcout << L"cannot open " << ifname << L"\n";
  }
  ifs.close();
  std::vector<bool> fitting_boolean;
  for (int i = 0; i < 13; i++) {
    if (reading_vec[i] == "true") {
      fitting_boolean.push_back(true);
    }
    else {
      fitting_boolean.push_back(false);
    }
  }
  output.SetFittingBool(fitting_boolean);
  if (reading_vec[13] == "true") {
    output.SetUseDeltaDataForSearching(true);
  }
  else {
    output.SetUseDeltaDataForSearching(false);
  }
  
  if (reading_vec[14] == "true") {
    output.SetUseFwDataForSearching(true);
  }
  else {
    output.SetUseFwDataForSearching(false);
  }
  output.SetRelativeEpsForDerivative(std::stod(reading_vec[15]));
  output.SetRelativeEpsForStopping(std::stod(reading_vec[16]));
  output.SetInitialMuLM(std::stod(reading_vec[17]));
  output.SetMaxLMIteration(std::stod(reading_vec[18]));
  for (int i = 19; i < reading_vec.size(); i++) {
    data_string_raw.push_back(usr::split(reading_vec[i],","));
  }

  bool end_of_delta_data = false;
  bool end_of_fw_data = false;

  for (int i = 0; i < data_string_raw.size(); i++) {
    double time_s_delta, delta_m, time_s_fw, fw;
    if (data_string_raw[i][0] != "") {
      time_s_delta = std::stod(data_string_raw[i][0]) * 3600.0;
    }
    else {
      end_of_delta_data = true;
    }
    if (data_string_raw[i][1] != "") {
      delta_m = std::stod(data_string_raw[i][1]) / 1000.0;
    }
    else {
      end_of_delta_data = true;
    }
    if (data_string_raw[i][2] != "") {
      time_s_fw = std::stod(data_string_raw[i][2]) * 3600.0;
    }
    else {
      end_of_fw_data = true;
    }
    if (data_string_raw[i][3] != "") {
      fw = std::stod(data_string_raw[i][3]);
    }
    else {
      end_of_fw_data = true;
    }
    if (!end_of_delta_data) {
      time_delta_2column.push_back({time_s_delta,delta_m});
    }
    if (!end_of_fw_data) {
      time_fw_2column.push_back({time_s_fw,fw});
    }
  }

  if (output.GetUseDeltaDataForSearching()) {
    data_for_tuning = time_delta_2column;
    output.SetNDeltaData(time_delta_2column.size());
    if (output.GetUseFwDataForSearching()) { //case: use both
      data_for_tuning.insert(data_for_tuning.end(),
        time_fw_2column.begin(), time_fw_2column.end());
      output.SetData(data_for_tuning);
      output.SetNFwData(time_fw_2column.size());
    }
    else { //use delta but don't use fw case
      output.SetData(data_for_tuning);
      output.SetNFwData(0);
    }
  }
  else { //not using delta data for searching
    output.SetNDeltaData(0);
    if (output.GetUseFwDataForSearching()) {
      data_for_tuning = time_fw_2column;
      output.SetData(data_for_tuning);
      output.SetNFwData(time_fw_2column.size());
    }
    else {
      output.SetDoSearching(false);
      output.SetNDeltaData(0);
      output.SetNFwData(0);
    }
  }
  if (!time_delta_2column.empty()) {
    output.SetGraphPlottingDeltaData(time_delta_2column);
  }
  if (!time_fw_2column.empty()) {
    output.SetGraphPlottingFwData(time_fw_2column);
  }
  return output;
}

void ReadInput::SetPath(ProgramPath p_path) {
  p_path_ = p_path;
}

void Main1PhaseFittingSearch::Run(ProgramPath p_path) {
  //create input for deposition object
  ReadInput read_input;
  read_input.SetPath(p_path);
  Fluid oil = read_input.ReadLiquid1P();
  Solid pipe_material;
  PipeGeometry pipe;
  read_input.ReadPipe1P(&pipe, &pipe_material);
  //Read pipe properties set the default value for
  //lenght from the inlet that the jacket start
  //This has to be called before ReadSurrounding
  InletCondition inlet = read_input.ReadInlet1P();
  CoolantCondition coolant_inlet;
  Fluid coolant_fluid;
  SimulationOptions sim_option;
  read_input.ReadCoolant1P(&coolant_inlet, &coolant_fluid, &pipe, &sim_option);
  read_input.ReadSimulationOption(&sim_option, coolant_inlet);
  //make wax material properties
  Solid wax;
  wax.SetConstantProperties("wax", -950, -2000, 0.25);
  sim_option.SetWaxMaterialProperties(wax);
  SinglePhaseDeposition deposition1;
//for debugging
//  sim_option.SetDepositionDuration(3600*1.0);

  deposition1.SetAll(sim_option,
    oil, coolant_fluid, inlet, coolant_inlet, pipe,
    pipe_material);
  deposition1.SetProgramPath(p_path);
  std::experimental::filesystem::remove_all(p_path.output_path_ + L"profile_data_pic\\");
  std::experimental::filesystem::remove_all(p_path.output_path_ + L"profile_data\\");
  std::experimental::filesystem::remove_all(p_path.output_path_ + L"data_1d\\");
  std::experimental::filesystem::remove_all(p_path.output_path_ + L"data_ave\\");
  FittingDepositionCall fitting_call;
  fitting_call = read_input.ReadModelTuning();
  fitting_call.SetDeposition(deposition1);
  fitting_call.SetSimulationOption(sim_option);
  fitting_call.SetUseLogSpace(false);
  fitting_call.Initialization();
  //finish setting up fitting_call
  usr::LMObject lm_deposition1;
  auto bound_deposition_fn = std::bind(
    &FittingDepositionCall::WrapDepositionCall,
    &fitting_call, std::placeholders::_1);
  lm_deposition1.SetFunction(bound_deposition_fn);
  lm_deposition1.SetDataForFitting(fitting_call.GetData());
  lm_deposition1.SetScalingVector(fitting_call.GetScalingVector());
  lm_deposition1.initial_guess_ = fitting_call.GetInitialGuess();
  lm_deposition1.SetDefaultEchoCallBack();
  std::vector<double> ans_lm;


  lm_deposition1.SetEpsilonForDerivative(fitting_call.GetRelativeEpsForDerivative());
  lm_deposition1.SetEpsilonForStop(fitting_call.GetRelativeEpsForStopping());
  lm_deposition1.SetMuLMInitial(fitting_call.GetInitialMuLM());
  lm_deposition1.SetMaxIteration(fitting_call.GetMaxLMIteration());
  lm_deposition1.SetOutputPath(p_path.output_path_);
  ans_lm = lm_deposition1.RunLM(p_path.output_path_);
  fitting_call.WrapDepositionCall(ans_lm);
  SinglePhaseDeposition deposition_after_lm;
  std::vector<std::vector<double>> delta_data_s_m = fitting_call.GetGraphPlottingDeltaData();
  std::vector<std::vector<double>> fw_data_s_f = fitting_call.GetGraphPlottingFwData();
  deposition_after_lm = fitting_call.GetDeposition();
  deposition_after_lm.PrintStandardOutput(delta_data_s_m, fw_data_s_f);
  deposition_after_lm.DetailedPostCalculation();
  deposition_after_lm.PrintDetailedOutput();
  lm_deposition1.PrintStandardOutput(
    fitting_call.GetFittingBoolName(),
    fitting_call.GetFittingBoolIndex());
  std::cout << "\n";
}
} //namespace spdepo