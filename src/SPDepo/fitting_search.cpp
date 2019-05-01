#include "fitting_search.h"
#include "fitting_search_constant.h"
#include "user_numerical_method.h"
#include <algorithm> //for std::count
namespace spdepo {


  std::vector<double> FittingDepositionCall::WrapDepositionCall(
      std::vector<double> w) {
      //w is the weight vector (vector of the fitting parameters)
    std::vector<double> delta_m;
    std::vector<double> delta_m_interpolated;
    std::vector<double> fw;
    std::vector<double> fw_interpolated;
    std::vector<double> t;
    std::vector<double> t_delta_data;
    std::vector<double> t_fw_data;
    int n_time_sim;
    int n_w = w.size();
    for (int i = 0; i < n_w; i++) {
      fn_params_adj_vec_[i](w[i]);
    }
    deposition_ = deposition_original_;
      //t_z_profile_.ZDiscretization(); cannot be done twice
      //deposition_ needs to be reset after every run.
    deposition_.sim_dep_options_ = sim_op_;
    deposition_.Calculate();
    deposition_.SimplePostCalculation();
    t = deposition_.ans_t_; //time in second
    n_time_sim = deposition_.delta_mm_vol_ave_.size();
    for (int i = 0; i < n_time_sim; i++) {
      delta_m.push_back(
        deposition_.delta_mm_vol_ave_[i][1] / 1000.0);
      fw.push_back(deposition_.fw_vol_ave_[i][1]);
    }
    //create t_delta_data
    for (int i = 0; i < n_delta_data_; i++) {
      t_delta_data.push_back(data_[i][0]);
    }
    //create t_delta_data
    for (int i = 0; i < n_fw_data_; i++) {
      t_fw_data.push_back(data_[n_delta_data_+i][0]);
    }
    //Get delta [m] output that match to data's time
    delta_m_interpolated = usr::Interpolation(
      t_delta_data, t, delta_m);
    fw_interpolated = usr::Interpolation(
      t_fw_data, t, fw);
    delta_m_interpolated.insert(delta_m_interpolated.end(),
      fw_interpolated.begin(), fw_interpolated.end());
    //the return vector contain both delta and fw
    return delta_m_interpolated;
  }
  void FittingDepositionCall::SetData(
      const std::vector<std::vector<double>>& data) {
    data_ = data;
  }
  std::vector<std::vector<double>> FittingDepositionCall::GetData() const{
    return data_;
  }
  void FittingDepositionCall::SetNDeltaData(int n) {
    n_delta_data_ = n;
  }
  void FittingDepositionCall::SetNFwData(int n) {
    n_fw_data_ = n;
  }
  void FittingDepositionCall::SetDeposition(
      SinglePhaseDeposition deposition) {
    deposition_ = deposition;
    deposition_original_ = deposition;
  }
  SinglePhaseDeposition FittingDepositionCall::GetDeposition() const{
    return deposition_;
  }
  void FittingDepositionCall::SetSimulationOption(
      SimulationOptions sim_op) {
    sim_op_ = sim_op;
  }
  void FittingDepositionCall::SetFittingBool(
      std::vector<bool> fitting_bool) {
    fitting_bool_ = fitting_bool;
    n_parameters_ = std::count(fitting_bool.begin(),
                               fitting_bool.end(),
                               true);
  }
  void FittingDepositionCall::Initialization() {
    int n_total = fitting_bool_.size();
    fn_params_adj_vec_.clear();
    for (int i = 0; i < n_total; i++) {
      if (fitting_bool_[i]) {
        fitting_bool_index_.push_back(i);
        fitting_bool_name_.push_back(usr::kfitting_bool_to_var_name.at(i));
        switch (i) {
          case 0: {
            auto fn = std::bind(
              &FittingDepositionCall::fn_k_alpha_, this,
                std::placeholders::_1);
            fn_params_adj_vec_.push_back(fn);
            auto fn2 = std::bind(
              &FittingDepositionCall::get_fn_k_alpha_, this);
            get_fn_params_adj_vec_.push_back(fn2);
            break;
          }
          case 1: {
            auto fn = std::bind(
              &FittingDepositionCall::fn_a_alpha_, this,
                std::placeholders::_1);
            fn_params_adj_vec_.push_back(fn);
            auto fn2 = std::bind(
              &FittingDepositionCall::get_fn_a_alpha_, this);
            get_fn_params_adj_vec_.push_back(fn2);
            break;
          }
          case 2: {
            auto fn = std::bind(
              &FittingDepositionCall::fn_k_r_constant_, this,
                std::placeholders::_1);
            fn_params_adj_vec_.push_back(fn);
            auto fn2 = std::bind(
              &FittingDepositionCall::get_fn_k_r_constant_, this);
            get_fn_params_adj_vec_.push_back(fn2);
            break;
          }
          case 3: {
            auto fn = std::bind(
              &FittingDepositionCall::fn_k_r_t_dependent_, this,
                std::placeholders::_1);
            fn_params_adj_vec_.push_back(fn);
            auto fn2 = std::bind(
              &FittingDepositionCall::get_fn_k_r_t_dependent_, this);
            get_fn_params_adj_vec_.push_back(fn2);
            break;
          }
          case 4: {
            auto fn = std::bind(
              &FittingDepositionCall::fn_frac_k_r_, this,
                std::placeholders::_1);
            fn_params_adj_vec_.push_back(fn);
            auto fn2 = std::bind(
              &FittingDepositionCall::get_fn_frac_k_r_, this);
            get_fn_params_adj_vec_.push_back(fn2);
            break;
          }
          case 5: {
            auto fn = std::bind(
              &FittingDepositionCall::fn_msr1_, this,
                std::placeholders::_1);
            fn_params_adj_vec_.push_back(fn);
            auto fn2 = std::bind(
              &FittingDepositionCall::get_fn_msr1_, this);
            get_fn_params_adj_vec_.push_back(fn2);
            break;
          }
          case 6: {
            auto fn = std::bind(
              &FittingDepositionCall::fn_msr2_, this,
                std::placeholders::_1);
            fn_params_adj_vec_.push_back(fn);
            auto fn2 = std::bind(
              &FittingDepositionCall::get_fn_msr2_, this);
            get_fn_params_adj_vec_.push_back(fn2);
            break;
          }
          case 7: {
            auto fn = std::bind(
              &FittingDepositionCall::fn_usr_coeff0_, this,
              std::placeholders::_1);
            fn_params_adj_vec_.push_back(fn);
            auto fn2 = std::bind(
              &FittingDepositionCall::get_fn_usr_coeff0_, this);
            get_fn_params_adj_vec_.push_back(fn2);
            break;
          }
          case 8: {
            auto fn = std::bind(
              &FittingDepositionCall::fn_usr_coeff1_, this,
              std::placeholders::_1);
            fn_params_adj_vec_.push_back(fn);
            auto fn2 = std::bind(
              &FittingDepositionCall::get_fn_usr_coeff1_, this);
            get_fn_params_adj_vec_.push_back(fn2);
            break;
          }
          case 9: {
            auto fn = std::bind(
              &FittingDepositionCall::fn_usr_coeff2_, this,
              std::placeholders::_1);
            fn_params_adj_vec_.push_back(fn);
            auto fn2 = std::bind(
              &FittingDepositionCall::get_fn_usr_coeff2_, this);
            get_fn_params_adj_vec_.push_back(fn2);
            break;
          }
          case 10: {
            auto fn = std::bind(
              &FittingDepositionCall::fn_usr_coeff3_, this,
              std::placeholders::_1);
            fn_params_adj_vec_.push_back(fn);
            auto fn2 = std::bind(
              &FittingDepositionCall::get_fn_usr_coeff3_, this);
            get_fn_params_adj_vec_.push_back(fn2);
            break;
          }
          case 11: {
            auto fn = std::bind(
              &FittingDepositionCall::fn_usr_coeff4_, this,
              std::placeholders::_1);
            fn_params_adj_vec_.push_back(fn);
            auto fn2 = std::bind(
              &FittingDepositionCall::get_fn_usr_coeff4_, this);
            get_fn_params_adj_vec_.push_back(fn2);
            break;
          }
          case 12: {
            auto fn = std::bind(
              &FittingDepositionCall::fn_usr_coeff5_, this,
              std::placeholders::_1);
            fn_params_adj_vec_.push_back(fn);
            auto fn2 = std::bind(
              &FittingDepositionCall::get_fn_usr_coeff5_, this);
            get_fn_params_adj_vec_.push_back(fn2);
            break;
          }
        }
      }
    }
  }
  void FittingDepositionCall::fn_k_alpha_(double x) { //bool0
    sim_op_.SetAlphaEquation(sim_op_.GetAlphaOption(), x, 0.0, 0.0);
  }
  double FittingDepositionCall::get_fn_k_alpha_() const {
    usr::BigEquation big_eq;
    usr::Equation eq;
    big_eq = sim_op_.GetAlphaEq();
    eq = big_eq.GetEquationVector()[0];
    return eq.GetCoefficient()[0];
  }
  void FittingDepositionCall::fn_a_alpha_(double x) { //bool1
    sim_op_.SetAlphaEquation(sim_op_.GetAlphaOption(), 0.0, x, 0.0);
  }
  double FittingDepositionCall::get_fn_a_alpha_() const {
    usr::BigEquation big_eq;
    usr::Equation eq;
    big_eq = sim_op_.GetAlphaEq();
    eq = big_eq.GetEquationVector()[0];
    return 1.0 / (eq.GetCoefficient()[0]);
  }
  void FittingDepositionCall::fn_k_r_constant_(double x) { //bool2
    sim_op_.SetPrecipitationRate(x);
  }
  double FittingDepositionCall::get_fn_k_r_constant_() const {
    return sim_op_.GetPrecipitationRate();
  }
  void FittingDepositionCall::fn_k_r_t_dependent_(double x) { //bool3
    sim_op_.SetPrecipitationRate(x);
  }
  double FittingDepositionCall::get_fn_k_r_t_dependent_() const {
    return sim_op_.GetPrecipitationRate();
  }
  void FittingDepositionCall::fn_frac_k_r_(double x) { //bool4
    //not implemented yet
  }
  double FittingDepositionCall::get_fn_frac_k_r_() const {
    return 42; //not implemented yet
  }
  void FittingDepositionCall::fn_msr1_(double x) { //bool5
    sim_op_.SetMSR1(x);
    if (use_log_space_) {
      sim_op_.SetMSR1(std::pow(10.0,x));
    }
  }
  double FittingDepositionCall::get_fn_msr1_() const { //bool5
    if (use_log_space_) {
      return std::log10(sim_op_.GetMSR1());
    } else {
      return sim_op_.GetMSR1();
    }
  }
  void FittingDepositionCall::fn_msr2_(double x) { //bool6
    sim_op_.SetMSR2(x);
    if (use_log_space_) {
      sim_op_.SetMSR2(std::pow(10.0, x));
    }
  }
  double FittingDepositionCall::get_fn_msr2_() const { //bool6
    if (use_log_space_) {
      return std::log10(sim_op_.GetMSR2());
    } else {
      return sim_op_.GetMSR2();
    }
  }

