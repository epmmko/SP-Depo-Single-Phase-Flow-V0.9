namespace SPDepoGUI
{
  partial class ShowResultsForm
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose();
      }
      base.Dispose(disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.checkBox1 = new System.Windows.Forms.CheckBox();
      this.button2 = new System.Windows.Forms.Button();
      this.label4 = new System.Windows.Forms.Label();
      this.textBox4 = new System.Windows.Forms.TextBox();
      this.SuspendLayout();
      // 
      // checkBox1
      // 
      this.checkBox1.AutoSize = true;
      this.checkBox1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.checkBox1.Location = new System.Drawing.Point(195, 18);
      this.checkBox1.Name = "checkBox1";
      this.checkBox1.Size = new System.Drawing.Size(127, 24);
      this.checkBox1.TabIndex = 11;
      this.checkBox1.Text = "Show All View";
      this.checkBox1.UseVisualStyleBackColor = true;
      // 
      // button2
      // 
      this.button2.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.button2.Location = new System.Drawing.Point(12, 12);
      this.button2.Name = "button2";
      this.button2.Size = new System.Drawing.Size(160, 34);
      this.button2.TabIndex = 10;
      this.button2.Text = "Show Results";
      this.button2.UseVisualStyleBackColor = true;
      this.button2.Click += new System.EventHandler(this.button2_Click);
      // 
      // label4
      // 
      this.label4.AutoSize = true;
      this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.label4.Location = new System.Drawing.Point(17, 69);
      this.label4.Name = "label4";
      this.label4.Size = new System.Drawing.Size(134, 20);
      this.label4.TabIndex = 9;
      this.label4.Text = "Result File Name:";
      // 
      // textBox4
      // 
      this.textBox4.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.textBox4.Location = new System.Drawing.Point(157, 60);
      this.textBox4.Name = "textBox4";
      this.textBox4.Size = new System.Drawing.Size(228, 26);
      this.textBox4.TabIndex = 8;
      // 
      // ShowResultsForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(409, 122);
      this.Controls.Add(this.checkBox1);
      this.Controls.Add(this.button2);
      this.Controls.Add(this.label4);
      this.Controls.Add(this.textBox4);
      this.Name = "ShowResultsForm";
      this.ShowIcon = false;
      this.Text = "ShowResultsForm";
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.CheckBox checkBox1;
    private System.Windows.Forms.Button button2;
    private System.Windows.Forms.Label label4;
    private System.Windows.Forms.TextBox textBox4;
  }
}