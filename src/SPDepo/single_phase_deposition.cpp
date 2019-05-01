#include "coolant_condition.h"
#include "deposit_properties.h"
#include "fluid_properties.h"
#include "inlet_condition.h"
#include "single_phase_deposition.h"
#include "solid_properties.h"
#include "pipe_geometry.h"
#include "user_equation_format.h"
#include "user_flow_enum.h"
#include "user_numerical_method.h"
#include "user_math_const.h"
#include "user_python_script.h"
#include "user_string_operation.h"
#include "check_plotting.h"
#include <string>
#include <chrono>
#include <filesystem> //for remove all
namespace spdepo {
  CoreDepositionEquation* ptr_wrap;
  std::vector<double> WrapSolve1P(double t, 
      const std::vector<double>& ri_fw_vec_vec) {
    //ri_fw_vec_vec = {ri[0], ..., ri[N], fw[0], ...,fw[N]}
    int n_zsec = ptr_wrap->n_zsec_;
    std::vector<double> ri(ri_fw_vec_vec.begin(),
                           ri_fw_vec_vec.begin() + n_zsec);
    std::vector<double> fw(ri_fw_vec_vec.begin() + n_zsec,
                           ri_fw_vec_vec.end());
    return ptr_wrap->Solve1P(ri,fw);
  }

  void SinglePhaseDeposition::SetAll(
    const SimulationOptions& sim_dep_options,
    const Fluid& oil,
    const Fluid& c_fluid,
    const InletCondition& inlet,
    const CoolantCondition& coolant_inlet,
    const PipeGeometry& pipe,
    const Solid& pipe_material) {
    sim_dep_options_ = sim_dep_options;
    oil_ = oil;
    c_fluid_ = c_fluid;
    inlet_ = inlet;
    coolant_inlet_ = coolant_inlet;
    pipe_ = pipe;
    pipe_material_ = pipe_material;
    wax_ = sim_dep_options_.GetWaxMaterialProperties();
    dCdT_eq_ = sim_dep_options_.GetUserDefinededdCdTEquation();
    precipitation_eq_ = 
      sim_dep_options_.GetUserDefinedPrecipitationEquation();
    wax_molar_volume_cm3_per_gmol_ = 
      sim_dep_options_.GetUserWaxMolarVolume();
    initial_total_oil_volume_ = 
      sim_dep_options_.GetInitialTotalOilVolume();

    r_ = pipe_.GetProperties(usr::kGeometry::d) * 0.5;
    //cannot get n_rmesh_ right up front,
    // because the single-phase hydrodynamics function has not been called yet
  }
  void SinglePhaseDeposition::Initialize() {
    usr::BigEquation k_oil;
    usr::BigEquation k_wax;
    double coolant_inlet_temperature; //[C]
    double rho_oil_at_c_inletT_ini; //[kg/m3]
      //oil density at coolant inlet temperature at t = 0 sec
    double rho_oil_at_h_inletT_ini; //[kg/m3]
      //oil density at oil inlet temperature at t = 0 sec
    double deposit_roughness = 0.0;
    double initial_fw;
    std::vector<double> mass_vec;
    double mass_local;
    double ri_local; //pipe radius - initial thickness
    double rho_deposit;
    pipe_radius_ = pipe_.GetProperties(usr::kGeometry::d) / 2.0;
    h_flow_.SetAll(oil_, pipe_,
                    inlet_.GetProperties(usr::kTwoPhaseVar::v_sl),
                    inlet_);
    Th0_inlet_ = inlet_.GetProperties(
      usr::kTwoPhaseVar::inlet_temperature);
  
    coolant_inlet_temperature = coolant_inlet_.GetDouble(
                          usr::kCoolant::inlet_temperature);

    rho_oil_at_c_inletT_ini = oil_.GetProperty(usr::kFluid::rho, coolant_inlet_temperature);
    rho_oil_at_h_inletT_ini = oil_.GetProperty(usr::kFluid::rho, Th0_inlet_);
    rho_deposit = rho_oil_at_h_inletT_ini;
    initial_h_solid_fraction_ = precipitation_eq_.At(Th0_inlet_);
    initial_c_solid_fraction_ = precipitation_eq_.At(coolant_inlet_temperature);
    wax_prop_.SetAll(precipitation_eq_,
                     dCdT_eq_,
                     oil_,
                     coolant_inlet_temperature,
                     wax_molar_volume_cm3_per_gmol_);
    initial_wax_concentration_ = rho_oil_at_c_inletT_ini * initial_c_solid_fraction_ - 
                                 rho_oil_at_h_inletT_ini * initial_h_solid_fraction_;
  

    h_flow_.DiscretizeR(Th0_inlet_, 1.0e-6);
    h_flow_.Solve(Th0_inlet_, 1.0e-6, deposit_roughness);

    t_z_profile_.SetAll(h_flow_, coolant_inlet_, c_fluid_, pipe_material_);
    t_z_profile_.ZDiscretization();
    k_wax = wax_.GetEquation(usr::kSolid::k);
    k_oil = oil_.GetEquation(usr::kFluid::k);
    n_zsec_ = t_z_profile_.GetSecSize();
    n_zmesh_ = n_zsec_ + 1;
    deposit_profile_.Initialize(n_zsec_, k_oil, k_wax, oil_.GetEquation(usr::kFluid::rho), pipe_radius_);
    initial_fw = precipitation_eq_.At(Th0_inlet_);
    if (sim_dep_options_.GetAlphaOption() == 
        usr::kDepositionOption::Lee2008_k_alpha_eq) {
      sim_dep_options_.SetAlphaEquationLee2008Form(initial_fw);
    }
    deposit_profile_.SetFwVec(std::vector<double>(n_zsec_,initial_fw));
    ri_local = pipe_radius_ - 1.0e-6;
    deposit_profile_.SetRiVec(
      std::vector<double>(n_zsec_, ri_local));

    for (int i = 0; i < n_zsec_; i++) {
      mass_local = (t_z_profile_.z_mesh_[i + 1] - t_z_profile_.z_mesh_[i]) *
        usr::pi * (std::pow(pipe_radius_, 2) - std::pow(ri_local,2)) * rho_deposit;
      mass_vec.push_back(mass_local);
    }
    deposit_profile_.SetMassVec(mass_vec);
    deposit_profile_.SetZMesh(t_z_profile_.z_mesh_);
    t_z_profile_.Solve(deposit_profile_);
      //when solve, the dep_profile global var of t_z_profile is updated
    sim_dep_options_.SetWaxProperties(wax_prop_);
    //for developing program purposes
    if (sim_dep_options_.GetKrAsAFunctionOfTemperatureOption()) {
      sim_dep_options_.SetTRefForKr(25.0);
      sim_dep_options_.SetDwoIniForKr();
    }
    for (int i = 0; i < n_zsec_; i++) {
      if (sim_dep_options_.GetShearDepositionModel() ==
        usr::kShearDepositionModel::MaximumTolerableShearStressModel) {
        //estimate the local shear stress
        SinglePhaseHydrodynamics hydro_local;
        hydro_local = t_z_profile_.h_hydro_;
        double t_bulk_local;
        t_bulk_local = t_z_profile_.th_sec_[i];
        tau0_sec_vec_.push_back(hydro_local.SolveOnlyShearStress(t_bulk_local, ri_local, 0.0));
      }
    }
  }
  void SinglePhaseDeposition::Calculate() {
    Initialize();
    std::vector<double> ri;
    std::vector<double> fw;
    std::vector<double> ri_fw;
    std::vector<double> debug_temp;
    std::vector<double> eps_vec;
    std::vector<double> eps_fw_vec;
    long num_fn_call;
    CoreDepositionEquation DepEq;

    ri = deposit_profile_.GetRiVec();
    fw = deposit_profile_.GetFwVec();

    DepEq.SetAll1Phase(*this);
    ptr_wrap = &DepEq;
    ri_fw = ri;
    eps_vec.assign(ri.size(), 1.0e-4);
    eps_fw_vec.assign(fw.size(), 1.0e-3);
    ri_fw.insert(ri_fw.end(),fw.begin(),fw.end());
    eps_vec.insert(eps_vec.end(),eps_fw_vec.begin(),eps_fw_vec.end());
    //Time-loop is already in DormandPrinceMany
    ans_ri_fw_.clear();
    usr::DormandPrinceMany(
      WrapSolve1P,
      ri_fw.size(),
      &num_fn_call,
      &ans_t_,
      &ans_ri_fw_,
      ri_fw,
      0.0,
      sim_dep_options_.GetDepositionDuration(),
      1,
      eps_vec,
      100,
      100,
      3,
      36000.0,
      0.1,true,false,false);
  }

