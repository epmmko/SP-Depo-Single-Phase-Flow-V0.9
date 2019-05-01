using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics; //Stopwatch
using System.IO;

namespace SPDepoGUI
{
  public partial class HydrodynamicsSinglePhaseLiquid : Form
  {
    List<ComboBox> combobox_list_liquid_;
    List<TextBox> liquid_textbox_;
    List<TextBox> pipe_textbox_;
    List<TextBox> inlet_textbox_;
    bool box5_tick = false;
    bool box6_tick = false;
    public HydrodynamicsSinglePhaseLiquid()
    {
      InitializeComponent();
      combobox_list_liquid_ = new List<ComboBox> { comboBox1, comboBox2 };
      liquid_textbox_ = new List<TextBox> { textBox1, textBox2, textBox3 };
      pipe_textbox_ = new List<TextBox> { textBox4, textBox5, textBox6,
                                          textBox7, textBox8};
      inlet_textbox_ = new List<TextBox> { textBox9, textBox10,
                                           textBox11 };
    }

    private void toolStripMenuItem3_Click(object sender, EventArgs e)
    { //Save All
      SaveFileDialog saveFileDialog1 = new SaveFileDialog();
      saveFileDialog1.Filter = "hydro_all (*.HyAll)|*.HyAll";
      saveFileDialog1.DefaultExt = "HyAll";
      saveFileDialog1.AddExtension = true;
      saveFileDialog1.Title = "Save Single-phase Liquid Hydrodyanmics File";
      saveFileDialog1.ShowDialog();
      if (saveFileDialog1.FileName != "")
      {
        FileStream fs = (FileStream)saveFileDialog1.OpenFile();
        using (StreamWriter sw = new StreamWriter(fs))
        {
          foreach (var tb in liquid_textbox_)
          {
            sw.WriteLine(tb.Text);
          }
          foreach (var tb in pipe_textbox_)
          {
            sw.WriteLine(tb.Text);
          }
          foreach (var tb in inlet_textbox_)
          {
            sw.WriteLine(tb.Text);
          }
        }
        fs.Close();
        String file_name = saveFileDialog1.FileName;
        String file_name_no_extension = file_name;
        file_name_no_extension = file_name.Substring(0, file_name.Length - 5);

        using (StreamWriter sw = new StreamWriter(
                            file_name_no_extension + "HyLiq",false))
        {
          foreach (var tb in liquid_textbox_)
          {
            sw.WriteLine(tb.Text);
          }
        }
        using (StreamWriter sw = new StreamWriter(
                            file_name_no_extension + "HyPip",false))
        {
          foreach (var tb in pipe_textbox_)
          {
            sw.WriteLine(tb.Text);
          }
        }
        using (StreamWriter sw = new StreamWriter(
                            file_name_no_extension + "HyInL",false))
        {
          foreach (var tb in inlet_textbox_)
          {
            sw.WriteLine(tb.Text);
          }
        }

      }
    }

  

    private void panel1_Paint(object sender, PaintEventArgs e)
    {

    }

    private void defaultValue_Load(object sender, EventArgs e)
    {
      textBox1.Text = "default";
      textBox1.ForeColor = Color.Gray;
      textBox4.Text = "default";
      textBox4.ForeColor = Color.Gray;
      textBox9.Text = "default";
      textBox9.ForeColor = Color.Gray;
      foreach (var cbbox in combobox_list_liquid_)
      {
        cbbox.SelectedIndex = 0;
      }
      button5.Enabled = false;
      button6.Enabled = false;
      timer1.Start();
    }

    private void textBox4_TextChanged(object sender, EventArgs e)
    {

    }




    private void comboBox1_SelectedValueChanged(object sender, EventArgs e)
    {
      ComboBox cbbox = sender as ComboBox;
      if (cbbox.SelectedIndex == 1)
      {
        button5.Enabled = true;
        textBox2.Enabled = false;
        box5_tick = true;
      }
      else
      {
        button5.Enabled = false;
        textBox2.Enabled = true;
        box5_tick = false;
        button5.BackColor = default(Color);
      }
    }

    private void comboBox2_SelectedValueChanged(object sender, EventArgs e)
    {
      ComboBox cbbox = sender as ComboBox;
      if (cbbox.SelectedIndex == 1)
      {
        button6.Enabled = true;
        textBox3.Enabled = false;
        box6_tick = true;
      }
      else
      {
        button6.Enabled = false;
        textBox3.Enabled = true;
        box6_tick = false;
        button6.BackColor = default(Color);
      }
    }

    private void button5_Click(object sender, EventArgs e)
    {
      box5_tick = false;
      button5.BackColor = default(Color);
      BigEquationForm bigForm = new BigEquationForm(textBox2);
      bigForm.Show();
    }

    private void button6_Click(object sender, EventArgs e)
    {
      box6_tick = false;
      button6.BackColor = default(Color);
      BigEquationForm bigForm = new BigEquationForm(textBox3);
      bigForm.Show();
    }

