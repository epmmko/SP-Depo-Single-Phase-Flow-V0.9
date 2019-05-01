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
  public partial class AdditionalPicture : Form
  {
    public string title_name_ = "";
    public AdditionalPicture()
    {
      InitializeComponent();
    }
    public void ShowPicture(){
      Image image;
      using (var bmpTemp = new Bitmap(title_name_)){
        image = new Bitmap(bmpTemp);
      }
      pictureBox1.Image = image;
      pictureBox1.SizeMode = PictureBoxSizeMode.Zoom;
    }

    private void AdditionalPicture_Load(object sender, EventArgs e)
    {
      this.Text = title_name_;
      ShowPicture();
    }

    public void ShowPicture(PictureBox picture_box, string file_name){
      Image image;
      using (var bmpTemp = new Bitmap(file_name))
      {
        image = new Bitmap(bmpTemp);
      }
      picture_box.Image = image;
      picture_box.SizeMode = PictureBoxSizeMode.Zoom;
    }
  }
}
