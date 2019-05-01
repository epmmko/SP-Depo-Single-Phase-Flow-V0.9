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
using System.Text.RegularExpressions;
using System.Diagnostics;

namespace SPDepoGUI
{
  using BigEquation;
  public partial class BigEquationForm : Form
  {
    ComboBox combo = null;
    TextBox textBoxAns_;
    List<ComboBox> cbb_local_;
    public BigEquationForm(TextBox textBoxAns)
    {
      InitializeComponent();
      textBoxAns_ = textBoxAns;
      dataGridView1.Rows[0].Height = 26;
      cbb_local_ = new List<ComboBox>{comboBox1, comboBox2,
        comboBox3, comboBox4, comboBox5, comboBox6};
      if(textBoxAns.Text != ""){
        textBox1.Text = textBoxAns.Text;
        OpenFrom("EditEquation");
      }
    }

    private void BigEquationForm_Load(object sender, EventArgs e)
    {
      Color color = ColorTranslator.FromHtml("#d1d1d1");
      dataGridView1.Columns["Column8"].DefaultCellStyle.BackColor = color;
      textBox2.Text = "0";
      textBox3.Text = "100";
      textBox4.Text = "50";
    }

    private void dataGridView1_CurrentCellDirtyStateChanged(object sender, EventArgs e)
    {
      if (dataGridView1.IsCurrentCellDirty)
      {
        dataGridView1.CommitEdit(DataGridViewDataErrorContexts.Commit);
      }
    }

    private void dataGridView1_CellValueChanged(object sender, DataGridViewCellEventArgs e)
    {
      if (combo != null)
      {
        int index_local = combo.SelectedIndex;
        int current_column = dataGridView1.CurrentCell.ColumnIndex;
        int current_row = dataGridView1.CurrentCell.RowIndex;
        if (current_column == 6) {
          if (index_local == 0){
            dataGridView1.Rows[current_row].Cells[7].Value = null;
          } else{
            dataGridView1.Rows[current_row].Cells[7].Value 
              = String.Format("{0}", index_local - 1);
          }
        }
      }
      dataGridView1.Invalidate();
    }

    private void dataGridView1_EditingControlShowing(object sender,
      DataGridViewEditingControlShowingEventArgs e)
    {
      combo = e.Control as ComboBox;

    }

    private void dataGridView1_CellEndEdit(object sender, DataGridViewCellEventArgs e)
    {
    }

    private void dataGridView1_DefaultValuesNeeded(object sender, DataGridViewRowEventArgs e)
    {
      var grid = sender as DataGridView;
      foreach (DataGridViewRow row in grid.Rows)
      {
        row.HeaderCell.Value = String.Format("c[{0}]", row.Index);
      }
    }

    private void dataGridView1_VisibleChanged(object sender, EventArgs e)
    {
      var grid = sender as DataGridView;
      foreach (DataGridViewRow row in grid.Rows)
      {
        row.HeaderCell.Value = String.Format("c[{0}]", row.Index);
      }
    }

    private void button1_Click(object sender, EventArgs e)
    {
      string a;
      int eq_number = 0;
      string eq_type = "";
      bool stop_count_equation = false;
      List<string> eqs = new List<string> { "", "", "", "", "", "", "", ""};

      for (int i = 0; i < 8; i++)
      {
        if (dataGridView1.Rows[0].Cells[i].Value == null ||
           dataGridView1.Rows[0].Cells[i].Value == DBNull.Value ||
           String.IsNullOrWhiteSpace(dataGridView1.Rows[0]
             .Cells[i].Value.ToString()))
        {
          i = 7;
          stop_count_equation = true;
        }
        if (!stop_count_equation){
          eq_number++;
        }
        foreach (DataGridViewRow row in dataGridView1.Rows)
        {
          if (row.Cells[i].Value == null ||
              row.Cells[i].Value == DBNull.Value ||
              String.IsNullOrWhiteSpace(row.Cells[i].Value.ToString()))
          {
            break;
          }
          eqs[i] = eqs[i] + 
            (row.Cells[i].Value).ToString().Replace(" ", string.Empty)
            + ',';
        }
        if (eqs[i].Length > 0){
          eqs[i] = eqs[i].TrimEnd(eqs[i][eqs[i].Length - 1]);
          eqs[i] = "[" + eqs[i] + "]";
        } else {
          eqs[i] = "[]";
        }

      }

      //Creating List of Equation Type
      eq_type = "[";
      for (int i = 0; i < eq_number; i++){
        eq_type = eq_type + String.Format("{0},",
                    cbb_local_[i].SelectedIndex - 1);
      }
      eq_type = eq_type.TrimEnd(',');
      eq_type = eq_type + "]";

      a = "*" + String.Format("{0}", eq_number) + "," + eq_type + ","
            + eqs[7] + ",";
      for (int i = 0; i < 6; i++){
        if(eqs[i] == ""){
          if(i==0){
            a = a + "[],";
          }
          break;
        }
        a = a + eqs[i] + ",";
      }
      a = a.TrimEnd(a[a.Length - 1]);
      textBox1.Text = a;
    }

    private void dataGridView1_RowsRemoved(object sender, DataGridViewRowsRemovedEventArgs e)
    {
      var grid = sender as DataGridView;
      foreach (DataGridViewRow row in grid.Rows)
      {
        row.HeaderCell.Value = String.Format("c[{0}]", row.Index);
      }
    }

    private void button2_Click(object sender, EventArgs e)
    { //Apply Button
      button1_Click(sender, e);
      textBoxAns_.Text = textBox1.Text;
    }

