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
  public partial class LMOutput : Form
  {
    string target_path_;
    public LMOutput(string target_path, string title, string file1, string file2,
                    StringBuilder sb)
    {
      InitializeComponent();
      this.Text = title;
      target_path_ = target_path;
      AdditionalPicture additional_picture = new AdditionalPicture();
      //show standard outputs
      additional_picture.ShowPicture(pictureBox1, target_path_ + file1);
      additional_picture.ShowPicture(pictureBox2, target_path_ + file2);
      textBox1.Text = sb.ToString();
      textBox1.ScrollBars = ScrollBars.Both;
      textBox1.Select(0, 0);
    }
  }
}
