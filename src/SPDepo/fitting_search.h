#ifndef FITTING_SEARCH_H_
#define FINTING_SEARCH_H_

#include "main_caller.h"
#include "single_phase_deposition.h"
#include "user_short_name.h"
#include <functional>
namespace spdepo{
class FittingDepositionCall {
public:
  //Take vector of fitting parameters
  //Return prediction of delta and fw combined into 1 vector
  //the size of the vector equals to the size of data
  //  the data is 2 column
  //  (time in the 1st col and delta/fw in 2nd col)
  std::vector<double> WrapDepositionCall(std::vector<double>);
  void SetData(const std::vector<std::vector<double>>&);
  std::vector<std::vector<double>> GetData() const;
  void SetNDeltaData(int number_of_delta_data);
  void SetNFwData(int number_of_fw_data);
  void SetDeposition(SinglePhaseDeposition deposition);
  void SetGraphPlottingDeltaData(std::vector<std::vector<double>>);
  std::vector<std::vector<double>> GetGraphPlottingDeltaData();
    //2 column: time[s], delta[m]
    //These data are just for graph plotting.
    //They may not be used in the search
  void SetGraphPlottingFwData(std::vector<std::vector<double>>);
  std::vector<std::vector<double>> GetGraphPlottingFwData();
    //2 column: time[s], delta[m]
    //These data are just for graph plotting.
    //They may not be used in the search
  void SetUseFwDataForSearching(bool);
  bool GetUseFwDataForSearching() const;
  void SetUseDeltaDataForSearching(bool);
  bool GetUseDeltaDataForSearching() const;
  void SetDoSearching(bool);
  bool GetDoSearching() const;
  SinglePhaseDeposition GetDeposition() const;
  void SetSimulationOption(SimulationOptions sim_op);
  void SetFittingBool(std::vector<bool> fitting_bool);
  void SetUseLogSpace(bool use_log_space);
  void Initialization();
    //need to be called after all parameters are set.
  //Set-Get for LMObject
  void SetRelativeEpsForDerivative(double re_eps_d);
  double GetRelativeEpsForDerivative() const;
  void SetRelativeEpsForStopping(double re_eps_stop);
  double GetRelativeEpsForStopping() const;
  void SetInitialMuLM(double mu_lm_ini);
  double GetInitialMuLM() const;
  void SetMaxLMIteration(int max_lm_iteration);
  int GetMaxLMIteration() const;
  //Get for LMObject (call after the read model tuning)
  std::vector<double> GetScalingVector() const;
  std::vector<double> GetInitialGuess() const;
  //vec[i] map to fitting_bool index
  std::vector<int> GetFittingBoolIndex();
  //use to get the name of the weight function based on the index of w
  //  itself.
  std::vector<std::string> GetFittingBoolName();
private:
  std::vector<std::vector<double>> data_;
  SinglePhaseDeposition deposition_;
  SinglePhaseDeposition deposition_original_;
  SimulationOptions sim_op_;
  //store the boolean to indicate the parameters that are used in LM
  std::vector<bool> fitting_bool_;
    //bool0 = k_alpha
    //bool1 = A_alpha
    //bool2 = k_r constant
    //bool3 = k_r temperature dependent
    //bool4 = fk_r fractional precipitation rate
    //bool5 = msr1
    //bool6 = msr2
    //bool7 = usr_alpha_coeff0
    //bool8 = usr_alpha_coeff1
    //bool9 = usr_alpha_coeff2
    //bool10 = usr_alpha_coeff3
    //bool11 = usr_alpha_coeff4
    //bool12 = usr_alpha_coeff5
  //store index that is true
  //this is to know the type of w
  //  from the index of w
  std::vector<int> fitting_bool_index_;
  std::vector<std::string> fitting_bool_name_;
  //map the index of the fitting_bool_ to the index of
  // the weight (fn_params_adj_vec_)
  int n_parameters_; //number of parameters that are to be used in LM
  int n_delta_data_;
  int n_fw_data_;
  //variable to store the parameters to be adjusted
  //When call the member function, it do the setup for sim_op_
  std::vector<std::function<void(double)>> fn_params_adj_vec_;
  //this is the counter part of fn_params_adj_vec_
  //When call the vector member (function) it return the value of each w
  std::vector<std::function<double()>> get_fn_params_adj_vec_;
  //function wrapper for each boolean case
  void fn_k_alpha_(double); //controled by bool0
  double get_fn_k_alpha_() const;
  void fn_a_alpha_(double); //controled by bool1
  double get_fn_a_alpha_() const;
  void fn_k_r_constant_(double); //controled by bool2
  double get_fn_k_r_constant_() const;
  void fn_k_r_t_dependent_(double); //controled by bool3
  double get_fn_k_r_t_dependent_() const;
  void fn_frac_k_r_(double); //controled by bool4
  double get_fn_frac_k_r_() const;
  void fn_msr1_(double); //controled by bool5
  double get_fn_msr1_() const;
  void fn_msr2_(double); //controled by bool6
  double get_fn_msr2_() const;
  void fn_usr_coeff0_(double); //controled by bool7
  double get_fn_usr_coeff0_() const;
  void fn_usr_coeff1_(double); //controled by bool8
  double get_fn_usr_coeff1_() const;
  void fn_usr_coeff2_(double); //controled by bool9
  double get_fn_usr_coeff2_() const;
  void fn_usr_coeff3_(double); //controled by bool10
  double get_fn_usr_coeff3_() const;
  void fn_usr_coeff4_(double); //controled by bool11
  double get_fn_usr_coeff4_() const;
  void fn_usr_coeff5_(double); //controled by bool12
  double get_fn_usr_coeff5_() const;
  //search in a logarithmic scale
  bool use_log_space_ = false;
  //Control to use data in searching
  bool use_fw_data_for_searching_ = true;
  bool use_delta_data_for_searching_ = true;
  //Data for graph plotting
  std::vector<std::vector<double>> graph_data_delta_s_m_;
    //2 column time[s], delta[m]
  std::vector<std::vector<double>> graph_data_fw_s_f_;
    //2 column time[s], fw[fraction: - ]
  //do searching or not
  bool do_searching_ = true;
  //LM Options
  double re_eps_d_ = 0.0001;
  double re_eps_stop_ = 1.0e-6;
  double mu_lm_ini_ = 1.0;
  int max_lm_iteration_ = 50;
};

class SearchFunctionWrap {
 public:
  usr::TwoDVector<double> data_;
  void SetFunctionToBeSearched(
    std::function<double(double)> fn);
  void SetErrorVectorCalculation(
    std::function<std::vector<double>(
      std::vector<double>)> fn_error_);
  std::function<double(double)> fn_;
  std::function<std::vector<double>(
    std::vector<double> fn_error)> fn_error_;
};
} //namespace spdepo

#endif