    private void timer1_Tick(object sender, EventArgs e)
    {
      if (box5_tick)
      {
        if (button5.BackColor == Color.LightCyan)
        {
          button5.BackColor = default(Color);
        }
        else
        {
          button5.BackColor = Color.LightCyan;
        }
      }
      if (box6_tick)
      {
        if (button6.BackColor == Color.LightCyan)
        {
          button6.BackColor = default(Color);
        }
        else
        {
          button6.BackColor = Color.LightCyan;
        }
      }

    }

    private void pictureBox1_Click(object sender, EventArgs e)
    {

    }

    private void textBox1_Enter(object sender, EventArgs e)
    {
      textBox1.ForeColor = default(Color);
    }

    private void showResultsToolStripMenuItem_Click(object sender, EventArgs e)
    {
      ShowResultsForm resultForm = new ShowResultsForm("tzr.jpg");
      resultForm.Show();
    }

    private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
    {

    }

    private void toolTip1_Popup(object sender, PopupEventArgs e)
    {

    }

    private void liquidPhaseToolStripMenuItem_Click(object sender, EventArgs e)
    {
      SaveFileDialog saveFileDialog1 = new SaveFileDialog();
      saveFileDialog1.Filter = "hydro_liquid (*.HyLiq)|*.HyLiq";
      saveFileDialog1.DefaultExt = "HyLiq";
      saveFileDialog1.AddExtension = true;
      saveFileDialog1.Title = "Save Single-phase Liquid Properties File";
      saveFileDialog1.ShowDialog();
      if (saveFileDialog1.FileName != "")
      {
        FileStream fs = (FileStream)saveFileDialog1.OpenFile();
        using (StreamWriter sw = new StreamWriter(fs))
        {
          foreach (var tb in liquid_textbox_)
          {
            sw.WriteLine(tb.Text);
          }
        }
        fs.Close();
      }

    }

    private void textBox5_TextChanged(object sender, EventArgs e)
    {

    }

    private void pipeGeometryToolStripMenuItem_Click(object sender, EventArgs e)
    {
      SaveFileDialog saveFileDialog1 = new SaveFileDialog();
      saveFileDialog1.Filter = "hydro_pipe (*.HyPip)|*.HyPip";
      saveFileDialog1.DefaultExt = "HyPip";
      saveFileDialog1.AddExtension = true;
      saveFileDialog1.Title = "Save Single-phase Pipe Geometry File";
      saveFileDialog1.ShowDialog();
      if (saveFileDialog1.FileName != "")
      {
        FileStream fs = (FileStream)saveFileDialog1.OpenFile();
        using (StreamWriter sw = new StreamWriter(fs))
        {
          foreach (var tb in pipe_textbox_)
          {
            sw.WriteLine(tb.Text);
          }
        }
        fs.Close();
      }

    }

    private void inletConditionToolStripMenuItem_Click(object sender, EventArgs e)
    {
      SaveFileDialog saveFileDialog1 = new SaveFileDialog();
      saveFileDialog1.Filter = "hydro_inlet (*.HyInL)|*.HyInL";
      saveFileDialog1.DefaultExt = "HyInL";
      saveFileDialog1.AddExtension = true;
      saveFileDialog1.Title = "Save Single-phase Inlet Condition File";
      saveFileDialog1.ShowDialog();
      if (saveFileDialog1.FileName != "")
      {
        FileStream fs = (FileStream)saveFileDialog1.OpenFile();
        using (StreamWriter sw = new StreamWriter(fs))
        {
          foreach (var tb in inlet_textbox_)
          {
            sw.WriteLine(tb.Text);
          }
        }
        fs.Close();
      }
    }

    private void openAllToolStripMenuItem_Click(object sender, EventArgs e)
    {
      String opened_data;
      OpenFileDialog openFileDialog1 = new OpenFileDialog();
      openFileDialog1.Filter = "hydro_all (*.HyAll)|*.HyAll";
      openFileDialog1.DefaultExt = "HyAll";
      openFileDialog1.Title = "Open Single-phase Liquid Hydrodyanmics File";
      openFileDialog1.ShowDialog();
      if (openFileDialog1.FileName != "")
      {
        FileStream fs = (FileStream)openFileDialog1.OpenFile();
        using (StreamReader sr = new StreamReader(fs))
        {
          foreach (var tb in liquid_textbox_)
          {
            opened_data = sr.ReadLine();
            tb.Text = opened_data;
          }
          foreach (var tb in pipe_textbox_)
          {
            opened_data = sr.ReadLine();
            tb.Text = opened_data;
          }
          foreach (var tb in inlet_textbox_)
          {
            opened_data = sr.ReadLine();
            tb.Text = opened_data;
          }
        }
        fs.Close();
      }

    }

    private void SaveLastOption()
    {
      using (StreamWriter sw = new StreamWriter("LastSetting.HyAll", false))
      {
        foreach (var tb in liquid_textbox_)
        {
          sw.WriteLine(tb.Text);
        }
        foreach (var tb in pipe_textbox_)
        {
          sw.WriteLine(tb.Text);
        }
        foreach (var tb in inlet_textbox_)
        {
          sw.WriteLine(tb.Text);
        }
      }
    }

    private void saveAndExitToolStripMenuItem_Click(object sender, EventArgs e)
    {
      SaveLastOption();
      this.Close();
    }

