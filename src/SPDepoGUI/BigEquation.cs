using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SPDepoGUI
{
  namespace BigEquation
  {
    public static class BigEquationExtension {
      public static Dictionary<String, String> operator_dictionary =
        new Dictionary<String, String>()
        {
          {"",""},
          {"0","0: f2(f1(x))"},
          {"1","1: f1(f2(x))"},
          {"2","2: f1(x) + f2(x)"},
          {"3","3: f2(x) - f1(x)"},
          {"4","4: f1(x) - f2(x)"},
          {"5","5: f1(x) * f2(x)"},
          {"6","6: f2(x) / f1(x)"},
          {"7","7: f1(X) / f2(x)"},
          {"8","8: pow(f2(x), f1(x))"},
          {"9","9: pow(f1(x), f2(x))"}
        };

      public static List<int> ListIndexOf(this String str, char x){
        List<int> index_list = new List<int> { };
        int index_int;
        int base_index;
        String b;
        b = str;
        base_index = 0;
        for (int i = 0; i < 10; i++)
        {
          index_int = b.IndexOf(x);
          if (index_int != -1)
          {
            index_list.Add(index_int + base_index);
            b = b.Substring(index_int + 1);
            base_index = base_index + index_int + 1;
          }
          else
          {
            break;
          }
        }
        return index_list;
      }
      public static List<List<String>> ExtractListOfList(this String a){
      //Convert from 1233,[1,3.4,5],[2,3,4,5],[12.2,3],5,[6,3] to
      //{{"1","3.4","5"},{"2","3","4","5"},{"12,2","3"},{"6","3"}}
        List<int> start_list = new List<int>(a.ListIndexOf('['));
        List<int> end_list = new List<int>(a.ListIndexOf(']'));
        List<List<String>> coefficient_nested_list = new List<List<string>>();
        String coeff_str;
        int eq_number = start_list.Count - 2;
        for (int i = 0; i < eq_number + 2; i++)
        {
          coeff_str = a.Substring(start_list[i] + 1,
                        end_list[i] - start_list[i] - 1);
          coefficient_nested_list.Add(new List<String>(coeff_str.Split(',')));
        }
        return coefficient_nested_list;
      }
      public static void PrintNestedList(List<List<String>> nested_list)
      {
        foreach (List<String> x in nested_list)
        {
          foreach (String u in x)
          {
            Console.Write(u + "  ");
          }
          Console.Write('\n');
        }
      }

    }
  }
}

