#include "check_plotting.h"
#include "user_python_script.h"
#include <iostream>
#include <Windows.h>
#include <fstream>
namespace spdepo {
  void quick_multi_pcolor() {
    //If the task is not separated, into sub-task,

    std::cout << "hi from quick plot\n";
    std::system("del .\\profile_data2\\*.* /Q");
    CreateDirectory(L"profile_data2", NULL);

    std::ofstream ofs;
    std::wstring fname = L"call1_multipcolor.csv";
    ofs.open(fname);
    if (!ofs) {
      std::wcerr << L"Cannot open " << fname << L'\n';
      std::system("pause");
    }
    //plot concentration profile
    ofs << ".\\profile_data\\c_tzr_mesh_post__" << '\n';
    ofs << 0 << '\n';
    ofs << 16 << '\n';
    ofs << ".\\profile_data2\\c_tzr_mesh_post__" << '\n';
    ofs << ".\\r_ratio_mesh_post_.csv" << '\n';
    ofs << ".\\z_mesh_vec.csv" << '\n';
    ofs << "C [kg/m3] at time {:.3f} [hr]" << '\n';
    ofs << "r/R [-]" << '\n';
    ofs << "z [m]" << '\n';
    ofs << "t_hr_vec.csv";
    ofs.close();
    std::wstring script_name = L"quick_many_pcolor.py ";
    std::wstring combine = script_name + fname;
    LPCWSTR params = combine.c_str();
    usr::CallPythonScript(params);

    fname = L"call2_multipcolor.csv";
    ofs.open(fname);
    if (!ofs) {
      std::wcerr << L"Cannot open " << fname << L'\n';
      std::system("pause");
    }
    //plot concentration profile
    ofs << ".\\profile_data\\c_tzr_mesh_post__" << '\n';
    ofs << 17 << '\n';
    ofs << 32 << '\n';
    ofs << ".\\profile_data2\\c_tzr_mesh_post__" << '\n';
    ofs << ".\\r_ratio_mesh_post_.csv" << '\n';
    ofs << ".\\z_mesh_vec.csv" << '\n';
    ofs << "C [kg/m3] at time {:.3f} [hr]" << '\n';
    ofs << "r/R [-]" << '\n';
    ofs << "z [m]" << '\n';
    ofs << "t_hr_vec.csv";
    ofs.close();

    combine = script_name + fname;
    params = combine.c_str();
    usr::CallPythonScript(params);


    //plot temperature profile
    fname = L"call3_multipcolor.csv";
    ofs.open(fname);
    if (!ofs) {
      std::wcerr << L"Cannot open " << fname << L'\n';
      std::system("pause");
    }

    ofs << ".\\profile_data\\t_tzr_mesh_post__" << '\n';
    ofs << 0 << '\n';
    ofs << 16 << '\n';
    ofs << ".\\profile_data2\\t_tzr_mesh_post__" << '\n';
    ofs << ".\\r_ratio_mesh_post_.csv" << '\n';
    ofs << ".\\z_mesh_vec.csv" << '\n';
    ofs << "T [C] at time {:.3f} [hr]" << '\n';
    ofs << "r/R [-]" << '\n';
    ofs << "z [m]" << '\n';
    ofs << "t_hr_vec.csv";
    ofs.close();
    
    combine = script_name + fname;
    params = combine.c_str();
    usr::CallPythonScript(params);

    fname = L"call4_multipcolor.csv";
    ofs.open(fname);
    if (!ofs) {
      std::wcerr << L"Cannot open " << fname << L'\n';
      std::system("pause");
    }

    ofs << ".\\profile_data\\t_tzr_mesh_post__" << '\n';
    ofs << 17 << '\n';
    ofs << 32 << '\n';
    ofs << ".\\profile_data2\\t_tzr_mesh_post__" << '\n';
    ofs << ".\\r_ratio_mesh_post_.csv" << '\n';
    ofs << ".\\z_mesh_vec.csv" << '\n';
    ofs << "T [C] at time {:.3f} [hr]" << '\n';
    ofs << "r/R [-]" << '\n';
    ofs << "z [m]" << '\n';
    ofs << "t_hr_vec.csv";
    ofs.close();

    combine = script_name + fname;
    params = combine.c_str();
    usr::CallPythonScript(params);

  }
} //namespace spdepo