    private void toolStripMenuItem5_Click(object sender, EventArgs e)
    {
      this.Close();
    }

    private void textBox4_Enter(object sender, EventArgs e)
    {
      textBox4.ForeColor = default(Color);
    }

    private void textBox9_Enter(object sender, EventArgs e)
    {
      textBox9.ForeColor = default(Color);
    }

    private void lastSettingToolStripMenuItem_Click(object sender, EventArgs e)
    {
      String opened_data;
      using (StreamReader sr = new StreamReader("LastSetting.HyAll"))
      {
        foreach (var tb in liquid_textbox_)
        {
          opened_data = sr.ReadLine();
          tb.Text = opened_data;
        }
        foreach (var tb in pipe_textbox_)
        {
          opened_data = sr.ReadLine();
          tb.Text = opened_data;
        }
        foreach (var tb in inlet_textbox_)
        {
          opened_data = sr.ReadLine();
          tb.Text = opened_data;
        }
      }
    }

    private void liquidPropertiesToolStripMenuItem_Click(object sender, EventArgs e)
    {
      String opened_data;
      OpenFileDialog openFileDialog1 = new OpenFileDialog();
      openFileDialog1.Filter = "hydro_liquid (*.HyLiq)|*.HyLiq";
      openFileDialog1.DefaultExt = "HyLiq";
      openFileDialog1.Title = "Open Single-phase Liquid Properties File";
      openFileDialog1.ShowDialog();
      if (openFileDialog1.FileName != "")
      {
        FileStream fs = (FileStream)openFileDialog1.OpenFile();
        using (StreamReader sr = new StreamReader(fs))
        {
          foreach (var tb in liquid_textbox_)
          {
            opened_data = sr.ReadLine();
            tb.Text = opened_data;
          }
        }
        fs.Close();
      }
    }

    private void pipeGeometryToolStripMenuItem1_Click(object sender, EventArgs e)
    {
      String opened_data;
      OpenFileDialog openFileDialog1 = new OpenFileDialog();
      openFileDialog1.Filter = "hydro_pipe (*.HyPip)|*.HyPip";
      openFileDialog1.DefaultExt = "HyPip";
      openFileDialog1.Title = "Open Single-phase Pipe Geometry File";
      openFileDialog1.ShowDialog();
      if (openFileDialog1.FileName != "")
      {
        FileStream fs = (FileStream)openFileDialog1.OpenFile();
        using (StreamReader sr = new StreamReader(fs))
        {
          foreach (var tb in pipe_textbox_)
          {
            opened_data = sr.ReadLine();
            tb.Text = opened_data;
          }
        }
        fs.Close();
      }
    }

    private void inletConditionToolStripMenuItem1_Click(object sender, EventArgs e)
    {
      String opened_data;
      OpenFileDialog openFileDialog1 = new OpenFileDialog();
      openFileDialog1.Filter = "hydro_inlet (*.HyInL)|*.HyInL";
      openFileDialog1.DefaultExt = "HyInL";
      openFileDialog1.Title = "Open Single-phase Inlet Condition File";
      openFileDialog1.ShowDialog();
      if (openFileDialog1.FileName != "")
      {
        FileStream fs = (FileStream)openFileDialog1.OpenFile();
        using (StreamReader sr = new StreamReader(fs))
        {
          foreach (var tb in inlet_textbox_)
          {
            opened_data = sr.ReadLine();
            tb.Text = opened_data;
          }
        }
        fs.Close();
      }
    }

    private void instructionToolStripMenuItem_Click(object sender, EventArgs e)
    {

    }

    private void createAnInputFileToolStripMenuItem_Click(object sender, EventArgs e)
    {
      String file_name_no_extension = "default_Hy1L_.";
      using (StreamWriter sw = new StreamWriter(
                          file_name_no_extension + "HyLiq",false))
      {
        foreach (var tb in liquid_textbox_)
        {
          sw.WriteLine(tb.Text);
        }
      }
      using (StreamWriter sw = new StreamWriter(
                          file_name_no_extension + "HyPip",false))
      {
        foreach (var tb in pipe_textbox_)
        {
          sw.WriteLine(tb.Text);
        }
      }
      using (StreamWriter sw = new StreamWriter(
                          file_name_no_extension + "HyInL",false))
      {
        foreach (var tb in inlet_textbox_)
        {
          sw.WriteLine(tb.Text);
        }
      }
    }

    private void performTheCalculationToolStripMenuItem_Click(object sender, EventArgs e)
    {
      String path_ = AppDomain.CurrentDomain.BaseDirectory;
      ProcessStartInfo start = new ProcessStartInfo();
      start.FileName = "SPDepo.exe";
      start.WindowStyle = ProcessWindowStyle.Normal;
      start.Arguments = "hydro_1_liquid " + 
                        "default_Hy1L_.HyLiq " + 
                        "default_Hy1L_.HyPip " + 
                        "default_Hy1L_.InL";
      int exit_code;
      using (Process proc = Process.Start(start))
      {
        proc.WaitForExit();
        exit_code = proc.ExitCode;
      }

    }
  }
}