  void SinglePhaseDeposition::PrintStandardOutput(
    std::vector<std::vector<double>> delta_data_s_m,
    std::vector<std::vector<double>> fw_data_s_f){
    std::ofstream ri_profile;
    std::ofstream delta_profile;
    std::ofstream fw_profile;
    std::ofstream ri_average;
    std::ofstream delta_average;
    std::ofstream fw_average;
    std::ofstream file_out;
    std::vector<std::wstring> file_name = 
      {p_path_.output_path_ + L"ri_profile.csv",
       p_path_.output_path_ + L"delta_mm_profile.csv",
       p_path_.output_path_ + L"fw_profile.csv",
       p_path_.output_path_ + L"ri_average.csv",
       p_path_.output_path_ + L"delta_mm_average.csv",
       p_path_.output_path_ + L"fw_average.csv",
       p_path_.output_path_ + L"t_hr_vec.csv",
       p_path_.output_path_ + L"z_sec_vec.csv",
       p_path_.output_path_ + L"z_mesh_vec.csv"};
    std::vector<usr::kDepositionPrintOption> print_option = {
      usr::kDepositionPrintOption::ri_profile,
      usr::kDepositionPrintOption::delta_mm_profile,
      usr::kDepositionPrintOption::fw_profile,
      usr::kDepositionPrintOption::ri_average,
      usr::kDepositionPrintOption::delta_mm_average,
      usr::kDepositionPrintOption::fw_average,
      usr::kDepositionPrintOption::t_hr_vec,
      usr::kDepositionPrintOption::z_sec_vec,
      usr::kDepositionPrintOption::z_mesh_vec
      };
    std::vector<usr::kDepositionPrintOption> print_picture = {
      usr::kDepositionPrintOption::ri_profile_picture,
      usr::kDepositionPrintOption::delta_profile_picture,
      usr::kDepositionPrintOption::fw_profile_picture
      };
    std::vector<std::vector<std::wstring>> print_xy = {
      {L"pyplot_2columns.py",
       p_path_.output_path_ + L"delta_mm_average.csv",
       p_path_.output_path_ + L"delta_mm_average.jpg",
       L"Average_Thickness[mm]",
       L"time[hr]",
       L"delta[mm]"},
      {L"pyplot_2columns.py",
       p_path_.output_path_ + L"fw_average.csv",
       p_path_.output_path_ + L"fw_average.jpg",
       L"Average_Wax_Fraction[w/w]",
       L"time[hr]",
       L"fw[w/w]"}
    };
    if (!delta_data_s_m.empty()) {
      std::wofstream wofs_local;
      wofs_local.open(p_path_.output_path_ + L"data_delta_hr_mm.csv");
      if (wofs_local.good()) {
        for (int i = 0; i < delta_data_s_m.size(); i++) {
          wofs_local << delta_data_s_m[i][0] / 3600.0 << L","
                     << delta_data_s_m[i][1] * 1000.0 << L"\n";
        }
      }
      wofs_local.close();
    }
    if (!fw_data_s_f.empty()) {
      std::wofstream wofs_local;
      wofs_local.open(p_path_.output_path_ + L"data_fw_hr_f.csv");
      if (wofs_local.good()) {
        for (int i = 0; i < fw_data_s_f.size(); i++) {
          wofs_local << fw_data_s_f[i][0] / 3600.0 << L","
                     << fw_data_s_f[i][1] << L"\n";
        }
      }
      wofs_local.close();
    }

    int n = file_name.size();
    size_t n_xy = print_xy.size();
    for (int i = 0; i < n; i++){
      file_out.open(file_name[i]);
      if (!(file_out)) {
        std::wcout << "cannot open " << file_name[i] << '\n';
        std::system("pause");
      }
      PrintParameters(file_out, print_option[i]);
      file_out.close();
    }
    //need to do the above output first
    n = print_picture.size();
    for (int i = 0; i < n; i++) {
      PrintPicture(print_picture[i]);
    }
    for (size_t i = 0; i < n_xy; i++) {
      PrintXY(print_xy);
    }
    //plot delta sim vs exp
    usr::PlotTwoSeries(
      p_path_.output_path_ + L"delta_mm_average.csv",
      p_path_.output_path_ + L"data_delta_hr_mm.csv",
      p_path_.output_path_ + L"sim_exp_delta_ave.jpg",
      L"average_deposit_thickness",
      L"time_[hr]",
      L"thickness_[mm]",
      L"simulation",
      L"experiment");
    usr::PlotTwoSeries(
      p_path_.output_path_ + L"fw_average.csv",
      p_path_.output_path_ + L"data_fw_hr_f.csv",
      p_path_.output_path_ + L"sim_exp_fw_ave.jpg",
      L"average_deposit_wax_fraction",
      L"time_[hr]",
      L"wax_fraction_[w/w]",
      L"simulation",
      L"experiment");
  }

