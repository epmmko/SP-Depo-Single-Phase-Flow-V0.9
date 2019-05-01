#include "user_string_operation.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
namespace spdepo {
namespace usr {




  std::vector<std::string> split(std::string phrase,
                                 std::string delimiter) {
    std::vector<std::string> list;
    std::string s = phrase;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
      token = s.substr(0, pos);
      list.push_back(token);
      s.erase(0, pos + delimiter.length());
    }
    list.push_back(s);
    return list;
  }

  std::vector<int> ListIndexOf(std::string str, char x) {
    std::vector<int> index_list = {};
    int index_int;
    int base_index;
    std::string b;
    b = str;
    base_index = 0;
    for (int i = 0; i < 10; i++) {
      if (b.find(x) == std::string::npos) {
        index_int = -1;
      }
      else {
        index_int = b.find(x);
      }

      if (index_int != -1) {
        index_list.push_back(index_int + base_index);
        b = b.substr(index_int + 1);
        base_index = base_index + index_int + 1;
      }
      else {
        break;
      }
    }
    return index_list;
  }

  std::vector<std::vector<std::string>> ExtractListOfList(std::string a) {
    //Convert from 1233,[1,3.4,5],[2,3,4,5],[12.2,3],5,[6,3] to
    //{{"1","3.4","5"},{"2","3","4","5"},{"12,2","3"},{"6","3"}}
    std::vector<int> start_list = ListIndexOf(a, '[');
    std::vector<int> end_list = ListIndexOf(a, ']');
    std::vector<std::vector<std::string>> coefficient_nested_list = {};
    std::string coeff_str;
    int eq_number = start_list.size() - 2;
    for (int i = 0; i < eq_number + 2; i++) {
      coeff_str = a.substr(start_list[i] + 1,
        end_list[i] - start_list[i] - 1);
      coefficient_nested_list.push_back(split(coeff_str, ","));
    }
    return coefficient_nested_list;
  }

  void PrintNestedList(std::vector<std::vector<std::string>> nested_list) {
    for (std::vector<std::string> x : nested_list)
    {
      for (std::string u : x)
      {
        std::cout << u << "  ";
      }
      std::cout << '\n';
    }
  }

  void PrintNestedList(std::vector<std::vector<double>> nested_list) {
    for (std::vector<double> x : nested_list)
    {
      for (double u : x)
      {
        std::cout << u << "  ";
      }
      std::cout << '\n';
    }
  }

  std::vector<std::vector<double>> ListOfListOfStringToDouble(
      std::vector<std::vector<std::string>> nested_list) {
    std::size_t list_size = nested_list.size();
    std::vector<std::vector<double>> output(list_size,
                                            std::vector<double>{});
    for (int i = 0; i < list_size; i ++)
    {
      std::size_t sub_list_size = nested_list[i].size();
      for (int j = 0; j < sub_list_size; j++)
      {
        if (nested_list[i][j] == "") {
          output[i].push_back(std::nan(""));
        }
        else {
          output[i].push_back(std::stod(nested_list[i][j]));
        }
        
      }
    }
    return output;
  }

  template<class T>
  std::wostream& PrintVectorCSVColumn(std::wostream& os,
    const std::vector<T>& obj) {
    std::size_t n = obj.size();
    for (std::size_t i = 0; i < n - 1; i++) {
      os << obj[i] << L"\n";
    }
    os << obj[n - 1];
    return os;
  }

  template<class T>
  std::wostream& PrintVectorCSVRow(std::wostream& os,
    const std::vector<T>& obj) {
    std::size_t n = obj.size();
    for (std::size_t i = 0; i < n - 1; i++) {
      os << obj[i] << L",";
    }
    os << obj[n-1];
    return os;
  }

  template<class T>
  std::wostream& PrintVectorCSV2D(std::wostream& os,
    const std::vector<std::vector<T>>& obj) {
    std::size_t n = obj.size();
    for (std::size_t i = 0; i < n - 1; i++) {
      PrintVectorCSVRow(os, obj[i]);
      os << L"\n";
    }
    PrintVectorCSVRow<T>(os, obj[n-1]);
    return os;
  }

  template<class T>
  void PrintVectorCSV3D(const std::wstring& name_no_ext,
      const std::wstring& folder_name,
      const std::vector<std::vector<std::vector<T>>>& obj) {
    std::size_t n = obj.size();
      //number of pattern
    LPCWSTR lwfolder_name = folder_name.c_str();
    CreateDirectoryW(lwfolder_name,NULL);
    for (std::size_t i = 0; i < n; i++) {
      std::wstring wi(std::to_wstring((int)i));
      std::wstring full_name = folder_name + name_no_ext 
        + L"_" + wi + L".csv";
      std::wofstream ofs;
      ofs.open(full_name);
      if (!ofs) {
        std::cerr << "cannot open ";
        std::wcerr << full_name;
        std::cerr << '\n';
        std::system("pause");
      }
      PrintVectorCSV2D<T>(ofs,obj[i]);
      ofs.close();
    }
  }

  //template instantiate;
  template std::wostream& PrintVectorCSVColumn(std::wostream& os,
    const std::vector<double>& obj);
  template std::wostream& PrintVectorCSVColumn(std::wostream& os,
    const std::vector<int>& obj);
  template std::wostream& PrintVectorCSVColumn(std::wostream& os,
    const std::vector<std::wstring>& obj);

  template std::wostream& PrintVectorCSVRow(std::wostream& os,
    const std::vector<double>& obj);
  template std::wostream& PrintVectorCSVRow(std::wostream& os,
    const std::vector<int>& obj);
  template std::wostream& PrintVectorCSVRow(std::wostream& os,
    const std::vector<std::wstring>& obj);

  template std::wostream& PrintVectorCSV2D(std::wostream& os,
    const std::vector<std::vector<double>>& obj);
  template std::wostream& PrintVectorCSV2D(std::wostream& os,
    const std::vector<std::vector<int>>& obj);
  template std::wostream& PrintVectorCSV2D(std::wostream& os,
    const std::vector<std::vector<std::wstring>>& obj);

  template void PrintVectorCSV3D(const std::wstring& name_no_ext,
    const std::wstring& folder_name,
    const std::vector<std::vector<std::vector<double>>>& obj);
  template void PrintVectorCSV3D(const std::wstring& name_no_ext,
    const std::wstring& folder_name,
    const std::vector<std::vector<std::vector<int>>>& obj);
  template void PrintVectorCSV3D(const std::wstring& name_no_ext,
    const std::wstring& folder_name,
    const std::vector<std::vector<std::vector<std::wstring>>>& obj);

} //namespace usr
} //namespace spdepo
