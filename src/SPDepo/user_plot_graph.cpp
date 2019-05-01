#include <Windows.h>
#include "user_python_script.h"
#include <sstream>
#include <string>
#include <iomanip>
namespace spdepo {
namespace usr {
  void CallPythonScript(LPCWSTR param) {
    LPCWSTR operation = L"open";
    LPCWSTR name = L"python";
    ShellExecute(nullptr, operation, name, param, NULL, SW_HIDE);
  }
  void PlotPColorGraph(
    std::wstring file_rcolumn_zrow,
    std::wstring file_r_column,
    std::wstring file_z_column,
    std::wstring name_title,
    std::wstring name_x_axis,
    std::wstring name_y_axis) {
    std::wstring file_name = L"pcolor.py";
    std::wstring combined = file_name + L" " +
      file_rcolumn_zrow + L" " +
      file_r_column + L" " +
      file_z_column + L" " +
      name_title + L" " +
      name_x_axis + L" " +
      name_y_axis;
    LPCWSTR param = combined.c_str();
    usr::CallPythonScript(param);
    
  }
  void PlotMultiplePColorGraph(
    const std::wstring& name_no_ext,
    const std::wstring& folder_name,
    const std::vector<std::vector<std::vector<double>>>& obj,
    const std::wstring& file_r_column,
    const std::wstring& file_z_column,
    const std::wstring& name_title,
    const std::wstring& name_x_axis,
    const std::wstring& name_y_axis,
    const std::vector<double>& time) {
    std::size_t n = obj.size();
    for (std::size_t i = 0; i < n; i++) {
      std::wstring wi(std::to_wstring((int)i));
      std::wstring full_name = folder_name + name_no_ext
        + L"_" + wi + L".csv";
      std::wstringstream wstr;
      wstr << std::fixed << std::setprecision(2) << time[i] << L"hr";
      std::wstring wtime(wstr.str());
      PlotPColorGraph(full_name, file_r_column, file_z_column,
        name_title + L"_" + wtime, name_x_axis, name_y_axis);
    }
  }
  void PlotTwoSeries(
      const std::wstring& file1_with_path_csv,
      const std::wstring& file2_with_path_csv,
      const std::wstring& output_jpg,
      const std::wstring& title,
      const std::wstring& x_axis,
      const std::wstring& y_axis,
      const std::wstring& label1,
      const std::wstring& label2,
      const std::wstring& dpi,
      const std::wstring& plot_mode1,
      const std::wstring& marker_size1,
      const std::wstring& line_width1,
      const std::wstring& fill_style1,
      const std::wstring& marker_edge_width1,
      const std::wstring& plot_mode2,
      const std::wstring& marker_size2,
      const std::wstring& line_width2,
      const std::wstring& fill_style2,
      const std::wstring& marker_edge_width2,
      const std::wstring& grid_alpha) {
    //The reason that the input list is not the same as the
    //  order of parameter in the combined wstring is that
    //  the input list is for easy use when
    //  many parameters have the default value,
    //  except file_name/title/dpi that my be changed frequently
    //The below parameter order matches with the input in python file
    std::wstring combined =
      L"twoSeries.py " +
      file1_with_path_csv + L" " +
      file2_with_path_csv + L" " +
      output_jpg + L" " +
      title + L" " +
      x_axis + L" " +
      y_axis + L" " +
      plot_mode1 + L" " +
      marker_size1 + L" " +
      line_width1 + L" " +
      fill_style1 + L" " +
      marker_edge_width1 + L" " +
      label1 + L" " +
      plot_mode2 + L" " +
      marker_size2 + L" " +
      line_width2 + L" " +
      fill_style2 + L" " +
      marker_edge_width2 + L" " +
      label2 + L" " +
      grid_alpha + L" " +
      dpi;
    LPCWSTR param = combined.c_str();
    usr::CallPythonScript(param);
  }
}  //namespace spdepo
}  //namespace usr