  void SinglePhaseDeposition::PlotEveryCTProfileOneByOne() {
    //This function is to be called inside PrintDetailedOutput
    //It is interchangble with
    // PlotEveryCTProfile4Task()
    std::vector<double> ans_t_hr;
    for (std::size_t i = 0; i < ans_t_.size(); i++) {
      ans_t_hr.push_back(ans_t_[i] / 3600.0);
    }
    usr::PlotMultiplePColorGraph(
      L"t_tzr_mesh_post_",
      p_path_.output_path_ + L"profile_data\\",
      t_tzr_mesh_post_,
      L"r_ratio_mesh_post_.csv",
      L"z_mesh_vec.csv",
      L"T[C]",
      L"r/R[-]",
      L"z[m]",
      ans_t_hr);

    usr::PlotMultiplePColorGraph(
      L"c_tzr_mesh_post_",
      p_path_.output_path_ + L"profile_data\\",
      c_tzr_mesh_post_,
      L"r_ratio_mesh_post_.csv",
      L"z_mesh_vec.csv",
      L"C[kg/m3]",
      L"r/R[-]",
      L"z[m]",
      ans_t_hr);
  }

  void SinglePhaseDeposition::PlotEveryCTProfile4Tasks() {
    int total_pattern_per_profile = ans_t_.size();
    int start1, start2, end1, end2;
    int n_first_half;
    start1 = 0;
    end2 = total_pattern_per_profile - 1;
    n_first_half = total_pattern_per_profile / 2;
    end1 = n_first_half - 1;
    start2 = end1 + 1;
    try {
      std::uintmax_t temp1 = std::experimental::filesystem::remove_all(
        p_path_.output_path_ + L"profile_data_pic");
    }
    catch (const std::exception& ex) {
    }
    try {
      std::wstring temp_path = p_path_.output_path_ + L"profile_data_pic\\";
      LPCWSTR path_data_pic = temp_path.c_str();
      CreateDirectory(path_data_pic, NULL);
    }
    catch (const std::exception& ex) {
    }
    std::wofstream wofs;
    std::vector<std::wstring> fname = {
      p_path_.output_path_ + L"task1_multipcolor.csv",
      p_path_.output_path_ + L"task2_multipcolor.csv",
      p_path_.output_path_ + L"task3_multipcolor.csv",
      p_path_.output_path_ + L"task4_multipcolor.csv",
      };
    std::vector<std::wstring> source_base_name = 
      { p_path_.output_path_ + L"profile_data\\c_tzr_mesh_post__",
        p_path_.output_path_ + L"profile_data\\c_tzr_mesh_post__",
        p_path_.output_path_ + L"profile_data\\t_tzr_mesh_post__",
        p_path_.output_path_ + L"profile_data\\t_tzr_mesh_post__"};
    std::vector<std::wstring> target_base_name =
      { p_path_.output_path_ + L"profile_data_pic\\c_tzr_mesh_post__",
        p_path_.output_path_ + L"profile_data_pic\\c_tzr_mesh_post__",
        p_path_.output_path_ + L"profile_data_pic\\t_tzr_mesh_post__",
        p_path_.output_path_ + L"profile_data_pic\\t_tzr_mesh_post__" };
    std::vector<int> loop_info_start = { start1, start2, start1, start2 };
    std::vector<int> loop_info_end = { end1, end2, end1, end2 };
    std::vector<std::wstring> title_format = {
      L"C [kg/m3] at time {:.3f} [hr]",
      L"C [kg/m3] at time {:.3f} [hr]",
      L"T [C] at time {:.3f} [hr]",
      L"T [C] at time {:.3f} [hr]"
      };
    std::wstring script_name = L"quick_many_pcolor.py ";
      //space at the end is needed
      //to separate the script and parameter names
    std::wstring combine;
    LPCWSTR params;

    for (int i = 0; i < 4; i++) {
      wofs.open(fname[i]);
      if (!wofs) {
        std::wcerr << L"Cannot open " << fname[i] << L'\n';
        std::system("pause");
      }
      wofs << source_base_name[i].c_str() << '\n';
      wofs << loop_info_start[i] << '\n';
      wofs << loop_info_end[i] << '\n';
      wofs << target_base_name[i].c_str() << '\n';
      wofs << p_path_.output_path_ << "r_ratio_mesh_post_.csv" << '\n';
      wofs << p_path_.output_path_ << "z_mesh_vec.csv" << '\n';
      wofs << title_format[i].c_str() << '\n';
      wofs << "r/R [-]" << '\n';
      wofs << "z [m]" << '\n';
      wofs << p_path_.output_path_ << "t_hr_vec.csv";
      wofs.close();
      combine = script_name + fname[i];
      params = combine.c_str();
      usr::CallPythonScript(params);
    }
  }

