#ifndef USER_STRING_OPERATION
#define USER_STRING_OPERATION

namespace spdepo {
namespace usr {
  std::vector<std::string> split(std::string phrase, std::string delimiter);
    //split with multiple delimiter
    //https://stackoverflow.com/questions/14265581/
    //  parse-split-a-string-in-c-using-string-delimiter-standard-c
  std::vector<int> ListIndexOf(std::string str, char x);
  std::vector<std::vector<std::string>> ExtractListOfList(std::string a);
  void PrintNestedList(std::vector<std::vector<std::string>> nested_list);
  void PrintNestedList(std::vector<std::vector<double>> nested_list);
  std::vector<std::vector<double>> ListOfListOfStringToDouble(
    std::vector<std::vector<std::string>> nested_list_of_string);
  template<class T>
  std::wostream& PrintVectorCSVColumn(std::wostream& os,
    const std::vector<T>& obj);
  template<class T>
  std::wostream& PrintVectorCSVRow (std::wostream& os,
    const std::vector<T>& obj);
  template<class T>
  std::wostream& PrintVectorCSV2D(std::wostream& os,
    const std::vector<std::vector<T>>& obj);
  template<class T>
  void PrintVectorCSV3D(const std::wstring& name_no_ext, const std::wstring& folder_name,
    const std::vector<std::vector<std::vector<T>>>& obj);
} //namespace usr
} //namespace spdepo

#endif