    private void button3_Click(object sender, EventArgs e)
    { //Save Equation As... Button
      button1_Click(sender, e);
      String path_ = AppDomain.CurrentDomain.BaseDirectory;
      SaveFileDialog saveFileDialog1 = new SaveFileDialog();
      saveFileDialog1.Filter = "BigEquation (*.BigEq)|*.BigEq";
      saveFileDialog1.DefaultExt = "BigEq";
      saveFileDialog1.AddExtension = true;
      saveFileDialog1.Title = "Save Big-Equation File";
      saveFileDialog1.ShowDialog();
      if (saveFileDialog1.FileName != "")
      {
        FileStream fs = (FileStream)saveFileDialog1.OpenFile();
        using (StreamWriter sw = new StreamWriter(fs))
        {
          sw.WriteLine(textBox1.Text);
        }
      }
    }

    private void button4_Click(object sender, EventArgs e)
    { //Open From... Button
      OpenFrom();
    }

    public void OpenFrom(string option = "OpenFrom"){
      //If option == "OpenFrom" it shows the open from dialog
      //If option == something else ("EditEquation")
      //  it reads from the input text
      //  and put the information in the table/comboboxes.
      String opened_data;
      String[] data;
      int number_of_equation;
      List<String> types_of_equation = new List<string>();
      List<List<String>> parse_ans;
      List<int> parse_ans_length = new List<int>();
      if(option == "OpenFrom"){
        OpenFileDialog openFileDialog1 = new OpenFileDialog();
        openFileDialog1.Filter = "BigEquation (*.BigEq)|*.BigEq";
        openFileDialog1.FilterIndex = 1;
        openFileDialog1.Title = "Open Big-Equation File";
        openFileDialog1.ShowDialog();
        if (openFileDialog1.FileName != "")
        {
          FileStream fs = (FileStream)openFileDialog1.OpenFile();
          using (StreamReader sr = new StreamReader(fs))
          {
            opened_data = sr.ReadLine();
            textBox1.Text = opened_data;
          }
          fs.Close();
        }
      }
      opened_data = textBox1.Text;
      opened_data = opened_data.TrimStart(opened_data[0]);
      parse_ans = opened_data.ExtractListOfList();
      data = opened_data.Split(',');
      number_of_equation = Int32.Parse(data[0]);
      foreach (ComboBox i in cbb_local_)
      {
        i.SelectedIndex = 0;
      }
      //Get and Set type-of-equation combobox
      for (int i = 0; i < number_of_equation; i++)
      {
        types_of_equation.Add(parse_ans[0][i]);
        cbb_local_[i].SelectedIndex = 1 + Int32.Parse(types_of_equation[i]);
      }

      foreach (List<String> list_string in parse_ans)
      {
        parse_ans_length.Add(list_string.Count);
      }
      dataGridView1.Rows.Clear();
      dataGridView1.Refresh();
      for (int i = 0; i < parse_ans_length.Max(); i++)
      {
        dataGridView1.Rows.Add();
      }
      //Set Coefficient
      for (int i = 0; i < number_of_equation; i++)
      {
        AddToDataGridView(i, parse_ans[i + 2]);
      }

      //Set Operator Text Column
      AddToDataGridViewComboBoxCell(6, parse_ans[1]);
      //Set Operator Value Column
      AddToDataGridView(7, parse_ans[1]);

      void AddToDataGridView(int column, List<String> data_list)
      {
        for (int j = 0; j < data_list.Count; j++)
        {
          dataGridView1.Rows[j].Cells[column].Value = data_list[j];
        }
      }
      void AddToDataGridViewComboBoxCell(int column, List<String> data_list)
      {
        for (int j = 0; j < data_list.Count; j++)
        {
          dataGridView1.Rows[j].Cells[column].Value =
            BigEquationExtension.operator_dictionary[data_list[j]];
        }
      }
    }

    private void equationDescriptionToolStripMenuItem_Click(object sender, EventArgs e)
    {
      EquationBasisInstruction eqnInfo = new EquationBasisInstruction();
      eqnInfo.Show();
    }

    private void bigEquationCreationGuidelinesToolStripMenuItem_Click(object sender, EventArgs e)
    {
      BigEquationInstruction bigEqnInfo = new BigEquationInstruction();
      bigEqnInfo.Show();
    }

    private void button5_Click(object sender, EventArgs e)
    { //Try Plotting Button
      button1_Click(sender, e);
      String path_ = AppDomain.CurrentDomain.BaseDirectory;

      FileStream fs = new FileStream(path_ + "default_plot.BigEq", FileMode.Create, FileAccess.Write, FileShare.None);
      using (StreamWriter sw = new StreamWriter(fs))
      {
        sw.WriteLine(textBox1.Text);
      }

      ProcessStartInfo start = new ProcessStartInfo();
      start.FileName = "SPDepo.exe";
      start.WindowStyle = ProcessWindowStyle.Normal;
      start.Arguments = path_ + "plot_big_equation " + textBox2.Text + " " + textBox3.Text + " " + textBox4.Text;
      int exit_code;
      using (Process proc = Process.Start(start))
      {
        proc.WaitForExit();
        exit_code = proc.ExitCode;
      }
    }

    private void button6_Click(object sender, EventArgs e)
    {
      string path_image = AppDomain.CurrentDomain.BaseDirectory;
      try
      {
        AdditionalPicture pic1 = new AdditionalPicture();
        pic1.title_name_ = "big_equation_plot.jpg";
        pic1.Show();
      }
      catch (Exception ex)
      {
        Console.WriteLine(ex.ToString());
      }
    }
  }
}