  void SinglePhaseDeposition::PrintDetailedOutput() {
    auto start_time_detailed_output = std::chrono::high_resolution_clock::now();
    std::cout << "creating detailed output and all graphs\n";
    std::vector<std::vector<double>>* pt_1d1 = &ti_tz_mesh_post_;
    std::vector<std::vector<double>>* pt_1d2 = &tw_tz_sec_post_;
    std::vector<std::vector<double>>* pt_1d3 = &m_tz_post_;
    std::vector<std::vector<double>>* pt_ave1 = &ti_ave_post_;
    std::vector<std::vector<double>>* pt_ave2 = &tw_ave_post_;
    std::vector<std::vector<double>>* pt_ave3 = &m_total_t_post_;
    std::vector<std::vector<std::vector<double>>*> list_1d_data 
      = {pt_1d1, pt_1d2, pt_1d3};
    std::vector<std::vector<std::vector<double>>*> list_ave_data 
      = {pt_ave1, pt_ave2, pt_ave3};

    //print 2D profile data at every time steps
    usr::PrintVectorCSV3D<double>(
      L"t_tzr_mesh_post_",
      p_path_.output_path_ + L"profile_data\\",
      t_tzr_mesh_post_);

    usr::PrintVectorCSV3D<double>(
      L"c_tzr_mesh_post_",
      p_path_.output_path_ + L"profile_data\\",
      c_tzr_mesh_post_);

    std::wofstream ofs;
    ofs.open(p_path_.output_path_ + L"r_ratio_mesh_post_.csv");
    if (!ofs) {
      std::cerr << "cannot open r_ratio_mesh_post_.csv\n";
      std::system("pause");
    }
    usr::PrintVectorCSVColumn(ofs, r_ratio_mesh_post_);
    ofs.close();

    PlotEveryCTProfile4Tasks();

    std::wstring wfolder_name1 = p_path_.output_path_ + L"data_1d\\";
    std::wstring wfolder_name2 = p_path_.output_path_ + L"data_ave\\";
    LPCWSTR lwfolder_name1 = wfolder_name1.c_str();
    LPCWSTR lwfolder_name2 = wfolder_name2.c_str();
    CreateDirectory(lwfolder_name1, NULL);
    CreateDirectory(lwfolder_name2, NULL);
    //print 1D profile data at every time steps
    //each time step occupies 1 row
    std::vector<std::wstring> fname_with_path_vec;
    for (std::size_t i = 0; i < list_1d_data.size(); i++) {
      std::wstring full_name_with_path = wfolder_name1 
        + file_name_1D_Profile_[i];
      std::wofstream ofs;
      ofs.open(full_name_with_path);
      usr::PrintVectorCSV2D<double>(ofs, *list_1d_data[i]);
      ofs.close();
      fname_with_path_vec.push_back(full_name_with_path);
    }
    usr::PlotPColorGraph(
      fname_with_path_vec[0],
      p_path_.output_path_ + L"z_mesh_vec.csv",
      p_path_.output_path_ + L"t_hr_vec.csv",
      L"Ti[C]",
      L"z[m]",
      L"time[hr]");
    
    usr::PlotPColorGraph(fname_with_path_vec[1],
      p_path_.output_path_ + L"z_sec_vec.csv",
      p_path_.output_path_ + L"t_hr_vec.csv",
      L"Tw[C]",
      L"z[m]",
      L"time[hr]");

    usr::PlotPColorGraph(fname_with_path_vec[2],
      p_path_.output_path_ + L"z_sec_vec.csv",
      p_path_.output_path_ + L"t_hr_vec.csv",
      L"wax-mass[gram]",
      L"z[m]",
      L"time[hr]");

    //print average properties
    //  1st column = time, 2nd column = ave-property
    std::vector<std::vector<std::wstring>> print_xy;
    std::vector<std::wstring> fname_no_ext_vec;
    for (std::size_t i = 0; i < list_ave_data.size(); i++) {
      std::wstring full_name_with_path = 
        wfolder_name2 + file_name_ave_2column_[i];
      std::wofstream ofs;
      ofs.open(full_name_with_path);
      usr::PrintVectorCSV2D<double>(ofs, *list_ave_data[i]);
      ofs.close();
      fname_no_ext_vec.push_back(full_name_with_path.substr(0, full_name_with_path.size() - 3));
      print_xy.push_back({
        L"pyplot_2columns.py",
        fname_no_ext_vec[i] + L"csv",
        fname_no_ext_vec[i] + L"jpg",
        L"name",
        L"t[hr]",
        L"y"});
    }

    print_xy[0][3] = L"ti_ave[C]";
    print_xy[0][5] = L"ti_ave[C]";
    print_xy[1][3] = L"tw_ave[C]";
    print_xy[1][5] = L"tw_ave[C]";
    print_xy[2][3] = L"m_total[gram]";
    print_xy[2][5] = L"m_total[gram]";
    PrintXY(print_xy);

    auto now_time = std::chrono::high_resolution_clock::now();
    std::cout
      << "time required to create detailed graphs [millisecond] = "
      << std::chrono::duration_cast<std::chrono::milliseconds>(
        now_time - start_time_detailed_output).count()
      << '\n';

  }

