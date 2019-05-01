//https://stackoverflow.com/questions/36582543/
  //c-sharp-savefiledialog-with-custom-extensions
//https://goo.gl/dmS2H1
  //https://docs.microsoft.com/en-us/dotnet/
  //framework/winforms/controls/
  //how-to-save-files-using-the-savefiledialog-component

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace SPDepoGUI
{
  public partial class inputFileForm : Form
  {
    string path_;
    List<TextBox> all_boxes = new List<TextBox> { };
    List<TextBox> liquid_boxes;
    List<TextBox> gas_boxes;
    List<TextBox> pipe_boxes;
    List<TextBox> inlet_boxes;
    public inputFileForm()
    {
      InitializeComponent();
      path_ = AppDomain.CurrentDomain.BaseDirectory;
      liquid_boxes = new List<TextBox> { textBox1, textBox2, textBox3 };
      gas_boxes = new List<TextBox> { textBox4, textBox5, textBox6 };
      pipe_boxes = new List<TextBox> { textBox7, textBox8, 
                                       textBox9, textBox10, textBox11 };
      inlet_boxes = new List<TextBox> { textBox12, textBox13 };
      all_boxes.AddRange(liquid_boxes);
      all_boxes.AddRange(gas_boxes);
      all_boxes.AddRange(pipe_boxes);
      all_boxes.AddRange(inlet_boxes);
    }


    private void exitToolStripMenuItem_Click(object sender, EventArgs e)
    {
      this.Close();
    }

    private void button1_Click(object sender, EventArgs e)
    {
      MessageBox.Show("saving at " + path_ + "liquid_properties.txt");
      using(StreamWriter sw = 
          File.CreateText(path_ + "liquid_properties.txt")){
        foreach (var textbox in liquid_boxes){
          sw.WriteLine(textbox.Text);
        }
      }
    }

    private void button2_Click(object sender, EventArgs e)
    {
      MessageBox.Show("saving at " + path_ + "gas_properties.txt");
      using (StreamWriter sw =
          File.CreateText(path_ + "gas_properties.txt"))
      {
        foreach (var textbox in gas_boxes)
        {
          sw.WriteLine(textbox.Text);
        }
      }
    }

    private void button3_Click(object sender, EventArgs e)
    {
      MessageBox.Show("saving at " + path_ + "pipe_geometry.txt");
      using (StreamWriter sw =
          File.CreateText(path_ + "pipe_geometry.txt"))
      {
        foreach (var textbox in pipe_boxes)
        {
          sw.WriteLine(textbox.Text);
        }
      }
    }

    private void button4_Click(object sender, EventArgs e)
    {
      MessageBox.Show("saving at " + path_ + "inlet_condition.txt");
      using (StreamWriter sw =
          File.CreateText(path_ + "inlet_condition.txt"))
      {
        foreach (var textbox in inlet_boxes)
        {
          sw.WriteLine(textbox.Text);
        }
      }
    }

    private void lastSettingToolStripMenuItem_Click(object sender, EventArgs e)
    {
      try{
        using (StreamReader sr = new StreamReader("liquid_properties.txt"))
        {
          string line;
          int i = 0;
          while ((line = sr.ReadLine()) != null){
            liquid_boxes[i].Text = line;
            i++;
            if (i > liquid_boxes.Count){
              break;
            }
          }
        }
        using (StreamReader sr = new StreamReader("gas_properties.txt"))
        {
          string line;
          int i = 0;
          while ((line = sr.ReadLine()) != null)
          {
            gas_boxes[i].Text = line;
            i++;
            if (i > gas_boxes.Count)
            {
              break;
            }
          }
        }
        using (StreamReader sr = new StreamReader("pipe_geometry.txt"))
        {
          string line;
          int i = 0;
          while ((line = sr.ReadLine()) != null)
          {
            pipe_boxes[i].Text = line;
            i++;
            if (i > pipe_boxes.Count)
            {
              break;
            }
          }
        }
        using (StreamReader sr = new StreamReader("inlet_condition.txt"))
        {
          string line;
          int i = 0;
          while ((line = sr.ReadLine()) != null)
          {
            inlet_boxes[i].Text = line;
            i++;
            if (i > inlet_boxes.Count)
            {
              break;
            }
          }
        }
      }
      catch (Exception e_)
      {
        Console.WriteLine("The file could not be read:\n");
        Console.WriteLine(e_.Message);
      }

    }


    private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
    {
      SaveFileDialog saveFileDialog1 = new SaveFileDialog();
      saveFileDialog1.Filter = "Project (*.project)|*.project";
      saveFileDialog1.DefaultExt = "project";
      saveFileDialog1.AddExtension = true;
      saveFileDialog1.Title = "Save Project File";
      saveFileDialog1.ShowDialog();

      if (saveFileDialog1.FileName != "")
      {
        FileStream fs = (FileStream)saveFileDialog1.OpenFile();
        using (StreamWriter sw = new StreamWriter(fs))
        {
          int i = 0;
          while (true)
          {
            sw.WriteLine(all_boxes[i].Text);
            i++;
            if (i >= all_boxes.Count)
            {
              break;
            }
          }
        }
        MessageBox.Show(fs.Name + " is created");
      }
    }

    private void saveToolStripMenuItem_Click(object sender, EventArgs e)
    {
      using (StreamWriter sw = new StreamWriter("flow_pattern.project"))
      {
        int i = 0;
        while (true)
        {
          sw.WriteLine(all_boxes[i].Text);
          i++;
          if (i >= all_boxes.Count)
          {
            break;
          }
        }
      }
      MessageBox.Show("flow_pattern.project is created at " + path_);
    }

    private void customizeToolStripMenuItem_Click(object sender, EventArgs e)
    {
      button1_Click(sender, e);
      button2_Click(sender, e);
      button3_Click(sender, e);
      button4_Click(sender, e);
    }

    private void openToolStripMenuItem1_Click(object sender, EventArgs e)
    {
      OpenFileDialog openFileDialog1 = new OpenFileDialog();
      openFileDialog1.Filter = "Project (*.project)|*.project";
      openFileDialog1.FilterIndex = 1;
      openFileDialog1.Title = "Open Project File";
      openFileDialog1.ShowDialog();
      if (openFileDialog1.FileName != "")
      {
        FileStream fs = (FileStream)openFileDialog1.OpenFile();
        using (StreamReader sw = new StreamReader(fs))
        {
          int i = 0;
          while (true)
          {
            all_boxes[i].Text = sw.ReadLine();
            i++;
            if (i >= all_boxes.Count)
            {
              break;
            }
          }
        }
      }
    }

    private void instructionToolStripMenuItem_Click(object sender, EventArgs e)
    {
      input_instruction user_instruction = new input_instruction();
      user_instruction.Show();
    }

    private void textBox2_Validated(object sender, EventArgs e)
    {
      if(double.Parse(textBox2.Text) > 1000.0 ||
           double.Parse(textBox2.Text) < 0.0)
      {
        MessageBox.Show("warning the input value is >1000.0 or less than 0");
      }
      
    }
  }
}
