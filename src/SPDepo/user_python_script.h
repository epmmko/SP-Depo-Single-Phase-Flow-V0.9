#ifndef USR_PYTHON_SCRIPT_H_
#define USR_PYTHON_SCRIPT_H_
#include <Windows.h>

namespace spdepo {
namespace usr {
  void CallPythonScript(LPCWSTR param);
  void PlotPColorGraph(
    std::wstring file_rcolumn_zrow,
    std::wstring file_r_column,
    std::wstring file_z_column,
    std::wstring name_title,
    std::wstring name_x_axis,
    std::wstring name_y_axis);
  void PlotMultiplePColorGraph(
    const std::wstring& name_no_ext,
    const std::wstring& folder_name,
    const std::vector<std::vector<std::vector<double>>>& obj,
    const std::wstring& file_r_column,
    const std::wstring& file_z_column,
    const std::wstring& name_title,
    const std::wstring& name_x_axis,
    const std::wstring& name_y_axis,
    const std::vector<double>& time);
  //This function plot two graph with python
  //The default is that one graph uses the empty blue circle
  //  and another graph uses squre orange with 200 dpi
  //underscore in the title/axis name will be interpret as a space
  void PlotTwoSeries(
    const std::wstring& file1_with_path_csv,
    const std::wstring& file2_with_path_csv,
    const std::wstring& output_jpg,
    const std::wstring& title,
    const std::wstring& x_axis,
    const std::wstring& y_axis,
    const std::wstring& label1,
    const std::wstring& label2,
    const std::wstring& dpi = L"200",
    const std::wstring& plot_mode1 = L"o--",
    const std::wstring& marker_size1 = L"4",
    const std::wstring& line_width1 = L"0.2",
    const std::wstring& fill_style1 = L"none",
    const std::wstring& marker_edge_width1 = L"0.8",
    const std::wstring& plot_mode2 = L"s",
    const std::wstring& marker_size2 = L"2",
    const std::wstring& line_width2 = L"0.1",
    const std::wstring& fill_style2 = L"full",
    const std::wstring& marker_edge_width2 = L"1",
    const std::wstring& grid_alpha = L"0.5");
} //namespace spdepo
} //namespace usr

#endif

