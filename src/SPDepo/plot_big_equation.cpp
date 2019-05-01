#include <fstream>
#include <string>
#include <vector>
#include <limits>
#include "plot_big_equation.h"
#include "user_string_operation.h"
#include "user_equation_format.h"
#include "user_python_script.h"
#include <Windows.h>
namespace spdepo {
  void plot_big_equation_task(double x_min,
                              double x_max, 
                              int n_points) {
    std::string eq_text;
    std::ifstream file;
    std::ofstream file_out_x;
    std::ofstream file_out_y;
    std::vector<std::vector<std::string>> eq_elements;
    std::vector<std::vector<double>> eq_elements_dbl;
    file.open("default_plot.BigEq");
    file_out_x.open("try_plot_x.csv");
    file_out_y.open("try_plot_y.csv");
    if (!file) {
      std::cerr << "default_plot.BigEq cannot be opened\n";
      std::system("pause");
      return;
    }
    if (!file_out_x) {
      std::cerr << "try_plot_x.csv cannot be opened\n";
      std::system("pause");
      return;
    }
    if (!file_out_y) {
      std::cerr << "try_plot_y.csv cannot be opened\n";
      std::system("pause");
      return;
    }
    file >> eq_text;
    file.close();
    std::cout << "test nan\n";
    double testnan;
    testnan = std::nan("");
    std::cout << testnan << '\n';
    std::cout << std::nan("") << '\n';
    std::cout << std::isnan(std::nan("")) << '\n';
    std::cout << eq_text << "\n\n\n";
    eq_elements = usr::ExtractListOfList(eq_text);
    usr::PrintNestedList(eq_elements);
    eq_elements_dbl = usr::ListOfListOfStringToDouble(eq_elements);
    std::cout << "\n\n\n";
    usr::PrintNestedList(eq_elements_dbl);
    usr::BigEquation plot_eq;
    plot_eq.ParseFromString(eq_text);
    double x, y, delta;
    delta = (x_max - x_min) / (n_points - 1);
    for (int i = 0; i < n_points; i++) {
      x = x_min + delta * i;
      file_out_x << std::setprecision(10) << x << '\n';
      y = plot_eq.At(x);
      file_out_y << std::setprecision(10) << y << '\n';
    }
    file_out_x.close();
    file_out_y.close();

    std::wstring file_name = L"pyplot_regular.py";
    std::wstring x_data_file = L"try_plot_x.csv";
    std::wstring y_data_file = L"try_plot_y.csv";
    std::wstring output_file = L"big_equation_plot.jpg";
    std::wstring name_title = L"BigEquation_Trial_Plot";
    std::wstring name_x_axis = L"x";
    std::wstring name_y_axis = L"y";

    std::wstring combined = file_name + L" " +
                            x_data_file + L" " +
                            y_data_file + L" " +
                            output_file + L" " +
                            name_title + L" " +
                            name_x_axis + L" " +
                            name_y_axis + L" ";
    LPCWSTR param = combined.c_str();
    usr::CallPythonScript(param);
  }
}