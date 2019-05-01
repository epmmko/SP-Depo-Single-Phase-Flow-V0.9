namespace SPDepoGUI
{
  partial class EquationBasisInstruction
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(EquationBasisInstruction));
      this.panel1 = new System.Windows.Forms.Panel();
      this.label11 = new System.Windows.Forms.Label();
      this.label1 = new System.Windows.Forms.Label();
      this.label2 = new System.Windows.Forms.Label();
      this.panel1.SuspendLayout();
      this.SuspendLayout();
      // 
      // panel1
      // 
      this.panel1.AutoScroll = true;
      this.panel1.BackColor = System.Drawing.Color.White;
      this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.panel1.Controls.Add(this.label2);
      this.panel1.Controls.Add(this.label11);
      this.panel1.Controls.Add(this.label1);
      this.panel1.Font = new System.Drawing.Font("Monospac821 BT", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.panel1.Location = new System.Drawing.Point(7, 12);
      this.panel1.Name = "panel1";
      this.panel1.Size = new System.Drawing.Size(848, 512);
      this.panel1.TabIndex = 3;
      this.panel1.Paint += new System.Windows.Forms.PaintEventHandler(this.panel1_Paint);
      // 
      // label11
      // 
      this.label11.AutoSize = true;
      this.label11.Font = new System.Drawing.Font("Courier New", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.label11.Location = new System.Drawing.Point(10, 145);
      this.label11.Name = "label11";
      this.label11.Size = new System.Drawing.Size(780, 357);
      this.label11.TabIndex = 10;
      this.label11.Text = resources.GetString("label11.Text");
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.label1.ForeColor = System.Drawing.Color.Blue;
      this.label1.Location = new System.Drawing.Point(9, 7);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(287, 25);
      this.label1.TabIndex = 0;
      this.label1.Text = "Equation Basis Instruction";
      // 
      // label2
      // 
      this.label2.AutoSize = true;
      this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 14F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.label2.ForeColor = System.Drawing.Color.Blue;
      this.label2.Location = new System.Drawing.Point(10, 41);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(725, 120);
      this.label2.TabIndex = 11;
      this.label2.Text = resources.GetString("label2.Text");
      // 
      // EquationBasisInstruction
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(867, 527);
      this.Controls.Add(this.panel1);
      this.Name = "EquationBasisInstruction";
      this.ShowIcon = false;
      this.Text = "EquationBasisInstruction";
      this.panel1.ResumeLayout(false);
      this.panel1.PerformLayout();
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.Panel panel1;
    private System.Windows.Forms.Label label11;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.Label label2;
  }
}