  void SinglePhaseDeposition::SimplePostCalculation() {
    //calculate volume average thickness
    //calculate volume average wax fraction
    long n_t;
    long n_axial;
    double ri;
    double fw;
    double dz;
    double ri_ave;
    double length_dep;
    double deposit_total_volume;
    double fw_volume_weighted_average;
    delta_mm_vol_ave_.clear();
    ri_m_vol_ave_.clear();
    fw_vol_ave_.clear();
    n_t = ans_t_.size();
    n_axial = ans_ri_fw_[0].size() / 2;

    length_dep = t_z_profile_.z_mesh_[n_zsec_];
    for (long i = 0; i < n_t; i++) {
      deposit_total_volume = 0;
      fw_volume_weighted_average = 0.0;
      for (long j = 0; j < n_axial; j++) {
        dz = t_z_profile_.z_mesh_[j + 1] - t_z_profile_.z_mesh_[j];
        ri = ans_ri_fw_[i][j];
        fw = ans_ri_fw_[i][j + n_axial];
        deposit_total_volume = deposit_total_volume
          + usr::pi * (r_ * r_ - ri * ri) * dz;
        fw_volume_weighted_average = fw_volume_weighted_average
          + usr::pi * (r_ * r_ - ri * ri) * dz * fw;
      }
      ri_ave = std::pow(r_ * r_
        - deposit_total_volume / usr::pi / length_dep, 0.5);
      fw_volume_weighted_average /= deposit_total_volume;
      std::vector<double> ri_ans = {ans_t_[i] / 3600.0, ri_ave};
      std::vector<double> delta_ans = {ans_t_[i] / 3600.0, (r_ - ri_ave) * 1000.0};
      std::vector<double> fw_ans = { ans_t_[i] / 3600.0, fw_volume_weighted_average };
      ri_m_vol_ave_.push_back(ri_ans);
      delta_mm_vol_ave_.push_back(delta_ans);
      fw_vol_ave_.push_back(fw_ans);
    }

  }