  void FittingDepositionCall::fn_usr_coeff0_(double x) { //bool7
    //not implemented yet
  }
  double FittingDepositionCall::get_fn_usr_coeff0_() const { //bool7
    return 42; //not implemented yet
  }
  void FittingDepositionCall::fn_usr_coeff1_(double x) { //bool8
    //not implemented yet
  }
  double FittingDepositionCall::get_fn_usr_coeff1_() const { //bool8
    return 42; //not implemented yet
  }
  void FittingDepositionCall::fn_usr_coeff2_(double x) { //bool9
    //not implemented yet
  }
  double FittingDepositionCall::get_fn_usr_coeff2_() const { //bool9
    return 42; //not implemented yet
  }
  void FittingDepositionCall::fn_usr_coeff3_(double x) { //bool10
    //not implemented yet
  }
  double FittingDepositionCall::get_fn_usr_coeff3_() const { //bool10
    return 42; //not implemented yet
  }
  void FittingDepositionCall::fn_usr_coeff4_(double x) { //bool11
    //not implemented yet
  }
  double FittingDepositionCall::get_fn_usr_coeff4_() const { //bool11
    return 42; //not implemented yet
  }
  void FittingDepositionCall::fn_usr_coeff5_(double x) { //bool12
    //not implemented yet
  }
  double FittingDepositionCall::get_fn_usr_coeff5_() const { //bool12
    return 42; //not implemented yet
  }
  void FittingDepositionCall::SetUseLogSpace(bool use_log_space) {
    use_log_space_ = use_log_space;
  }
  void FittingDepositionCall::SetGraphPlottingDeltaData(
      std::vector<std::vector<double>> data) {
    graph_data_delta_s_m_ = data;
  }
  std::vector<std::vector<double>> 
      FittingDepositionCall::GetGraphPlottingDeltaData() {
    return graph_data_delta_s_m_;
  }
  void FittingDepositionCall::SetGraphPlottingFwData(
    std::vector<std::vector<double>> data) {
    graph_data_fw_s_f_ = data;
  }
  std::vector<std::vector<double>> 
      FittingDepositionCall::GetGraphPlottingFwData() {
    return graph_data_fw_s_f_;
  }
  void FittingDepositionCall::SetUseFwDataForSearching(bool input) {
    use_fw_data_for_searching_ = input;
  }
  bool FittingDepositionCall::GetUseFwDataForSearching() const{
    return use_fw_data_for_searching_;
  }
  void FittingDepositionCall::SetUseDeltaDataForSearching(bool input) {
    use_delta_data_for_searching_ = input;
  }
  bool FittingDepositionCall::GetUseDeltaDataForSearching() const{
    return use_delta_data_for_searching_;
  }
  void FittingDepositionCall::SetDoSearching(bool input) {
    do_searching_ = input;
  }
  bool FittingDepositionCall::GetDoSearching() const{
    return do_searching_;
  }
  void FittingDepositionCall::SetRelativeEpsForDerivative(double re_eps_d) {
    re_eps_d_ = re_eps_d;
  }
  double FittingDepositionCall::GetRelativeEpsForDerivative() const {
    return re_eps_d_;
  }
  void FittingDepositionCall::SetRelativeEpsForStopping(double re_eps_stop) {
    re_eps_stop_ = re_eps_stop;
  }
  double FittingDepositionCall::GetRelativeEpsForStopping() const {
    return re_eps_stop_;
  }
  void FittingDepositionCall::SetInitialMuLM(double mu_lm_ini) {
    mu_lm_ini_ = mu_lm_ini;
  }
  double FittingDepositionCall::GetInitialMuLM() const {
    return mu_lm_ini_;
  }
  void FittingDepositionCall::SetMaxLMIteration(int max_lm_iteration) {
    max_lm_iteration_ = max_lm_iteration;
  }
  int FittingDepositionCall::GetMaxLMIteration() const {
    return max_lm_iteration_;
  }
  std::vector<double> FittingDepositionCall::GetScalingVector() const {
    enum class kLocalOption {use_just_delta, use_just_fw, use_both, use_none};
    kLocalOption option;
    double delta_av;
    double fw_av;
    std::vector<double> delta_vec;
    std::vector<double> fw_vec;
    std::vector<double> output;
    if (use_delta_data_for_searching_) {
      if (use_fw_data_for_searching_) {
        option = kLocalOption::use_both;
      }
      else {
        option = kLocalOption::use_just_delta;
      }
    }
    else {
      if (use_fw_data_for_searching_) {
        option = kLocalOption::use_just_fw;
      }
      else {
        option = kLocalOption::use_none;
      }
    }
    switch (option) {
      case kLocalOption::use_both: {
        delta_av = usr::AverageSecondColumn(graph_data_delta_s_m_);
        fw_av = usr::AverageSecondColumn(graph_data_fw_s_f_);
        output.assign(graph_data_delta_s_m_.size(), delta_av);
        output.insert(output.end(),graph_data_fw_s_f_.size(), fw_av);
        break;
      }
      case kLocalOption::use_just_delta: {
        delta_av = usr::AverageSecondColumn(graph_data_delta_s_m_);
        output.assign(graph_data_delta_s_m_.size(), delta_av);
        break;
      }
      case kLocalOption::use_just_fw: {
        fw_av = usr::AverageSecondColumn(graph_data_fw_s_f_);
        output.assign(graph_data_fw_s_f_.size(), fw_av);
        break;
      }
      case kLocalOption::use_none: {
        break;
      }
    }
    return output;
  }

  std::vector<double> FittingDepositionCall::GetInitialGuess() const {
    std::vector<double> ans(get_fn_params_adj_vec_.size(),0.0);
    for (int i = 0; i < ans.size(); i++) {
      ans[i] = get_fn_params_adj_vec_[i]();
    }
    return ans;
  }

  std::vector<int> FittingDepositionCall::GetFittingBoolIndex() {
    return fitting_bool_index_;
  }
  std::vector<std::string> FittingDepositionCall::GetFittingBoolName() {
    return fitting_bool_name_;
  }

  void SearchFunctionWrap::SetFunctionToBeSearched(
    std::function<double(double)> fn) {
    fn_ = fn;
  }
  void SearchFunctionWrap::SetErrorVectorCalculation(
      std::function<std::vector<double>(
        std::vector<double>)> fn_error) {
    fn_error_ = fn_error;
  }
}

