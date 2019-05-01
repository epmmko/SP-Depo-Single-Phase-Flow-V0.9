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
  public partial class Deposition1POutput : Form
  {
    string target_path_;
    public Deposition1POutput(string target_path, string title, string file1, string file2)
    //pictureBox1 = "delta_mm_average.jpg"
    //pictureBox2 = "
    {
      InitializeComponent();
      this.Text = title;
      target_path_ = target_path;
      AdditionalPicture additional_picture = new AdditionalPicture();
      //show standard outputs
      additional_picture.ShowPicture(pictureBox1, target_path_ + file1);
      additional_picture.ShowPicture(pictureBox2, target_path_ + file2);
    }

    private void Deposition1POutput_Load(object sender, EventArgs e)
    {

    }
  }
}