  void SinglePhaseDeposition::DetailedPostCalculation() {
    std::size_t n_t;
    std::size_t n_axial;
      //section variable, not mesh
    CoreDepositionEquation DepEq;
    mass_calculation::RadialCProfile c_radial;
    std::vector<double> rhs_ri_fw;
    double rho_dep;
    double ri;
    double fw;
    double t;
    double ti; //mesh interface temperature [C]
    double tw;
    double ti_ave; //lengh weighted average
    double tw_ave;
    std::vector<double> wax_mass_vec;
    double length_dep;
    double wax_mass;
    double wax_mass_total;
    double dz;
    DepEq.SetAll1Phase(*this);
    n_t = ans_t_.size();
    n_axial = ans_ri_fw_[0].size() / 2; 
    ptr_wrap = &DepEq;
    n_rmesh_ = h_flow_.GetProperties(usr::kSinglePhaseFlow::n_rmesh);
    t_tzr_mesh_post_.clear();
    c_tzr_mesh_post_.clear();
    ti_tz_mesh_post_.clear();
    ti_ave_post_.clear();
    tw_tz_sec_post_.clear();
    tw_ave_post_.clear();
    length_dep = t_z_profile_.z_mesh_[n_zsec_];
    for (std::size_t i = 0; i < n_t; i++) {
      ti_tz_mesh_post_.push_back({});
      tw_tz_sec_post_.push_back({});
      t = ans_t_[i];
      rhs_ri_fw = WrapSolve1P(ans_t_[i], ans_ri_fw_[i]);
      c_radial = DepEq.c_radial_;
      wax_mass_vec.clear();
      wax_mass_total = 0.0;
      t_tzr_mesh_post_.push_back(c_radial.t_zr_mesh_);
      c_tzr_mesh_post_.push_back(c_radial.c_zr_mesh_);
      for (std::size_t j = 0; j < n_axial + 1; j++) {
        //mesh loop
        ti = c_radial.t_zr_mesh_[j][n_rmesh_-1];
        ti_tz_mesh_post_[i].push_back(ti);
      }
      ti_ave = 0.0;
      tw_ave = 0.0;
      for (std::size_t j = 0; j < n_axial; j++) {
        //section loop
        dz = t_z_profile_.z_mesh_[j + 1] - t_z_profile_.z_mesh_[j];
          //only the z part in t_z_profile_ is needed,
          //DepEq.t_z_profile_ id not needed
        ri = ans_ri_fw_[i][j];
        fw = ans_ri_fw_[i][j + n_axial];
        rho_dep = DepEq.oil_.GetProperty(usr::kFluid::rho, 
                               DepEq.t_z_profile_.tdep_sec_[j]);
        wax_mass = usr::pi * (r_ * r_ - ri * ri) * dz * rho_dep * fw * 1000.0;
        wax_mass_vec.push_back(wax_mass);
        wax_mass_total = wax_mass_total + wax_mass;
        ti_ave = ti_ave + c_radial.t_zr_sec_[j][n_rmesh_ - 1] * dz;
        tw = DepEq.t_z_profile_.thw_sec_[j];
        tw_ave = tw_ave + tw * dz;
        tw_tz_sec_post_[i].push_back(tw);
      }
      ti_ave = ti_ave / length_dep;
      tw_ave = tw_ave / length_dep;
      ti_ave_post_.push_back({t/3600.0,ti_ave});
      tw_ave_post_.push_back({t/3600.0,tw_ave});
      m_tz_post_.push_back(wax_mass_vec);
      m_total_t_post_.push_back({t/3600.0, wax_mass_total});
      r_ratio_mesh_post_ = c_radial.r_ratio_mesh_;
    }
   return; 
  }


  void SinglePhaseDeposition::PrintParameters(
      std::ostream& os, usr::kDepositionPrintOption print_option) {
    long n_t;
    long n_radial;
    n_t = ans_t_.size();
    n_radial = ans_ri_fw_[0].size() / 2;
      //section number, not mesh
      // "/2" because ans_ri_fw_[0] contain both ri and fw data
    switch (print_option) {
      case(usr::kDepositionPrintOption::ri_profile):{
        for (long i = 0; i < n_t; i++) {
          for (long j = 0; j < n_radial; j++) {
            os << std::setprecision(7) << std::scientific 
               << ans_ri_fw_[i][j];
            if (j < n_radial - 1) {
            //add comma at the end except the last member of each row
              os << ',';
            }
          }
          os << '\n';
        }
        break;
      }
      case(usr::kDepositionPrintOption::delta_mm_profile): {
        for (long i = 0; i < n_t; i++) {
          for (long j = 0; j < n_radial; j++) {
            os << std::setprecision(7) << std::scientific 
               << (r_ - ans_ri_fw_[i][j]) * 1000.0;
            if (j < n_radial - 1) {
              //add comma at the end except the last member of each row
              os << ',';
            }
          }
          os << '\n';
        }
        break;
      }
      case(usr::kDepositionPrintOption::fw_profile): {
        for (long i = 0; i < n_t; i++) {
          for (long j = 0; j < n_radial; j++) {
            os << std::setprecision(7) << std::scientific
              << ans_ri_fw_[i][j+n_radial];
            if (j < n_radial - 1) {
              //add comma at the end except the last member of each row
              os << ',';
            }
          }
          os << '\n';
        }
        break;
      }
      case(usr::kDepositionPrintOption::ri_average): {
        for (long i = 0; i < n_t; i++) {
          os << std::setprecision(7) << std::scientific
             << ri_m_vol_ave_[i][0] << ',';
          os << ri_m_vol_ave_[i][1] << '\n';
        }
        break;
      }
      case(usr::kDepositionPrintOption::delta_mm_average): {
        for (long i = 0; i < n_t; i++) {
          os << std::setprecision(7) << std::scientific
             << delta_mm_vol_ave_[i][0] << ',';
          os << delta_mm_vol_ave_[i][1] << '\n';
        }
        break;
      }
      case(usr::kDepositionPrintOption::fw_average): {
        for (long i = 0; i < n_t; i++) {
          os << std::setprecision(7) << std::scientific
             << fw_vol_ave_[i][0] << ',';
          os << fw_vol_ave_[i][1] << '\n';
        }
        break;
      }
      case(usr::kDepositionPrintOption::t_hr_vec): {
        for (long i = 0; i < n_t; i++) {
          os << std::setprecision(7) << std::scientific << ans_t_[i] / 3600.0 << '\n';
        }
        break;
      }
      case(usr::kDepositionPrintOption::z_sec_vec): {
        for (long i = 0; i < n_radial; i++) {
          os << std::setprecision(7) << std::scientific
             << t_z_profile_.z_sec_[i] << '\n';
        }
        break;
      }
      case(usr::kDepositionPrintOption::z_mesh_vec): {
        for (long i = 0; i < n_radial + 1; i++) {
          os << std::setprecision(7) << std::scientific
            << t_z_profile_.z_mesh_[i] << '\n';
        }
        break;
      }
    }
  }

