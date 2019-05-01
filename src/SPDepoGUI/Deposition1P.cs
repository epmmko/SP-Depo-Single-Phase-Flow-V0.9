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
  public partial class Deposition1P : Form
  {
    string exePath_;
    string outputPath_;
    List<ComboBox> combobox_list_liquid_;
    List<ComboBox> combobox_list_surrounding_;
    List<TextBox> liquid_textbox_;
    List<TextBox> pipe_textbox_;
    List<TextBox> inlet_textbox_;
    List<TextBox> surrounding_textbox_loop_case_;
    List<TextBox> surrounding_textbox_field_case_;
    List<TextBox> thermo_prop_surrounding_tab_;
    List<TextBox> surrounding_fluid_surrounding_tab_;
    List<CheckBox> model_tuning_tab_;
    List<TextBox> model_tuning_text_box_;
    bool box5_tick_ = false;
    bool box6_tick_ = false;
    bool box2_tick_ = false;
    bool box1_tick_ = false;
    //Edit Eq blink >> Pipe Properties
    bool box30_tick_ = false;
    //Edit Eq blink >> Surrounding
    bool box9_tick_ = false;
    bool box8_tick_ = false;
    bool box7_tick_ = false;
    bool box4_tick_ = false;
    //Edit Eq blink >> Simulation option
    bool box20_tick_ = false;
    bool box18_tick_ = false;
    bool box21_tick_ = false;

    public void OpenProject(string project_hy_all)
    { //input string is *.hyAll
      //this is to be called from the main form when open a project
      if (project_hy_all != "")
      {
        String file_name = project_hy_all;
        String file_name_no_extension = file_name;
        file_name_no_extension = file_name.Substring(0, file_name.Length - 5);
        OpenLiquidPropertiesTab(file_name_no_extension + "HyLiq");
        OpenPipePropertiesTab(file_name_no_extension + "HyPip");
        OpenInletCondition(file_name_no_extension + "HyInL");
        OpenSurroundingTabFlowLoopCase(file_name_no_extension + "SurFl");
        OpenSurroundingTabFieldCase(file_name_no_extension + "SurFd");
        OpenSimulationOption(file_name_no_extension + "SimOp");
        OpenModelTuningTab(file_name_no_extension + "MTune");
      }
    }

      public Deposition1P(FolderBrowserDialog folderBrowserDialog1, string exePath)
    {
      InitializeComponent();
      this.Text = "Deposition Single-Phase:" + folderBrowserDialog1.SelectedPath;
      outputPath_ = folderBrowserDialog1.SelectedPath + "\\";
      exePath_ = exePath;
      Directory.SetCurrentDirectory(outputPath_);
      Directory.CreateDirectory("inputs");
      combobox_list_liquid_ = new List<ComboBox> { comboBox1, comboBox2 };
      liquid_textbox_ = new List<TextBox> { textBox1, textBox2, textBox3, textBox13, textBox12 };
      pipe_textbox_ = new List<TextBox> { textBox4, textBox5, textBox35, textBox6,
                                          textBox7, textBox8, textBox61, textBox55};
      inlet_textbox_ = new List<TextBox> { textBox9, textBox10,
                                           textBox11};
      combobox_list_surrounding_ = new List<ComboBox> { comboBox9, comboBox8, comboBox7, comboBox6, comboBox5,
        comboBox15, comboBox14, comboBox13, comboBox12, comboBox11};
      surrounding_textbox_loop_case_ = new List<TextBox> {
        textBox34, textBox17, textBox16, textBox15, textBox14, textBox54,
        textBox23, textBox22, textBox21, textBox20, textBox19};
      //has only the consecutive textbox
      //The first one that are separated by a combobox is not included
      surrounding_textbox_field_case_ = new List<TextBox> {
        textBox32, textBox31, textBox24, textBox30, textBox18,
        textBox29, textBox28, textBox27, textBox26, textBox25};
      //These are the contiguous textBox only.
      //The first two that are separated by a combobox is not included
      thermo_prop_surrounding_tab_ = new List<TextBox>{
        textBox23, textBox22, textBox21, textBox20, textBox19};
      surrounding_fluid_surrounding_tab_ = new List<TextBox>{
        textBox29, textBox28, textBox27, textBox26, textBox25};
      model_tuning_tab_ = new List<CheckBox> {
        mcheckBox1, mcheckBox2, mcheckBox3, mcheckBox4, mcheckBox5, mcheckBox6,
        mcheckBox7, mcheckBox8, mcheckBox9, mcheckBox10, mcheckBox11,
        mcheckBox12, mcheckBox13, mcheckBox14, mcheckBox15 };
      model_tuning_text_box_ = new List<TextBox> {
        mtextBox1, mtextBox2, mtextBox3, mtextBox4};

      textBox1.Text = "default";
      textBox1.ForeColor = Color.Gray;
      textBox4.Text = "default";
      textBox4.ForeColor = Color.Gray;
      textBox9.Text = "default";
      textBox9.ForeColor = Color.Gray;
      textBox23.Text = "default";
      textBox23.ForeColor = Color.Gray;
      textBox29.Text = "default";
      textBox29.ForeColor = Color.Gray;
      foreach (var cbbox in combobox_list_liquid_)
      {
        cbbox.SelectedIndex = 0;
      }
      button5.Enabled = false;
      button6.Enabled = false;
      timer1.Start();
      //initial setting for pipe properties
      comboBox16.SelectedIndex = 0;
      //initial setting for Surrounding tab
      radioButton5.Checked = true;
      groupBox2.Enabled = true;
      groupBox6.Enabled = false;
      textBox14.Enabled = false;
      foreach (var cb in combobox_list_surrounding_)
      {
        cb.SelectedIndex = 0;
      }
      comboBox15.SelectedIndex = 1;
      //default pipe is not buried

      //set Simulation Options tab
      radioButton3.Checked = true;
      comboBox20.SelectedIndex = 1;
      comboBox21.SelectedIndex = 1;
      comboBox22.SelectedIndex = 0;
      radioButton7.Checked = true;
      radioButton10.Checked = true;
      //set model tuning tab
      mtextBox1.Text = "0.001";
      mtextBox2.Text = "0.000001";
      mtextBox3.Text = "1.0";
      mtextBox4.Text = "50";
      //set comment tab
      textBox63.ScrollBars = ScrollBars.Both;

    }
    private void SaveLiquidPropertiesTab(string file_name)
    {
      using (StreamWriter sw = new StreamWriter(file_name, false))
      {
        foreach (var tb in liquid_textbox_)
        {
          sw.WriteLine(tb.Text);
        }
      }
    }
    private void OpenLiquidPropertiesTab(string file_name)
    {
      using (StreamReader sr = new StreamReader(file_name))
      {
        foreach (var tb in liquid_textbox_)
        {
          tb.Text = sr.ReadLine();
        }
      }
    }
    private void SavePipePropertiesTab(string file_name)
    {
      using (StreamWriter sw = new StreamWriter(file_name, false))
      {
        foreach (var tb in pipe_textbox_)
        {
          sw.WriteLine(tb.Text);
        }
      }
    }
    private void OpenPipePropertiesTab(string file_name)
    {
      using (StreamReader sr = new StreamReader(file_name))
      {
        foreach (var tb in pipe_textbox_)
        {
          tb.Text = sr.ReadLine();
        }
      }
    }
    private void SaveInletCondition(string file_name)
    {
      using (StreamWriter sw = new StreamWriter(file_name, false))
      {
        foreach (var tb in inlet_textbox_)
        {
          sw.WriteLine(tb.Text);
        }
      }
    }
    private void OpenInletCondition(string file_name)
    {
      using (StreamReader sr = new StreamReader(file_name))
      {
        foreach (var tb in inlet_textbox_)
        {
          tb.Text = sr.ReadLine();
        }
      }
    }
    private void SaveSurroundingTabFlowLoopCase(string file_name)
    {
      using (StreamWriter sw = new StreamWriter(file_name, false))
      {
        List<string> loop_case_str_vec = GetSurroundingFlowLoopCaseString();
        foreach (var text in loop_case_str_vec)
        {
          sw.WriteLine(text);
        }
      }
    }
    private void OpenSurroundingTabFlowLoopCase(string file_name)
    {
      List<string> input_list = new List<string> { };
      using (StreamReader sr = new StreamReader(file_name))
      {
        while (!sr.EndOfStream)
        {
          input_list.Add(sr.ReadLine());
        }
      }
      SetSurroundingFlowLoopCaseFromString(input_list);
    }
    private void SaveSurroundingTabFieldCase(string file_name)
    {
      using (StreamWriter sw = new StreamWriter(file_name, false))
      {
        List<string> field_case_str_vec = GetSurroundingFieldCaseString();
        foreach (var text in field_case_str_vec)
        {
          sw.WriteLine(text);
        }
      }
    }
    private void OpenSurroundingTabFieldCase(string file_name)
    {
      List<string> input_list = new List<string> { };
      using (StreamReader sr = new StreamReader(file_name))
      {
        while (!sr.EndOfStream)
        {
          input_list.Add(sr.ReadLine());
        }
      }
      SetSurroundingFieldCaseFromString(input_list);
    }
    private void SaveSimulationOption(string file_name)
    {
      using (StreamWriter sw = new StreamWriter(file_name, false))
      {
        List<string> text_list = GetSimulationOptions();
        foreach (var text in text_list)
        {
          sw.WriteLine(text);
        }
      }
    }
    private void OpenSimulationOption(string file_name)
    {
      List<string> input_list = new List<string> { };
      using (StreamReader sr = new StreamReader(file_name))
      {
        while (!sr.EndOfStream)
        {
          input_list.Add(sr.ReadLine());
        }
      }
      SetSimulationOptions(input_list);
    }
    private void SaveModelTuningTab(string file_name)
    {
      int n_row = -1;
      int n_col = dataGridView1.ColumnCount;
      //count number of row that has data in dataGridView1
      foreach (DataGridViewRow row in dataGridView1.Rows)
      {
        n_row++;
        int empty_cell_count = 0;
        for (int j = 0; j < n_col; j++)
        {
          if (row.Cells[j].Value == null ||
              row.Cells[j].Value == DBNull.Value ||
              String.IsNullOrWhiteSpace(
                row.Cells[j].Value.ToString()))
          {
            empty_cell_count++;
          }
          else
          {
            break;
          }
        }
        if (empty_cell_count == n_col)
        {
          break;
        }
      }

      using (StreamWriter sw = new StreamWriter(file_name, false))
      {
        //read data from checkBox and write to a text file
        for (int i = 0; i < model_tuning_tab_.Count; i++)
        {
          if (model_tuning_tab_[i].Checked)
          {
            sw.WriteLine("true");
          }
          else
          {
            sw.WriteLine("false");
          }
        }
        //read data from textBox and write to a text file
        for (int i = 0; i < model_tuning_text_box_.Count; i++)
        {
          sw.WriteLine(model_tuning_text_box_[i].Text);
        }
        //read data from dataGridView and write to a text file
        for (int i = 0; i < n_row; i++)
        {
          string row_str = "";
          for (int j = 0; j < n_col; j++)
          {
            if (dataGridView1.Rows[i].Cells[j].Value == null ||
                         dataGridView1.Rows[i].Cells[j].Value == DBNull.Value ||
                         String.IsNullOrWhiteSpace(dataGridView1.Rows[i]
                           .Cells[j].Value.ToString()))
            {
            }
            else
            {
              row_str += dataGridView1.Rows[i].Cells[j].Value.ToString();
            }
            if (j < n_col - 1)
            {
              row_str += ",";
            }
          }
          sw.WriteLine(row_str);
        }
      }
    }
    private void SaveAll(string file_name)
    {
      using (StreamWriter sw = new StreamWriter(file_name, false))
      {
        List<string> text_list = GetSaveAll(file_name);
        foreach (var text in text_list)
        {
          sw.WriteLine(text);
        }
      }
    }

    private void OpenModelTuningTab(string file_name)
    {
      List<string> input_list = new List<string> { };
      using (StreamReader sr = new StreamReader(file_name))
      {
        while (!sr.EndOfStream)
        {
          input_list.Add(sr.ReadLine());
        }
      }
      SetModelTuning(input_list);
    }

    private List<string> GetCalculationOptions()
    {
      List<string> ans = new List<string> { };
      if (checkBox7.Checked)
      {
        ans.Add("true");
      }
      else
      {
        ans.Add("false");
      }
      ans.Add(textBox47.Text);
      ans.Add(textBox46.Text);
      ans.Add(textBox48.Text);
      ans.Add(textBox45.Text);
      ans.Add(textBox50.Text);
      ans.Add(textBox43.Text);
      ans.Add(textBox49.Text);
      ans.Add(textBox51.Text);
      if (checkBox6.Checked)
      {
        ans.Add("true");
      }
      else
      {
        ans.Add("false");
      }
      return ans;
    }

    private List<string> GetSimulationOptions()
    {
      //iterate through Simulation Options tab by
      //going through each groupBox from left column, then right column
      //going through the element inside the group box by
      // left column, then right column
      List<string> ans = new List<string> { };
      ans.Add(textBox39.Text);
      ans.Add(comboBox20.SelectedIndex.ToString());
      ans.Add(comboBox21.SelectedIndex.ToString());
      ans.Add(comboBox22.SelectedIndex.ToString());
      List<RadioButton> rb_list = new List<RadioButton>{
        radioButton1, radioButton2, radioButton3, radioButton4};
      for (int i = 0; i < rb_list.Count; i++)
      {
        if (rb_list[i].Checked)
        {
          ans.Add(i.ToString());
          break;
        }
      }
      ans.Add(textBox38.Text);
      ans.Add(textBox33.Text);
      ans.Add(textBox42.Text);
      ans.Add(textBox40.Text);
      List<RadioButton> rb_list2 = new List<RadioButton>{
        radioButton7, radioButton8, radioButton9};
      for (int i = 0; i < rb_list2.Count; i++)
      {
        if (rb_list2[i].Checked)
        {
          ans.Add(i.ToString());
          break;
        }
      }
      ans.Add(textBox37.Text);
      ans.Add(textBox41.Text);
      ans.Add(textBox57.Text);
      List<RadioButton> rb_list3 = new List<RadioButton>{
        radioButton10, radioButton11};
      for (int i = 0; i < rb_list3.Count; i++)
      {
        if (rb_list3[i].Checked)
        {
          ans.Add(i.ToString());
          break;
        }
      }
      ans.Add(textBox36.Text);
      ans.Add(textBox44.Text);
      return ans;
    }
    private void SetSimulationOptions(List<string> input_list)
    {
      textBox39.Text = input_list[0];
      comboBox20.SelectedIndex = Int32.Parse(input_list[1]);
      comboBox21.SelectedIndex = Int32.Parse(input_list[2]);
      comboBox22.SelectedIndex = Int32.Parse(input_list[3]);
      List<RadioButton> rb_list = new List<RadioButton>{
        radioButton1, radioButton2, radioButton3, radioButton4};
      rb_list[Int32.Parse(input_list[4])].Checked = true;
      textBox38.Text = input_list[5];
      textBox33.Text = input_list[6];
      textBox42.Text = input_list[7];
      textBox40.Text = input_list[8];
      List<RadioButton> rb_list2 = new List<RadioButton>{
        radioButton7, radioButton8, radioButton9};
      rb_list2[Int32.Parse(input_list[9])].Checked = true;
      textBox37.Text = input_list[10];
      textBox41.Text = input_list[11];
      textBox57.Text = input_list[12];
      List<RadioButton> rb_list3 = new List<RadioButton>{
        radioButton10, radioButton11};
      rb_list3[Int32.Parse(input_list[13])].Checked = true;
      textBox36.Text = input_list[14];
      textBox44.Text = input_list[15];
    }
    private void SetModelTuning(List<string> input_list)
    {
      int n_col = dataGridView1.ColumnCount;
      int n_check_box = model_tuning_tab_.Count;
      int n_text_box = model_tuning_text_box_.Count;
      for (int i = 0; i < n_check_box; i++)
      {
        if (input_list[i] == "true")
        {
          model_tuning_tab_[i].Checked = true;
        }
        else
        {
          model_tuning_tab_[i].Checked = false;
        }
      }
      for (int i = n_check_box; i < n_check_box + n_text_box; i++)
      {
        model_tuning_text_box_[i - n_check_box].Text = input_list[i];
      }
      List<string> row_data = new List<string>();
      dataGridView1.Rows.Clear();
      dataGridView1.Refresh();
      for (int i = n_check_box + n_text_box; i < input_list.Count; i++)
      {
        dataGridView1.Rows.Add();
        row_data = (input_list[i].Split(',').ToList());
        for (int j = 0; j < n_col; j++)
        {
          if (row_data[j] != "")
          {
            dataGridView1.Rows[i - n_check_box - n_text_box]
              .Cells[j].Value = double.Parse(row_data[j]);
          }
        }
      }
    }

    private List<string> GetSurroundingFieldCaseString()
    {
      List<string> ans = new List<string> { };
      if (radioButton6.Checked)
      {
        ans.Add("true");
      }
      else
      {
        ans.Add("false");
      }
      ans.Add(textBox53.Text);
      ans.Add(textBox60.Text);
      ans.Add(comboBox15.SelectedIndex.ToString());
      foreach (var tb in surrounding_textbox_field_case_)
      {
        ans.Add(tb.Text);
      }
      return ans;
    }
    private void SetSurroundingFieldCaseFromString(List<string> input_list)
    {
      if (input_list[0] == "true")
      {
        radioButton6.Checked = true;
      }
      else
      {
        radioButton6.Checked = false;
      }
      textBox53.Text = input_list[1];
      textBox60.Text = input_list[2];
      int num;
      bool success = Int32.TryParse(input_list[2], out num);
      if (success)
      {
        comboBox15.SelectedItem = num;
      }

      int n = surrounding_textbox_field_case_.Count;
      int tb_index;
      for (int i = 4; i < n; i++)
      {
        tb_index = i - 4;
        surrounding_textbox_field_case_[tb_index].Text = input_list[i];
      }
    }

    private List<string> GetSurroundingFlowLoopCaseString()
    {
      List<string> ans = new List<string> { };
      if (radioButton5.Checked)
      {
        ans.Add("true");
      }
      else
      {
        ans.Add("false");
      }
      ans.Add(textBox52.Text);
      ans.Add(comboBox9.SelectedIndex.ToString());
      foreach (var tb in surrounding_textbox_loop_case_)
      {
        ans.Add(tb.Text);
      }
      return ans;
    }
    private void SetSurroundingFlowLoopCaseFromString(List<string> input_list)
    {
      if (input_list[0] == "true")
      {
        radioButton5.Checked = true;
      }
      else
      {
        radioButton5.Checked = false;
      }
      textBox52.Text = input_list[1];
      //https://stackoverflow.com/questions/450059/
      //how-do-i-set-the-selected-item-in-a-combobox-to-match-my-string-using-c
      int num;
      bool success = Int32.TryParse(input_list[1], out num);
      if (success)
      {
        comboBox9.SelectedItem = num;
      }


      int n = input_list.Count;
      int tb_index;
      for (int i = 3; i < n; i++)
      {
        tb_index = i - 3;
        surrounding_textbox_loop_case_[tb_index].Text = input_list[i];
      }
    }

    private List<string> GetSaveAll(string file_name)
    {
      string file_name_no_extension = file_name.Substring(0, file_name.Length - 5);
      List<string> ans = new List<string> {
        "Saved Data\n",
        "Liquid properties definition: " + file_name_no_extension + "HyLiq\n",
        "Pipe properties definition: " + file_name_no_extension + "HyPip\n",
        "Inlet condition definition: " + file_name_no_extension + "HyInL\n",
        "Surrounding flow loop case definition: " + file_name_no_extension + "SurFl\n",
        "Surrounding field case definition: " + file_name_no_extension + "SurFd\n",
        "Simulation option definition: " + file_name_no_extension + "SimOp\n",
        "Model Tuning definition: " + file_name_no_extension + "MTune\n"
        };
      return ans;
    }
    private void toolStripMenuItem3_Click(object sender, EventArgs e)
    { //Save All
      SaveFileDialog saveFileDialog1 = new SaveFileDialog();
      Directory.CreateDirectory(outputPath_ + @"inputs\");
      try
      {
        saveFileDialog1.InitialDirectory = outputPath_ + @"inputs\";
      }
      catch
      {
      }

      saveFileDialog1.Filter = "hydro_all (*.HyAll)|*.HyAll";
      saveFileDialog1.DefaultExt = "HyAll";
      saveFileDialog1.AddExtension = true;
      saveFileDialog1.Title = "Save file bundle in all extension with the same name";
      saveFileDialog1.ShowDialog();
      if (saveFileDialog1.FileName != "")
      {
        String file_name = saveFileDialog1.FileName;
        String file_name_no_extension = file_name;
        file_name_no_extension = file_name.Substring(0, file_name.Length - 5);

        SaveAll(file_name);
        SaveLiquidPropertiesTab(file_name_no_extension + "HyLiq");
        SavePipePropertiesTab(file_name_no_extension + "HyPip");
        SaveInletCondition(file_name_no_extension + "HyInL");
        SaveSurroundingTabFlowLoopCase(file_name_no_extension + "SurFl");
        SaveSurroundingTabFieldCase(file_name_no_extension + "SurFd");
        SaveSimulationOption(file_name_no_extension + "SimOp");
        SaveModelTuningTab(file_name_no_extension + "MTune");

      }
    }



    private void panel1_Paint(object sender, PaintEventArgs e)
    {

    }

    private void defaultValue_Load(object sender, EventArgs e)
    {

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
        box5_tick_ = true;
      }
      else
      {
        button5.Enabled = false;
        textBox2.Enabled = true;
        box5_tick_ = false;
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
        box6_tick_ = true;
      }
      else
      {
        button6.Enabled = false;
        textBox3.Enabled = true;
        box6_tick_ = false;
        button6.BackColor = default(Color);
      }
    }

    private void comboBox4_SelectedValueChanged(object sender, EventArgs e)
    {
      ComboBox cbbox = sender as ComboBox;
      if (cbbox.SelectedIndex == 1)
      {
        button2.Enabled = true;
        textBox13.Enabled = false;
        box2_tick_ = true;
      }
      else
      {
        button2.Enabled = false;
        textBox13.Enabled = true;
        box2_tick_ = false;
        button2.BackColor = default(Color);
      }
    }

    private void comboBox3_SelectedValueChanged(object sender, EventArgs e)
    {
      ComboBox cbbox = sender as ComboBox;
      if (cbbox.SelectedIndex == 1)
      {
        button1.Enabled = true;
        textBox12.Enabled = false;
        box1_tick_ = true;
      }
      else
      {
        button1.Enabled = false;
        textBox12.Enabled = true;
        box1_tick_ = false;
        button1.BackColor = default(Color);
      }
    }

    private void comboBox8_SelectedValueChanged(object sender, EventArgs e)
    { //cbb >> density >> coolant prop >> surrounding
      ComboBox cbbox = sender as ComboBox;
      if (cbbox.SelectedIndex == 1)
      {
        button9.Enabled = true;
        textBox22.Enabled = false;
        box9_tick_ = true;
      }
      else
      {
        button9.Enabled = false;
        textBox22.Enabled = true;
        box9_tick_ = false;
        button9.BackColor = default(Color);
      }
    }
    private void comboBox7_SelectedValueChanged(object sender, EventArgs e)
    { //cbb >> density >> coolant prop >> surrounding
      ComboBox cbbox = sender as ComboBox;
      if (cbbox.SelectedIndex == 1)
      {
        button8.Enabled = true;
        textBox21.Enabled = false;
        box8_tick_ = true;
      }
      else
      {
        button8.Enabled = false;
        textBox21.Enabled = true;
        box8_tick_ = false;
        button8.BackColor = default(Color);
      }
    }
    private void comboBox6_SelectedIndexChanged(object sender, EventArgs e)
    { //cbb >> thermal cond >> coolant prop >> surrounding
      ComboBox cbbox = sender as ComboBox;
      if (cbbox.SelectedIndex == 1)
      {
        button7.Enabled = true;
        textBox20.Enabled = false;
        box7_tick_ = true;
      }
      else
      {
        button7.Enabled = false;
        textBox20.Enabled = true;
        box7_tick_ = false;
        button7.BackColor = default(Color);
      }
    }
    private void comboBox5_SelectedIndexChanged(object sender, EventArgs e)
    { //cbb >> thermal cond >> coolant prop >> surrounding
      ComboBox cbbox = sender as ComboBox;
      if (cbbox.SelectedIndex == 1)
      {
        button4.Enabled = true;
        textBox19.Enabled = false;
        box4_tick_ = true;
      }
      else
      {
        button4.Enabled = false;
        textBox19.Enabled = true;
        box4_tick_ = false;
        button4.BackColor = default(Color);
      }
    }
    private void button5_Click(object sender, EventArgs e)
    {
      //"Edit Equation" Density >> Liquid Properties
      box5_tick_ = false;
      button5.BackColor = default(Color);
      BigEquationForm bigForm = new BigEquationForm(textBox2);
      bigForm.Show();
    }

    private void button6_Click(object sender, EventArgs e)
    {
      //"Edit Equation" Viscosity >> Liquid Properties
      box6_tick_ = false;
      button6.BackColor = default(Color);
      BigEquationForm bigForm = new BigEquationForm(textBox3);
      bigForm.Show();
    }

    private void button2_Click(object sender, EventArgs e)
    {
      //"Edit Equation" Thermal Conductivity >> Liquid Properties
      box2_tick_ = false;
      button2.BackColor = default(Color);
      BigEquationForm bigForm = new BigEquationForm(textBox3);
      bigForm.Show();
    }

    private void button1_Click(object sender, EventArgs e)
    {
      //"Edit Equation" Specific Heat Capacity >> Liquid Properties
      box1_tick_ = false;
      button1.BackColor = default(Color);
      BigEquationForm bigForm = new BigEquationForm(textBox3);
      bigForm.Show();
    }
    private void button30_Click(object sender, EventArgs e)
    {
      //"Edit Equation" Pipe Material >> Pipe Properties
      box30_tick_ = false;
      button30.BackColor = default(Color);
      BigEquationForm bigForm = new BigEquationForm(textBox55);
      bigForm.Show();
    }

    private void button9_Click(object sender, EventArgs e)
    {
      //"Edit Equation" Density >> Coolant Properties >> Surrounding
      box9_tick_ = false;
      button9.BackColor = default(Color);
      BigEquationForm bigForm = new BigEquationForm(textBox22);
      bigForm.Show();
    }
    private void button8_Click(object sender, EventArgs e)
    {
      //"Edit Equation" Viscosity >> Coolant Properties >> Surrounding
      box8_tick_ = false;
      button8.BackColor = default(Color);
      BigEquationForm bigForm = new BigEquationForm(textBox21);
      bigForm.Show();
    }
    private void button7_Click(object sender, EventArgs e)
    {
      //"Edit Equation" thermal cond >> Coolant Properties >> Surrounding
      box7_tick_ = false;
      button7.BackColor = default(Color);
      BigEquationForm bigForm = new BigEquationForm(textBox20);
      bigForm.Show();
    }
    private void button4_Click(object sender, EventArgs e)
    {
      //"Edit Equation" cp >> Coolant Properties >> Surrounding
      box4_tick_ = false;
      button4.BackColor = default(Color);
      BigEquationForm bigForm = new BigEquationForm(textBox19);
      bigForm.Show();
    }
    private void button20_Click(object sender, EventArgs e)
    {
      //Edit Equation >> fs(T) Eq >> Simulation Option
      box20_tick_ = false;
      button20.BackColor = default(Color);
      BigEquationForm bigForm = new BigEquationForm(textBox38);
      bigForm.Show();
    }


    private void timer1_Tick(object sender, EventArgs e)
    {
      if (box5_tick_)
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
      if (box6_tick_)
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
      if (box2_tick_)
      {
        if (button2.BackColor == Color.LightCyan)
        {
          button2.BackColor = default(Color);
        }
        else
        {
          button2.BackColor = Color.LightCyan;
        }
      }
      if (box1_tick_)
      {
        if (button1.BackColor == Color.LightCyan)
        {
          button1.BackColor = default(Color);
        }
        else
        {
          button1.BackColor = Color.LightCyan;
        }
      }
      if (box30_tick_)
      {
        if (button30.BackColor == Color.LightCyan)
        {
          button30.BackColor = default(Color);
        }
        else
        {
          button30.BackColor = Color.LightCyan;
        }
      }
      if (box9_tick_)
      {
        if (button9.BackColor == Color.LightCyan)
        {
          button9.BackColor = default(Color);
        }
        else
        {
          button9.BackColor = Color.LightCyan;
        }
      }
      if (box8_tick_)
      {
        if (button8.BackColor == Color.LightCyan)
        {
          button8.BackColor = default(Color);
        }
        else
        {
          button8.BackColor = Color.LightCyan;
        }
      }
      if (box7_tick_)
      {
        if (button7.BackColor == Color.LightCyan)
        {
          button7.BackColor = default(Color);
        }
        else
        {
          button7.BackColor = Color.LightCyan;
        }
      }
      if (box4_tick_)
      {
        if (button4.BackColor == Color.LightCyan)
        {
          button4.BackColor = default(Color);
        }
        else
        {
          button4.BackColor = Color.LightCyan;
        }
      }
      if (box20_tick_)
      {
        if (button20.BackColor == Color.LightCyan)
        {
          button20.BackColor = default(Color);
        }
        else
        {
          button20.BackColor = Color.LightCyan;
        }
      }
      if (box18_tick_)
      {
        if (button18.BackColor == Color.LightCyan)
        {
          button18.BackColor = default(Color);
        }
        else
        {
          button18.BackColor = Color.LightCyan;
        }
      }
      if (box21_tick_)
      {
        if (button21.BackColor == Color.LightCyan)
        {
          button21.BackColor = default(Color);
        }
        else
        {
          button21.BackColor = Color.LightCyan;
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

    private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
    {

    }

    private void toolTip1_Popup(object sender, PopupEventArgs e)
    {

    }



    private void textBox5_TextChanged(object sender, EventArgs e)
    {

    }



    private void openAllToolStripMenuItem_Click(object sender, EventArgs e)
    {
      //Open All
      OpenFileDialog openFileDialog1 = new OpenFileDialog();
      bool path_exist = Directory.Exists(outputPath_ + @"inputs\");
      if (path_exist)
      {
        try
        {
          openFileDialog1.InitialDirectory = outputPath_ + @"inputs\";
        }
        catch
        {
        }
      }
      else
      {
        openFileDialog1.InitialDirectory = outputPath_;
      }

      openFileDialog1.Filter = "hydro_all (*.HyAll)|*.HyAll";
      openFileDialog1.DefaultExt = "HyAll";
      openFileDialog1.Title = "Open file bundle in all extension with the same name";
      openFileDialog1.ShowDialog();

      OpenProject(openFileDialog1.FileName);

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
      //File >> Open >> Open... >> Liquid Properties
      OpenFileDialog openFileDialog1 = new OpenFileDialog();
      openFileDialog1.Filter = "hydro_liquid (*.HyLiq)|*.HyLiq";
      openFileDialog1.DefaultExt = "HyLiq";
      openFileDialog1.Title = "Open Single-phase Liquid Properties File";
      openFileDialog1.ShowDialog();
      if (openFileDialog1.FileName != "")
      {
        OpenLiquidPropertiesTab(openFileDialog1.FileName);
      }
    }
    private void liquidPhaseToolStripMenuItem_Click(object sender, EventArgs e)
    {
      //File >> Save As... >> Liquid Properties
      SaveFileDialog saveFileDialog1 = new SaveFileDialog();
      saveFileDialog1.Filter = "hydro_liquid (*.HyLiq)|*.HyLiq";
      saveFileDialog1.DefaultExt = "HyLiq";
      saveFileDialog1.AddExtension = true;
      saveFileDialog1.Title = "Save Single-phase Liquid Properties File";
      saveFileDialog1.ShowDialog();
      if (saveFileDialog1.FileName != "")
      {
        SaveLiquidPropertiesTab(saveFileDialog1.FileName);
      }
    }
    private void pipeGeometryToolStripMenuItem1_Click(object sender, EventArgs e)
    {
      //File >> Open >> Open... >> Pipe Geometry
      OpenFileDialog openFileDialog1 = new OpenFileDialog();
      openFileDialog1.Filter = "hydro_pipe (*.HyPip)|*.HyPip";
      openFileDialog1.DefaultExt = "HyPip";
      openFileDialog1.Title = "Open Single-phase Pipe Geometry File";
      openFileDialog1.ShowDialog();
      if (openFileDialog1.FileName != "")
      {
        OpenPipePropertiesTab(openFileDialog1.FileName);
      }
    }
    private void pipeGeometryToolStripMenuItem_Click(object sender, EventArgs e)
    {
      //File >> Save As... >> Pipe Geometry
      SaveFileDialog saveFileDialog1 = new SaveFileDialog();
      saveFileDialog1.Filter = "hydro_pipe (*.HyPip)|*.HyPip";
      saveFileDialog1.DefaultExt = "HyPip";
      saveFileDialog1.AddExtension = true;
      saveFileDialog1.Title = "Save Single-phase Pipe Geometry File";
      saveFileDialog1.ShowDialog();
      if (saveFileDialog1.FileName != "")
      {
        SavePipePropertiesTab(saveFileDialog1.FileName);
      }

    }
    private void inletConditionToolStripMenuItem1_Click(object sender, EventArgs e)
    {
      //File >> Open >> Open... >> Inlet Condition
      OpenFileDialog openFileDialog1 = new OpenFileDialog();
      openFileDialog1.Filter = "hydro_inlet (*.HyInL)|*.HyInL";
      openFileDialog1.DefaultExt = "HyInL";
      openFileDialog1.Title = "Open Single-phase Inlet Condition File";
      openFileDialog1.ShowDialog();
      if (openFileDialog1.FileName != "")
      {
        OpenInletCondition(openFileDialog1.FileName);
      }
    }
    private void inletConditionToolStripMenuItem_Click(object sender, EventArgs e)
    {
      //File >> Save As... >> Inlet Condition
      SaveFileDialog saveFileDialog1 = new SaveFileDialog();
      saveFileDialog1.Filter = "hydro_inlet (*.HyInL)|*.HyInL";
      saveFileDialog1.DefaultExt = "HyInL";
      saveFileDialog1.AddExtension = true;
      saveFileDialog1.Title = "Save Single-phase Inlet Condition File";
      saveFileDialog1.ShowDialog();
      if (saveFileDialog1.FileName != "")
      {
        SaveInletCondition(saveFileDialog1.FileName);
      }
    }
    private void flowLoopCaseToolStripMenuItem_Click(object sender, EventArgs e)
    {
      //File >> Open >> Open... >> Surrounding >> Flow Loop Case
      OpenFileDialog openFileDialog1 = new OpenFileDialog();
      openFileDialog1.Filter = "sur_flow_loop (*.SurFl)|*.SurFl";
      openFileDialog1.DefaultExt = "SurFl";
      openFileDialog1.Title = "Open Surrounding Flow loop Case File";
      openFileDialog1.ShowDialog();
      if (openFileDialog1.FileName != "")
      {
        OpenSurroundingTabFlowLoopCase(openFileDialog1.FileName);
      }
    }
    private void flowLoopCaseToolStripMenuItem1_Click(object sender, EventArgs e)
    {
      //File >> Save As... >> Surrounding >> Flow Loop Case
      SaveFileDialog saveFileDialog1 = new SaveFileDialog();
      saveFileDialog1.Filter = "sur_flow_loop (*.SurFl)|*.SurFl";
      saveFileDialog1.DefaultExt = "SurFl";
      saveFileDialog1.AddExtension = true;
      saveFileDialog1.Title = "Save Surrounding Flow loop Case File";
      saveFileDialog1.ShowDialog();
      if (saveFileDialog1.FileName != "")
      {
        SaveSurroundingTabFlowLoopCase(saveFileDialog1.FileName);
      }
    }
    private void fieldCaseToolStripMenuItem_Click(object sender, EventArgs e)
    {
      //File >> Open >> Open... >> Surrounding >> Field Case
      OpenFileDialog openFileDialog1 = new OpenFileDialog();
      openFileDialog1.Filter = "sur_field_case (*.SurFd)|*.SurFd";
      openFileDialog1.DefaultExt = "SurFd";
      openFileDialog1.Title = "Open Surrounding Field Case File";
      openFileDialog1.ShowDialog();
      if (openFileDialog1.FileName != "")
      {
        OpenSurroundingTabFieldCase(openFileDialog1.FileName);
      }
    }
    private void fieldCaseToolStripMenuItem1_Click(object sender, EventArgs e)
    {
      //File >> Save As... >> Surrounding >> Field Case
      SaveFileDialog saveFileDialog1 = new SaveFileDialog();
      saveFileDialog1.Filter = "sur_field_case (*.SurFd)|*.SurFd";
      saveFileDialog1.DefaultExt = "SurFd";
      saveFileDialog1.AddExtension = true;
      saveFileDialog1.Title = "Save Surrounding Field Case File";
      saveFileDialog1.ShowDialog();
      if (saveFileDialog1.FileName != "")
      {
        SaveSurroundingTabFieldCase(saveFileDialog1.FileName);
      }
    }
    private void simulationOptionsToolStripMenuItem1_Click(object sender, EventArgs e)
    {
      //File >> Open >> Open... >> Simulation Option
      OpenFileDialog openFileDialog1 = new OpenFileDialog();
      openFileDialog1.Filter = "simulation_option (*.SimOp)|*.SimOp";
      openFileDialog1.DefaultExt = "SimOp";
      openFileDialog1.Title = "Open Simulation Option File";
      openFileDialog1.ShowDialog();
      if (openFileDialog1.FileName != "")
      {
        OpenSimulationOption(openFileDialog1.FileName);
      }
    }

    private void simulationOptionsToolStripMenuItem_Click(object sender, EventArgs e)
    {
      //File >> Save As >> Simulation Options
      SaveFileDialog saveFileDialog1 = new SaveFileDialog();
      saveFileDialog1.Filter = "simulation_option (*.SimOp)|*.SimOp";
      saveFileDialog1.DefaultExt = "SimOp";
      saveFileDialog1.AddExtension = true;
      saveFileDialog1.Title = "Save Simulation Option File";
      saveFileDialog1.ShowDialog();
      if (saveFileDialog1.FileName != "")
      {
        SaveSimulationOption(saveFileDialog1.FileName);
      }
    }

    private void instructionToolStripMenuItem_Click(object sender, EventArgs e)
    {

    }

    private void createAnInputFileToolStripMenuItem_Click(object sender, EventArgs e)
    {
      //Operation >> Create All Input File
      SaveAll(@".\inputs\default_Dep1L_.HyAll");
      SaveLiquidPropertiesTab(@".\inputs\default_Dep1L_.HyLiq");
      SavePipePropertiesTab(@".\inputs\default_Dep1L_.HyPip");
      SaveInletCondition(@".\inputs\default_Dep1L_.HyInL");
      SaveSurroundingTabFlowLoopCase(@".\inputs\default_Dep1L_.SurFl");
      SaveSurroundingTabFieldCase(@".\inputs\default_Dep1L_.SurFd");
      SaveSimulationOption(@".\inputs\default_Dep1L_.SimOp");
      SaveModelTuningTab(@".\inputs\default_Dep1L_.MTune");
    }

    private void performTheCalculationToolStripMenuItem_Click(object sender, EventArgs e)
    {
      String path_ = exePath_;
      ProcessStartInfo start = new ProcessStartInfo();
      start.FileName = exePath_ + "SPDepo.exe";
      start.WindowStyle = ProcessWindowStyle.Normal;

      start.Arguments = outputPath_ +
                        " deposition " +
                        "single_phase ";

      int exit_code;
      using (Process proc = Process.Start(start))
      {
        proc.WaitForExit();
        exit_code = proc.ExitCode;
      }
    }
    private void parameterSearchToolStripMenuItem_Click(object sender, EventArgs e)
    {
      String path_ = exePath_;
      ProcessStartInfo start = new ProcessStartInfo();
      start.FileName = exePath_ + "SPDepo.exe";
      start.WindowStyle = ProcessWindowStyle.Normal;

      start.Arguments = outputPath_ +
                        " call_LM " +
                        "single_phase ";

      int exit_code;
      using (Process proc = Process.Start(start))
      {
        proc.WaitForExit();
        exit_code = proc.ExitCode;
      }
    }

    private void textBox17_TextChanged(object sender, EventArgs e)
    {

    }

    private void textBox16_TextChanged(object sender, EventArgs e)
    {

    }

    private void label18_Click(object sender, EventArgs e)
    {

    }

    private void groupBox10_Enter(object sender, EventArgs e)
    {

    }



    private void radioButton5_CheckedChanged(object sender, EventArgs e)
    {
      RadioButton rbutton = sender as RadioButton;
      if (!rbutton.Checked)
      {
        groupBox2.Enabled = false;
      }
      else
      {
        groupBox2.Enabled = true;
      }

    }

    private void radioButton6_CheckedChanged(object sender, EventArgs e)
    {
      RadioButton rbutton = sender as RadioButton;
      if (!rbutton.Checked)
      {
        groupBox6.Enabled = false;
      }
      else
      {
        groupBox6.Enabled = true;
      }
    }



    private void textBox8_TextChanged(object sender, EventArgs e)
    {
      TextBox tb = sender as TextBox;
      textBox14.Text = tb.Text;
    }



    private void textBox31_TextChanged(object sender, EventArgs e)
    {

    }

    private void textBox32_TextChanged(object sender, EventArgs e)
    {

    }

    private void comboBox15_SelectedIndexChanged(object sender, EventArgs e)
    {

    }

    private void textBox60_TextChanged(object sender, EventArgs e)
    {

    }



    private void button28_Click(object sender, EventArgs e)
    {
      SaveFileDialog saveFileDialog1 = new SaveFileDialog();
      saveFileDialog1.Filter = "comment (*.Commt)|*.Commt";
      saveFileDialog1.DefaultExt = "Commt";
      saveFileDialog1.AddExtension = true;
      saveFileDialog1.Title = "Save Comments";
      saveFileDialog1.ShowDialog();
      if (saveFileDialog1.FileName != "")
      {
        using (StreamWriter sw = new StreamWriter(saveFileDialog1.FileName))
        {
          sw.WriteLine(textBox63.Text);
        }
      }
    }

    private void button29_Click(object sender, EventArgs e)
    {
      OpenFileDialog openFileDialog1 = new OpenFileDialog();
      openFileDialog1.Filter = "comment (*.Commt)|*.Commt";
      openFileDialog1.DefaultExt = "Commt";
      openFileDialog1.Title = "Open Comments";
      openFileDialog1.ShowDialog();
      if (openFileDialog1.FileName != "")
      {
        using (StreamReader sr = new StreamReader(openFileDialog1.FileName))
        {
          StringBuilder sb = new StringBuilder { };
          while (!sr.EndOfStream)
          {
            sb.AppendLine(sr.ReadLine());
          }
          textBox63.Text = sb.ToString();
        }
      }
    }

    private void label47_Click(object sender, EventArgs e)
    {

    }

    private void textBox41_TextChanged(object sender, EventArgs e)
    {

    }

    private void radioButton7_CheckedChanged(object sender, EventArgs e)
    {
      if (radioButton7.Checked)
      {
        textBox37.Enabled = true;
        textBox41.Enabled = false;
        textBox57.Enabled = false;
      }
    }

    private void radioButton8_CheckedChanged(object sender, EventArgs e)
    {
      if (radioButton8.Checked)
      {
        textBox37.Enabled = false;
        textBox41.Enabled = true;
        textBox57.Enabled = false;
      }
    }

    private void radioButton9_CheckedChanged(object sender, EventArgs e)
    {
      if (radioButton9.Checked)
      {
        textBox37.Enabled = false;
        textBox41.Enabled = false;
        textBox57.Enabled = true;
      }
    }

    private void textBox65_TextChanged(object sender, EventArgs e)
    {

    }

    private void label80_Click(object sender, EventArgs e)
    {

    }



    private void radioButton10_CheckedChanged(object sender, EventArgs e)
    {
      //radio button "None" in "Impact of shear on wax deposition" in
      //"Simulation Options" tab
      if (radioButton10.Checked)
      {
        textBox36.Enabled = false;
        textBox44.Enabled = false;
      }
    }

    private void radioButton11_CheckedChanged(object sender, EventArgs e)
    {
      //radio button "Maximum Tolerable Shear Stress (MTSS) Model"
      //in "Impact of shear on wax deposition" in
      //"Simulation Options" tab
      if (radioButton11.Checked)
      {
        textBox36.Enabled = true;
        textBox44.Enabled = true;
      }
    }
    private void button25_Click(object sender, EventArgs e)
    {
      //"Create an Input File" button in Simulation Options tab
      SaveSimulationOption(@".\inputs\default_Dep1L_.SimOp");
    }
    private void button26_Click(object sender, EventArgs e)
    {
      //"Create an Input File" in "Calculation Options" tab

    }
    private void button22_Click(object sender, EventArgs e)
    {
      //Button: "Create an Input File"
      //in Liquid Properties tab of Deposition:Single-Phase form
      SaveLiquidPropertiesTab(@".\inputs\default_Dep1L_.HyLiq");
    }
    private void button3_Click(object sender, EventArgs e)
    {
      //Button: "Create an Input File"
      //in Pipe Geometry tab of Deposition:Single-Phase form
      SavePipePropertiesTab(@".\inputs\default_Dep1L_.HyPip");
    }
    private void button23_Click(object sender, EventArgs e)
    {
      //Button: "Create an Input File"
      //in Inlet Condition tab of  Single-Phase deposition
      SaveInletCondition(@".\inputs\default_Dep1L_.HyInL");
    }
    private void button24_Click(object sender, EventArgs e)
    {
      //Create an Input File Button in
      //Surrounding tab of Single-Phase deposition
      SaveSurroundingTabFlowLoopCase(@".\inputs\default_Dep1L_.SurFl");
      SaveSurroundingTabFieldCase(@".\inputs\default_Dep1L_.SurFd");
    }

    private void button14_Click(object sender, EventArgs e)
    {
      //"Open Thermophysical Properties" in Surrounding tab
      OpenFileDialog openFileDialog1 = new OpenFileDialog();
      openFileDialog1.Filter = "hydro_liquid (*.HyLiq)|*.HyLiq";
      openFileDialog1.DefaultExt = "HyLiq";
      openFileDialog1.Title = "Open Thermophysical Properties File";
      openFileDialog1.ShowDialog();
      if (openFileDialog1.FileName != "")
      {
        using (StreamReader sr = new StreamReader(openFileDialog1.FileName))
        {
          foreach (var tb in thermo_prop_surrounding_tab_)
          {
            tb.Text = sr.ReadLine();
          }
        }
      }
    }

    private void button15_Click(object sender, EventArgs e)
    {
      //"Save Thermophysical Properties" in Surrounding tab
      SaveFileDialog saveFileDialog1 = new SaveFileDialog();
      saveFileDialog1.Filter = "hydro_liquid (*.HyLiq)|*.HyLiq";
      saveFileDialog1.DefaultExt = "HyLiq";
      saveFileDialog1.AddExtension = true;
      saveFileDialog1.Title = "Save Thermophysical Properties File";
      saveFileDialog1.ShowDialog();
      if (saveFileDialog1.FileName != "")
      {
        using (StreamWriter sw = new StreamWriter(saveFileDialog1.FileName))
        {
          foreach (var tb in thermo_prop_surrounding_tab_)
          {
            sw.WriteLine(tb.Text);
          }
        }
      }
    }

    private void button17_Click(object sender, EventArgs e)
    {
      //"Open Surrounding Fluid Properties" in Surrounding tab
      OpenFileDialog openFileDialog1 = new OpenFileDialog();
      openFileDialog1.Filter = "hydro_liquid (*.HyLiq)|*.HyLiq";
      openFileDialog1.DefaultExt = "HyLiq";
      openFileDialog1.Title = "Open Surrounding Fluid Properties File";
      openFileDialog1.ShowDialog();
      if (openFileDialog1.FileName != "")
      {
        using (StreamReader sr = new StreamReader(openFileDialog1.FileName))
        {
          foreach (var tb in surrounding_fluid_surrounding_tab_)
          {
            tb.Text = sr.ReadLine();
          }
        }
      }
    }

    private void button16_Click(object sender, EventArgs e)
    {
      //"Save Thermophysical Properties" in Surrounding tab
      SaveFileDialog saveFileDialog1 = new SaveFileDialog();
      saveFileDialog1.Filter = "hydro_liquid (*.HyLiq)|*.HyLiq";
      saveFileDialog1.DefaultExt = "HyLiq";
      saveFileDialog1.AddExtension = true;
      saveFileDialog1.Title = "Save Surrounding Fluid Properties File";
      saveFileDialog1.ShowDialog();
      if (saveFileDialog1.FileName != "")
      {
        using (StreamWriter sw = new StreamWriter(saveFileDialog1.FileName))
        {
          foreach (var tb in surrounding_fluid_surrounding_tab_)
          {
            sw.WriteLine(tb.Text);
          }
        }
      }
    }



    private void textBox2_TextChanged(object sender, EventArgs e)
    {
      TextBox tb = sender as TextBox;
      if (tb.Text[0] == '*')
      {
        comboBox1.SelectedIndex = 1;
        box5_tick_ = false;
        button5.BackColor = default(Color);
      }
      else
      {
        comboBox1.SelectedIndex = 0;
      }
    }

    private void textBox3_TextChanged(object sender, EventArgs e)
    {
      TextBox tb = sender as TextBox;
      if (tb.Text[0] == '*')
      {
        comboBox2.SelectedIndex = 1;
        box6_tick_ = false;
        button6.BackColor = default(Color);
      }
      else
      {
        comboBox2.SelectedIndex = 0;
      }
    }

    private void textBox13_TextChanged(object sender, EventArgs e)
    {
      TextBox tb = sender as TextBox;
      if (tb.Text[0] == '*')
      {
        comboBox4.SelectedIndex = 1;
        box2_tick_ = false;
        button2.BackColor = default(Color);
      }
      else
      {
        comboBox4.SelectedIndex = 0;
      }
    }

    private void textBox12_TextChanged(object sender, EventArgs e)
    {
      TextBox tb = sender as TextBox;
      if (tb.Text[0] == '*')
      {
        comboBox3.SelectedIndex = 1;
        box1_tick_ = false;
        button1.BackColor = default(Color);
      }
      else
      {
        comboBox3.SelectedIndex = 0;
      }
    }

    private void textBox22_TextChanged(object sender, EventArgs e)
    {
      //Density >> Flow Loop Case >> Surrounding
      TextBox tb = sender as TextBox;
      if (tb.Text[0] == '*')
      {
        comboBox8.SelectedIndex = 1;
        box9_tick_ = false;
        button9.BackColor = default(Color);
      }
      else
      {
        comboBox8.SelectedIndex = 0;
      }
    }

    private void textBox21_TextChanged(object sender, EventArgs e)
    {
      //Viscosity >> Flow Loop Case >> Surrounding
      TextBox tb = sender as TextBox;
      if (tb.Text[0] == '*')
      {
        comboBox7.SelectedIndex = 1;
        box8_tick_ = false;
        button8.BackColor = default(Color);
      }
      else
      {
        comboBox7.SelectedIndex = 0;
      }
    }

    private void textBox20_TextChanged(object sender, EventArgs e)
    {
      //Thermal Cond >> Flow Loop Case >> Surrounding
      TextBox tb = sender as TextBox;
      if (tb.Text[0] == '*')
      {
        comboBox6.SelectedIndex = 1;
        box7_tick_ = false;
        button7.BackColor = default(Color);
      }
      else
      {
        comboBox6.SelectedIndex = 0;
      }
    }

    private void textBox19_TextChanged(object sender, EventArgs e)
    {
      //Thermal Cond >> Flow Loop Case >> Surrounding
      TextBox tb = sender as TextBox;
      if (tb.Text[0] == '*')
      {
        comboBox5.SelectedIndex = 1;
        box4_tick_ = false;
        button4.BackColor = default(Color);
      }
      else
      {
        comboBox5.SelectedIndex = 0;
      }
    }

    private void button13_Click(object sender, EventArgs e)
    {
      //Density >> Field Case >> Surrounding
    }

    private void comboBox20_SelectedIndexChanged(object sender, EventArgs e)
    {
      //Predict Deposit Composition >> Simulation Options
      //  true
      //  false
      //Make no changes in the GUI
    }

    private void comboBox21_SelectedIndexChanged(object sender, EventArgs e)
    {
      //Crystal aspect ratio formula >> Simulation Options
      //alpha = 1 + k_alpha * fw(Singh et al. 2000)
      //alpha = 1 + sqrt((fw - fw0) / A)(Lee 2008)
      //User define function
      ComboBox cb = sender as ComboBox;
      int index = cb.SelectedIndex;
      if ((index == 0) || (index == 1))
      {
        button21.Enabled = false;
        box21_tick_ = false;
        button21.BackColor = default(Color);
      }
      else
      {
        button21.Enabled = true;
        box21_tick_ = true;
      }
    }

    private void comboBox22_SelectedIndexChanged(object sender, EventArgs e)
    {
      //Precipitation Curve Model >> Simulation Options
      //  user defined function
      //  compositional model
      //  lumped component model
      ComboBox cb = sender as ComboBox;
      int index = cb.SelectedIndex;
      if (index == 0)
      {
        groupBox9.Enabled = true;
        textBox38.Enabled = false;
        textBox33.Enabled = false;
        box20_tick_ = true;
        box18_tick_ = true;
      }
      else
      {
        groupBox9.Enabled = false;
        box20_tick_ = false;
        box18_tick_ = false;
        button20.BackColor = default(Color);
        button18.BackColor = default(Color);
      }
    }

    private void button18_Click(object sender, EventArgs e)
    {
      //Edit Equation >> dCdT(T) Eq >> Simulation Option
      box18_tick_ = false;
      button18.BackColor = default(Color);
      BigEquationForm bigForm = new BigEquationForm(textBox33);
      bigForm.Show();
    }

    private void button21_Click(object sender, EventArgs e)
    {
      //Edit Equation >> Alpha Eq >> Simulation Option
      box21_tick_ = false;
      button21.BackColor = default(Color);
      BigEquationForm bigForm = new BigEquationForm(textBox40);
      bigForm.Show();
    }

    private void textBox38_TextChanged(object sender, EventArgs e)
    {
      //Edit Equation >> fs(T) Eq >> Simulation Option
      TextBox tb = sender as TextBox;
      if (tb.Text[0] == '*')
      {
        box20_tick_ = false;
        button20.BackColor = default(Color);
      }
    }

    private void textBox33_TextChanged(object sender, EventArgs e)
    {
      //Edit Equation >> fs(T) Eq >> Simulation Option
      TextBox tb = sender as TextBox;
      if (tb.Text[0] == '*')
      {
        box18_tick_ = false;
        button18.BackColor = default(Color);
      }
    }

    private void textBox40_TextChanged(object sender, EventArgs e)
    {
      //Edit Equation >> Alpha Eq >> Simulation Option
      TextBox tb = sender as TextBox;
      if (tb.Text[0] == '*')
      {
        box21_tick_ = false;
        button21.BackColor = default(Color);
      }
    }

    private void textBox28_TextChanged(object sender, EventArgs e)
    {

    }

    private void comboBox16_SelectedIndexChanged(object sender, EventArgs e)
    {
      ComboBox cbbox = sender as ComboBox;
      if (cbbox.SelectedIndex == 1)
      {
        button30.Enabled = true;
        textBox55.Enabled = false;
        box5_tick_ = true;
      }
      else
      {
        button30.Enabled = false;
        textBox55.Enabled = true;
        box5_tick_ = false;
        button30.BackColor = default(Color);
      }
    }

    private void comboBox2_SelectedIndexChanged(object sender, EventArgs e)
    {

    }

    private void textBox55_TextChanged(object sender, EventArgs e)
    {
      TextBox tb = sender as TextBox;
      if (tb.Text[0] == '*')
      {
        comboBox16.SelectedIndex = 1;
        box30_tick_ = false;
        button30.BackColor = default(Color);
      }
      else
      {
        comboBox16.SelectedIndex = 0;
      }
    }

    private void checkBox7_CheckedChanged(object sender, EventArgs e)
    {
      //Use Default Setup (recommended) >> Calculation Options
    }

    private void toolStripMenuItem4_Click(object sender, EventArgs e)
    {

    }

    private void quickDefaultOutputToolStripMenuItem_Click(object sender, EventArgs e)
    {
      try
      {
        Deposition1POutput default_temporal_output =
          new Deposition1POutput(outputPath_, "temporal data", "delta_mm_average.jpg",
            "fw_average.jpg");
        default_temporal_output.Show();
        Deposition1POutput default_profile_output =
          new Deposition1POutput(outputPath_, "profile data", "delta_mm_profile_r.jpg",
            "fw_profile_r.jpg");
        default_profile_output.Show();
        Deposition1POutput default_multiple_trends_output =
          new Deposition1POutput(outputPath_, "multiple trend lines output", "delta_mm_profile_z.jpg",
            "fw_profile_z.jpg");
        default_multiple_trends_output.Show();
        if (radioButton2.Checked || radioButton3.Checked || radioButton4.Checked)
        {
          System.IO.DirectoryInfo directoryInfo = new DirectoryInfo(@".\profile_data_pic");
          int i_total = directoryInfo.GetFiles().Length;
          int number_of_pattern = i_total / 6;
          string name_first_part = "c_tzr_mesh_post__";
          string name_last_part = ".jpg";
          TrackBarPictures trackBarPictures1 =
            new TrackBarPictures("c-heat-map", outputPath_ + @"profile_data_pic\" + name_first_part, name_last_part, number_of_pattern);
          trackBarPictures1.Show();
          name_last_part = "_r.jpg";
          TrackBarPictures trackBarPictures2 =
            new TrackBarPictures("c-radial-data",
            outputPath_ + @"profile_data_pic\" + name_first_part,
            name_last_part, number_of_pattern);
          trackBarPictures2.Show();
          name_last_part = "_z.jpg";
          TrackBarPictures trackBarPictures3 =
            new TrackBarPictures("c-axial-data",
            outputPath_ + @"profile_data_pic\" + name_first_part,
            name_last_part, number_of_pattern);
          trackBarPictures3.Show();

          name_first_part = "t_tzr_mesh_post__";
          name_last_part = ".jpg";
          TrackBarPictures trackBarPictures1t =
            new TrackBarPictures("T-heat-map", outputPath_ + @"profile_data_pic\" + name_first_part, name_last_part, number_of_pattern);
          trackBarPictures1t.Show();
          name_last_part = "_r.jpg";
          TrackBarPictures trackBarPictures2t =
            new TrackBarPictures("t-radial-data",
            outputPath_ + @"profile_data_pic\" + name_first_part,
            name_last_part, number_of_pattern);
          trackBarPictures2t.Show();
          name_last_part = "_z.jpg";
          TrackBarPictures trackBarPictures3t =
            new TrackBarPictures("t-axial-data",
            outputPath_ + @"profile_data_pic\" + name_first_part,
            name_last_part, number_of_pattern);
          trackBarPictures3t.Show();
        }
      }
      catch
      {
        Console.WriteLine("Cannot read the output graph\n" +
          "Check the calculation & output paths");
      }
      try
      {
        Deposition1POutput default_temporal_output =
          new Deposition1POutput(outputPath_, "temporal data", "sim_exp_delta_ave.jpg",
            "sim_exp_fw_ave.jpg");
      }
      catch
      { //the data are not available to compare, just show nothing.
      }
    }

    private void changeToNewOutputFolderToolStripMenuItem_Click(object sender, EventArgs e)
    {
      //Operation >> Change to New Output Folder
      FolderBrowserDialog folderBrowserDialog1 = new FolderBrowserDialog();
      folderBrowserDialog1.Description = "Specify the new target folder";
      folderBrowserDialog1.SelectedPath = AppDomain.CurrentDomain.BaseDirectory;
      if (folderBrowserDialog1.ShowDialog() == DialogResult.OK)
      {
        outputPath_ = folderBrowserDialog1.SelectedPath + "\\";
      }
      Directory.SetCurrentDirectory(outputPath_);
      Directory.CreateDirectory("inputs");
      this.Text = "Deposition Single-Phase:" + folderBrowserDialog1.SelectedPath;
    }

    private void button27_Click(object sender, EventArgs e)
    {
      //Model Tuning >> Create an input file
      SaveModelTuningTab(@".\inputs\default_Dep1L_.MTune");
    }

    private void modelTuningToolStripMenuItem1_Click(object sender, EventArgs e)
    {
      //File >> Open >> Open... >> Model Tuning
      OpenFileDialog openFileDialog1 = new OpenFileDialog();
      openFileDialog1.Filter = "model_tuning (*.MTune)|*.MTune";
      openFileDialog1.DefaultExt = "MTune";
      openFileDialog1.Title = "Open Model Tuning File";
      openFileDialog1.ShowDialog();
      if (openFileDialog1.FileName != "")
      {
        OpenModelTuningTab(openFileDialog1.FileName);
      }
    }

    private void dataGridView1_KeyDown(object sender, KeyEventArgs e)
    {
      //https://bit.ly/2UgLTeB
      if (e.Control && e.KeyCode == Keys.C)
      {
        DataObject d = dataGridView1.GetClipboardContent();
        Clipboard.SetDataObject(d);
        e.Handled = true;
      }
      else if (e.Control && e.KeyCode == Keys.V)
      {
        string s = Clipboard.GetText();
        string[] lines = s.Split('\n');
        int row = dataGridView1.CurrentCell.RowIndex;
        int col = dataGridView1.CurrentCell.ColumnIndex;
        foreach (string line in lines)
        {
          if (row < dataGridView1.RowCount && line.Length > 0)
          {
            string[] cells = line.Split('\t');
            for (int i = 0; i < cells.GetLength(0); i++)
            {
              if (col + i < this.dataGridView1.ColumnCount)
              {
                dataGridView1[col + i, row].Value =
                  Convert.ChangeType(cells[i], dataGridView1[col + i, row].ValueType);
              }
              else
              {
                break;
              }
            }
            row++;
          }
          else
          {
            break;
          }
        }
      }
    }

    private void outputToolStripMenuItem_Click(object sender, EventArgs e)
    {//Operation >> Show Results >> Quick Output Comparison
      try
      {
        Deposition1POutput prediction_comparison =
          new Deposition1POutput(outputPath_, outputPath_, "sim_exp_delta_ave.jpg",
            "sim_exp_fw_ave.jpg");
        prediction_comparison.Show();
      }
      catch
      {
        Console.WriteLine("Cannot read the output graph\n" +
          "Check sim_exp_delta_ave.jpg and sim_exp_fw_ave.jpg");
      }
    }

    private void quickLMResultToolStripMenuItem_Click(object sender, EventArgs e)
    {//Operation >> Show Results >> Quick Search Result
      List<string> ans_list = new List<string>();
      List<List<string>> csv_string = new List<List<string>>();
      string file_name = outputPath_ + "LM_result.csv";
      StringBuilder sb = new StringBuilder { };
      using (StreamReader sr = new StreamReader(file_name)){
        while(!sr.EndOfStream){
          sb.AppendLine(sr.ReadLine().Replace(",","\t\t"));
        }
      }
      try
      {
        LMOutput lm_output =
          new LMOutput(outputPath_, outputPath_, "sim_exp_delta_ave.jpg",
            "sim_exp_fw_ave.jpg", sb);
        lm_output.Show();
      }
      catch
      {
        Console.WriteLine("Cannot read the output graph\n" +
          "Check sim_exp_delta_ave.jpg and sim_exp_fw_ave.jpg");
      }
    }
  }
}
