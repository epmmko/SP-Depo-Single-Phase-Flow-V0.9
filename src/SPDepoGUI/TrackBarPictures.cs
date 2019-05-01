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
  public partial class TrackBarPictures : Form
  {
    List<string> file_name_;
    AdditionalPicture additionalPicture_;
    public TrackBarPictures(string title, string name_first_part_with_path, string name_last_part, int number_of_pattern)
    {
      InitializeComponent();
      this.Text = title;
      trackBar1.Maximum = number_of_pattern - 1;
      trackBar1.Minimum = 0;
      file_name_ = new List<string>();
      additionalPicture_ = new AdditionalPicture();
      for (int i = 0; i < number_of_pattern; i++){
        file_name_.Add(name_first_part_with_path + i + name_last_part);
      }
      additionalPicture_.ShowPicture(pictureBox1, file_name_[0]);
    }

    private void trackBar1_Scroll(object sender, EventArgs e)
    {
      TrackBar trackBar = sender as TrackBar;
      additionalPicture_.ShowPicture(pictureBox1, file_name_[trackBar1.Value]);
    }
  }
}