  void SinglePhaseDeposition::PrintPicture(usr::kDepositionPrintOption option) const{
    switch(option){
      case(usr::kDepositionPrintOption::ri_profile_picture): {
        usr::PlotPColorGraph(
          p_path_.output_path_ + L"ri_profile.csv",
          p_path_.output_path_ + L"z_sec_vec.csv",
          p_path_.output_path_ + L"t_hr_vec.csv",
          L"ri[m]",
          L"z[m]",
          L"t[hr]");
        break;
      }
      case(usr::kDepositionPrintOption::delta_profile_picture): {
        usr::PlotPColorGraph(
          p_path_.output_path_ + L"delta_mm_profile.csv",
          p_path_.output_path_ + L"z_sec_vec.csv",
          p_path_.output_path_ + L"t_hr_vec.csv",
          L"delta[mm]",
          L"z[m]",
          L"t[hr]");
        break;
      }
      case(usr::kDepositionPrintOption::fw_profile_picture): {
        usr::PlotPColorGraph(
          p_path_.output_path_ + L"fw_profile.csv",
          p_path_.output_path_ + L"z_sec_vec.csv",
          p_path_.output_path_ + L"t_hr_vec.csv",
          L"fw[-]",
          L"z[m]",
          L"t[hr]");
        break;
      }
    }
  }

  void SinglePhaseDeposition::PrintXY(std::vector<std::vector<std::wstring>> call_string) const {
     std::size_t n_i = call_string.size();
     for (size_t i = 0; i < n_i; i++) {
       std::wstring combined;
       size_t n_j = call_string[i].size();
       for (size_t j = 0; j < n_j; j++) {
         if (j < n_j - 1) {
           combined = combined + L" " + call_string[i][j] + L" ";
         }
         else {
           combined = combined + L" " + call_string[i][j];
         }
       }
       LPCWSTR param = combined.c_str();
       usr::CallPythonScript(param);
     }
  }

  void SinglePhaseDeposition::SetProgramPath(ProgramPath p_path) {
    p_path_ = p_path;
  }

  void CoreDepositionEquation::SetAll1Phase(
      const SinglePhaseDeposition& dep) {
    //fix input variables
    sim_dep_options_ = dep.sim_dep_options_;
    oil_ = dep.oil_;
    c_fluid_ = dep.c_fluid_;
    inlet_ = dep.inlet_;
    c_flow_ = dep.coolant_inlet_;
    pipe_ = dep.pipe_;
    pipe_material_ = dep.pipe_material_;
    wax_ = dep.wax_;
    wax_prop_ = dep.wax_prop_;
    precipitation_eq_ = dep.precipitation_eq_;
    n_zsec_ = dep.n_zsec_;
    n_rmesh_ = dep.h_flow_.GetProperties(usr::kSinglePhaseFlow::n_rmesh);
    //non-const input-output variables
    h_flow_ = dep.h_flow_;
    t_z_profile_ = dep.t_z_profile_;
    deposit_profile_ = dep.deposit_profile_;
    r_ = dep.r_;

    c_ini0_ = wax_prop_.GetC(dep.Th0_inlet_);
    initial_total_oil_volume_ = dep.initial_total_oil_volume_;
    if (sim_dep_options_.GetShearDepositionModel() 
        == usr::kShearDepositionModel::MaximumTolerableShearStressModel) {
      tau0_sec_vec_ = dep.tau0_sec_vec_;
    }
  }
  
