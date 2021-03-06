#include <chrono>
#include <iostream>
#include "check_library3_vprofile.h"
#include "check_big_equation.h"
#include "check_simpson_method.h"
#include "check_deposition_library.h"
#include "check_dopri_many.h"
#include "check_plotting.h"
#include "check_LM.h"
#include "speed_test_debug.h"
#include "plot_big_equation.h"
#include "main_caller.h"
#include "user_call_options.h"
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <exception>
#include <Windows.h>
#include <cstdlib>
#include "program_path.h"
#include <sstream>
//https://stackoverflow.com/questions/36042637/
//  how-to-calculate-execution-time-in-milliseconds

//Calling Parameters argv can be the calculation option
//Argument passing into this program must be small case
//with no space.
//Type1 Calling options are
//  [calculation options] + [phase] + [boundary condition] 
//    + [specific option]
//  For example, C:\SPDepo\spdepo.exe heat_transfer oil_water field
//  This will do the heat transfer calculation for oil-water in the
//    field case.
//
//  [calculation options] are
//  0) default is deposition with single-phase 
//  1) "hydrodynamics"
//  2) "heat_transfer"
//  3) "deposition"
//  4) "plot_big_equation"
//     It is optional to give the .BigEq file as the next input
//     for example
//     C:\SPDepo\spdepo.exe plot_big_equation test1.BigEq
//
//  [phase] options are
//  1) "single_phase"
//  2) "oil_water"
//  3) "gas_liquid"
//
//  [boundary condition] options are
//  1) "isothermal"
//  2) "counter_current"
//  3) "concurrent"
//  4) "constant temperature"
//
//Type2 Calling Option is to find the unknown constant
//Calling syntax is
//  [fitting method] + [required experimental data file]
//
//  [fitting method] options are
//  1) "levenberg_marquardt"
//
//  [required experimental data file]
//  1) "thickness_data.csv waxfraction_data.csv"
//
//Type3 Calling Option is
//  "global_fitting folder_structure.csv"
//  folder_structure.csv contains the information on
//    structure of the data file and the name. It should be
//
//  spdepo.exe
//  spdepogui.exe
//  pcolor.py
//  folder_structure.csv
//  \data_set1\case1\delta.csv
//  \data_set1\case1\fw.csv
//  \data_set1\case07\delta.csv
//  \data_set1\case07\fw.csv
//  \data_set1\case18\delta.csv
//  \data_set1\case18\fw.csv
//  \data_south_pelto_ep\case18\delta.csv
//  \data_south_pelto_ep\case18\fw.csv
//  \data_south_pelto_ep\case27\delta.csv
//  \data_south_pelto_ep\case27\fw.csv
//
//  Content of the folder_structure.csv corresponding
//    to the above data files is
//
//    data_set1\case1\delta.csv
//    data_set1\case1\fw.csv
//    data_set1\case07\delta.csv
//    data_set1\case07\fw.csv
//    data_set1\case18\delta.csv
//    data_set1\case18\fw.csv
//    data_south_pelto_ep\case18\delta.csv
//    data_south_pelto_ep\case18\fw.csv
//    data_south_pelto_ep\case27\delta.csv
//    data_south_pelto_ep\case27\fw.csv
//
// Content of delta.csv is
//
// time[hr],thickness[m]
// 0, 0
// 0.5, 0.1
// etc.
// The unit must be hour and metre. The first line mustint main be
//   header of each column
// Content of fw.csv is
// 
// time[hr],wax_fraction[m]
// 0, 0
// 0.5, 0.1
// etc.
// The unit must be hour and metre. The first line must be
//   header of each column
//


int wmain(int argc, TCHAR **argv)
{
  spdepo::ProgramPath program_path;
  program_path.output_path_ = argv[1];
  std::wstring exe_path = argv[0];

  int exe_path_length = exe_path.length();
  program_path.python_path_ = exe_path.substr(0, exe_path_length - 10);
  SetCurrentDirectoryW(program_path.python_path_.c_str());
  wchar_t x1[MAX_PATH];
  GetCurrentDirectoryW(MAX_PATH, x1);
  std::wcout << x1 << '\n';
  //The argv[1] is the input path
  //argv[1]\\inputs\\ must contain the input data files.
  spdepo::usr::kMainArgv2 argv2;
  std::wcout << "argc = " << argc << '\n';
  for (int i = 0; i < argc; i++) {
    std::wcout << "argv[" << i << "] = " << argv[i] << '\n';
  }

  //Set the calling parameters
  if (argc >= 2) {
    try {
      argv2 = spdepo::usr::string_to_kmainarg2.at(argv[2]);
      //string_to_kmainarg1 is a map, defined in user_call_options.h
    }
    catch (const std::runtime_error& re)
    {
      std::cerr << "Runtime error: " << re.what() << std::endl;
      return 1;
    }
    catch (const std::exception& ex)
    {
      std::cerr << "Error occurred: " << ex.what() << std::endl;
      return 1;
    }
  }

  switch (argv2) {
  case spdepo::usr::kMainArgv2::plot_big_equation: {
    if (argc == 3) {
      spdepo::plot_big_equation_task();
    }
    else if (argc == 4) {
      spdepo::plot_big_equation_task(std::stod(argv[3]));
    }
    else if (argc == 5) {
      spdepo::plot_big_equation_task(std::stod(argv[3]),
        std::stod(argv[4]));
    }
    else {
      spdepo::plot_big_equation_task(std::stod(argv[3]),
        std::stod(argv[4]),
        std::stoi(argv[5]));
    }


    std::system("pause");
    return 0;
    break;
  }
                                                   //case: deposition single_phase
  case spdepo::usr::kMainArgv2::deposition: {
    spdepo::usr::kMainArgv3 argv3;
    argv3 = spdepo::usr::string_to_kmainarg3.at(argv[3]);
    switch (argv3) {
    case(spdepo::usr::kMainArgv3::single_phase): {
      spdepo::Main1PhaseDeposition Deposition1PMain;
      Deposition1PMain.Run(program_path);
      break;
    }
    }
    //      spdepo::CheckDepositionLibrary();
    //      spdepo::quick_multi_pcolor();
    std::system("pause");
    return 0;
    break;
  }
  case spdepo::usr::kMainArgv2::debug_LM: {
    spdepo::CheckLM2 check_LM2;
    check_LM2.Run();
    std::system("pause");
    return 0;
    break;
  }
  case spdepo::usr::kMainArgv2::call_LM: {
    spdepo::Main1PhaseFittingSearch main_fitting_search;
    main_fitting_search.Run(program_path);
    std::system("pause");
    return 0;
    break;
  }
  }



  auto start_time = std::chrono::high_resolution_clock::now();
  auto end_time = std::chrono::high_resolution_clock::now();
  std::cout << "The total program run-time in milliseconds is ";
  std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(
    end_time - start_time).count() << '\n';
  std::time_t test = std::time(nullptr);
  std::cout << test;
  std::system("pause");
  return 0;
}

