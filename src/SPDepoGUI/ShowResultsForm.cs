using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace SPDepoGUI
{
  public partial class ShowResultsForm : Form
  {
    String caller_;
    public ShowResultsForm()
    {
      InitializeComponent();
    }

    public ShowResultsForm(String default_file_name,
                           String caller = "HydroSinglePhase"){
      InitializeComponent();
      textBox4.Text = default_file_name;
      caller_ = caller;
    }

    private void button2_Click(object sender, EventArgs e)
    {
      AdditionalPicture pic1 = new AdditionalPicture();
      AdditionalPicture pic2 = new AdditionalPicture();
      AdditionalPicture pic3 = new AdditionalPicture();
      string original_name = textBox4.Text;
      pic1.title_name_ = original_name;
      string core_name;
      int length = original_name.Length;
      core_name = original_name.Substring(0, length - 4);
      pic2.title_name_ = core_name + "_r.jpg";
      pic3.title_name_ = core_name + "_z.jpg";
      pic1.Show();
      pic2.Show();
      pic3.Show();
    }
  }
}