  std::vector<double> CoreDepositionEquation::Solve1P(
      const std::vector<double>& ri,
      const std::vector<double>& fw){
    //Return std::vector<double> size of 2*N of the ODE right hand side
    //The first N members are RHS of dri/dt equation of every cell
    //The second N members are RHS of dFw/dt equation of every cell

    //calculate the dr/dt terms from
    //dr/dt = a1 * (a2 - a3)
    double a1_term;
    double a2_term;
    double a3_term;
    double Dwo_ri;
    double Dwo_eff;
    double dc_dr_rim;
    double dc_dr_rip;
    double dc_dT_ri;
    double dTdr_rip; //dT/dr at ri plus
    double dTdr_rim; //dT/dr at ri minus

    double kdep_Tri;
    double koil_Tri;
    usr::BigEquation alpha_eq;
    double alpha;
    double rho_dep;
    std::vector<double> rhs_ri;
    std::vector<double> rhs_fw;
    //for shear stress calculation
    SinglePhaseHydrodynamics hydro_local;
    double tau_local; //[Pa]
    deposit_profile_.SetFwVec(fw);
    deposit_profile_.SetRiVec(ri);

    t_z_profile_.Solve(deposit_profile_);
      //local deposit_profile_ object in t_z_profile_ get updated
      // when .Solve method is run

    c_radial_ = mass_calculation::RadialCProfile();
      //to delete everything and reset the status of c_radial_
    //update wax_prop_.precipitation_curve_
    //with respect to depletion in the oil here, as needed.
    c_radial_.SetAll(
      sim_dep_options_.GetPrecipitationRate(),
      t_z_profile_, //depletion term is stored in here
      h_flow_,
      wax_prop_,
      ri,
      fw,
      c_ini0_,
      initial_total_oil_volume_,
      sim_dep_options_);
    c_radial_.ZDepositGrouping();
    c_radial_.Solve();
    //Get closure relationship for wax crystal aspect ratio calculation
    alpha_eq = sim_dep_options_.GetAlphaEq();

    hydro_local = t_z_profile_.h_hydro_;
    //for shear deposition option
    for (int i = 0; i < n_zsec_; i++) {
      double sr1 = 1.0;
      double sr2 = 1.0; //Panacharoensawad (2012) fitting parameters
      if (sim_dep_options_.GetShearDepositionModel() ==
        usr::kShearDepositionModel::MaximumTolerableShearStressModel) {
        //estimate the local shear stress
        double t_bulk_local;
        t_bulk_local = (c_radial_.t_bulk_z_mesh_[i] + c_radial_.t_bulk_z_mesh_[i + 1]) / 2.0;
        tau_local = hydro_local.SolveOnlyShearStress(t_bulk_local, ri[i], 0.0);
        double tau_max;
        tau_max = tau0_sec_vec_[i] * (sim_dep_options_.GetMSR1() * fw[i] + 1.0);
        sr1 = (tau_max - tau_local) / tau_local;
        if (sr1 > 1.0) {
          sr1 = 1.0;
        } else if (sr1 < 0) {
          sr1 = 0.0;
        }
        sr2 = sim_dep_options_.GetMSR2() * (tau_local - tau0_sec_vec_[i]) /
                tau0_sec_vec_[i];
        if (sr2 > 1.0) {
          sr2 = 1.0;
        } else if (sr2 < 0){
          sr2 = 0.0;
        }
      }
      T_ri_ = c_radial_.t_zr_sec_[i][n_rmesh_ - 1];
      rho_dep = oil_.GetProperty(usr::kFluid::rho, t_z_profile_.tdep_sec_[i]);
      a1_term = -1.0 / fw[i] / rho_dep;
      dc_dr_rim = (c_radial_.c_zr_sec_[i][n_rmesh_ - 1] - 
                   c_radial_.c_zr_sec_[i][n_rmesh_ - 2]) / 
                  (c_radial_.r_ratio_mesh_[n_rmesh_ - 1] -
                   c_radial_.r_ratio_mesh_[n_rmesh_ - 2]) / ri[i];
      Dwo_ri = c_radial_.wax_prop_.CalculateDiffusivity(T_ri_);
      a2_term = -1.0 * Dwo_ri * dc_dr_rim;
      dTdr_rim = (c_radial_.t_zr_sec_[i][n_rmesh_ - 1] -
                  c_radial_.t_zr_sec_[i][n_rmesh_ - 2]) /
                 (c_radial_.r_ratio_mesh_[n_rmesh_ - 1] -
                  c_radial_.r_ratio_mesh_[n_rmesh_ - 2]) / ri[i];
      kdep_Tri = deposit_profile_.CalculateKDep(T_ri_, fw[i]);
      koil_Tri = oil_.GetProperty(usr::kFluid::k, T_ri_);
      dTdr_rip = koil_Tri / kdep_Tri * dTdr_rim;
      dc_dT_ri = c_radial_.wax_prop_.GetDCDT(T_ri_);
      dc_dr_rip = dc_dT_ri * dTdr_rip;
      alpha = alpha_eq.At(fw[i]);
      Dwo_eff = Dwo_ri * wax_prop_.CalculateEffectiveDiffusivityRatio(alpha, fw[i]);
      a3_term = -Dwo_eff * dc_dr_rip * sr2;
      if (a2_term <= a3_term) {
        //DormandPrince approximate the thickness and Fw
        //at the next time step to calculate
        //dr/dt and dfw/dt at the current time step
        //This method can overshoot the ri,
        //resulting in the deposit that is thicker than it should,
        //as a result, the demand for deposition is more than the
        //supply wax molecules.
        //This if statement is here to allow the consistency
        //in the calculation
        //if the aging wax flux demand is more than the
        //incoming wax mass flux supply, then
        //all the supply wax mass flux is used for the aging purposes
        rhs_ri.push_back(0.0);
        rhs_fw.push_back(2.0 * ri[i] * a2_term
          / rho_dep / (r_ * r_ - ri[i] * ri[i]) * sr2);
      } else {
        rhs_ri.push_back(a1_term * (a2_term - a3_term) * sr1);
        rhs_fw.push_back(-2.0 * ri[i] * Dwo_eff * dc_dT_ri *
          dTdr_rip / rho_dep / (r_ * r_ - ri[i] * ri[i]) * sr2);
      }
    }
    rhs_ri.insert(rhs_ri.end(), rhs_fw.begin(), rhs_fw.end());
    return rhs_ri;
  }
} //namespace spdepo