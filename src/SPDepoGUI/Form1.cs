//https://stackoverflow.com/questions/240171/
//launching-an-application-exe-from-c
//https://goo.gl/81dmem
//https://goo.gl/wRrRnn
//https://stackoverflow.com/questions/27488313/
//excel workbooks.opentext example
using System;
using System.Windows.Forms;
using System.Diagnostics;
using Excel = Microsoft.Office.Interop.Excel;
using System.Collections.Generic;

namespace SPDepoGUI
{
  using BigEquation;
  public partial class Form1 : Form
  {
    public Form1()
    {
      InitializeComponent();
      Console.WriteLine("Welcome to Console Output of SP-Depo Version 1.0");
      Console.WriteLine("Created on 2/15/2019");
      Console.WriteLine("Contributor/Author: Arya Shahdi");
      Console.WriteLine("Contributor/Author: Ekarit Panacharoensawad");
      Console.WriteLine("License: The Unlicense");

      string a = "*3,[0,1,2],[13],[1.36],[20,3.5],[4.01,0.5,0.62]";
      List<List<String>> ans;
      ans = a.ExtractListOfList();
      BigEquationExtension.PrintNestedList(ans);
    }

    private void Quit_Program(object sender, EventArgs e)
    {
      Application.Exit();
    }

    private void create_input_Click(object sender, EventArgs e)
    {
      inputFileForm inputForm = new inputFileForm();
      inputForm.Show();
    }

    private void pointFlowPatternToolStripMenuItem_Click(object sender, EventArgs e)
    {
      ProcessStartInfo start = new ProcessStartInfo();
      start.FileName = "MinEnergy002.exe";
      start.WindowStyle = ProcessWindowStyle.Normal;
      int exit_code;
      using (Process proc = Process.Start(start))
      {
        proc.WaitForExit();
        exit_code = proc.ExitCode;
      }
      OnePointFlowPatternResult onePtResult = new OnePointFlowPatternResult();
      onePtResult.Show();
    }

    private void onePointFlowPatternToolStripMenuItem_Click(object sender, EventArgs e)
    {

    }
/*
    private void txt_to_excel(string file_name){
      string path_ = AppDomain.CurrentDomain.BaseDirectory;
      string path_temp = path_ + file_name + ".txt";
      object misValue = System.Reflection.Missing.Value;
      Excel.Application xlApp = new Excel.Application();
      Excel.Workbooks xlWorkBooks = xlApp.Workbooks;
      xlWorkBooks.OpenText(path_temp, misValue, 3,
          Excel.XlTextParsingType.xlDelimited,
          Excel.XlTextQualifier.xlTextQualifierNone,
          misValue, true, misValue, misValue, true, misValue, misValue,
          misValue, misValue, misValue, misValue, misValue, misValue);
      Excel.Workbook xlWorkBook = xlWorkBooks.Application.ActiveWorkbook;
      xlWorkBook.SaveAs(path_ + file_name + ".xls", 
          Excel.XlFileFormat.xlWorkbookNormal,
          misValue, misValue, misValue, misValue,
          Excel.XlSaveAsAccessMode.xlExclusive, misValue,
          misValue, misValue, misValue, misValue);
      xlWorkBook.Close(true, misValue, misValue);
      xlWorkBooks.Close();
      xlApp.Quit();
      System.Runtime.InteropServices.Marshal.ReleaseComObject(xlWorkBook);
      System.Runtime.InteropServices.Marshal.ReleaseComObject(xlWorkBooks);
      System.Runtime.InteropServices.Marshal.ReleaseComObject(xlApp);
    }
*/
    private void runToolStripMenuItem_Click(object sender, EventArgs e)
    {

    }

    private void inputFilesToolStripMenuItem_Click(object sender, EventArgs e)
    {

    }

    private void printToolStripMenuItem_Click(object sender, EventArgs e)
    {

    }

    private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
    {

    }

    private void newToolStripMenuItem_Click(object sender, EventArgs e)
    {

    }

    private void singlePhaseLiquidToolStripMenuItem_Click(
      object sender, EventArgs e)
    {
      HydrodynamicsSinglePhaseLiquid hydro_liquid = 
        new HydrodynamicsSinglePhaseLiquid();
      hydro_liquid.Show();
    }

    private void toolStripMenuItem1_Click(object sender, EventArgs e)
    { //New Project >> Wax Deposition Prediction >> Single-PHase Oil
      FolderBrowserDialog folderBrowserDialog1 = new FolderBrowserDialog();
      folderBrowserDialog1.Description = "Select/create the destination folder for the calculation results";
      folderBrowserDialog1.SelectedPath = AppDomain.CurrentDomain.BaseDirectory;
      string coreCalculationExePath = AppDomain.CurrentDomain.BaseDirectory;
      if (folderBrowserDialog1.ShowDialog() == DialogResult.OK) {
        Deposition1P dep1p = new Deposition1P(folderBrowserDialog1, coreCalculationExePath);
        dep1p.Show();
      } else {
        MessageBox.Show("A folder for deposition calculation must be selected before proceeds");
      }
    }

    private void openToolStripMenuItem_Click(object sender, EventArgs e)
    {
    }

    private void singlePhaseOilToolStripMenuItem_Click(object sender, EventArgs e)
    { //File >> Open Project >> Wax Deposition Prediction >> Single-Phase Oil
      OpenFileDialog openFileDialog1 = new OpenFileDialog();
      openFileDialog1.InitialDirectory = AppDomain.CurrentDomain.BaseDirectory;
      openFileDialog1.Filter = "hydro_all |default_Dep1L_.HyAll";
      openFileDialog1.DefaultExt = "HyAll";
      openFileDialog1.Title = "Open file bundle in all extension with the same name";
      openFileDialog1.ShowDialog();
      if (openFileDialog1.FileName != "")
      {
        int index = openFileDialog1.FileName.LastIndexOf('\\');
        string dir_name = openFileDialog1.FileName.Substring(0, index - 7);
        FolderBrowserDialog folderBrowserDialog1 = new FolderBrowserDialog();
        folderBrowserDialog1.SelectedPath = dir_name;
        Deposition1P dep1p = new Deposition1P(folderBrowserDialog1, AppDomain.CurrentDomain.BaseDirectory);
        dep1p.OpenProject(openFileDialog1.FileName);
        dep1p.Show();
      }
    }
  }
}
