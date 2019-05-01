#ifndef USER_CALL_OPTIONS
#define USER_CALL_OPTIONS
#include <map>
#include <string>
namespace spdepo {
  namespace usr {
    enum class kMainArgv2 {plot_big_equation,
                           deposition,
                           debug_LM,
                           debug_LM2,
                           call_LM
                             };
    enum class kMainArgv3 {single_phase};


    std::map<std::wstring, kMainArgv2> string_to_kmainarg2{
        {L"plot_big_equation",kMainArgv2::plot_big_equation},
        {L"deposition", kMainArgv2::deposition},
        {L"debug_LM", kMainArgv2::debug_LM},
        {L"debug_LM2", kMainArgv2::debug_LM2 },
        {L"call_LM", kMainArgv2::call_LM}
      };
    std::map<std::wstring, kMainArgv3> string_to_kmainarg3{
        {L"single_phase", kMainArgv3::single_phase}
      };
  } // namespace usr
} // namespace spdepo
#endif