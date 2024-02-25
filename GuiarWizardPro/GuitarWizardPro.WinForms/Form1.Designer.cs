namespace GuitarWizardPro.WinForms
{
    partial class Form1
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
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
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            pictureBox = new PictureBox();
            Framerate = new TextBox();
            audioFrameCounter = new TextBox();
            ((System.ComponentModel.ISupportInitialize)pictureBox).BeginInit();
            SuspendLayout();
            // 
            // pictureBox
            // 
            pictureBox.Location = new Point(0, 0);
            pictureBox.Margin = new Padding(2);
            pictureBox.Name = "pictureBox";
            pictureBox.Size = new Size(1024, 500);
            pictureBox.TabIndex = 0;
            pictureBox.TabStop = false;
            // 
            // Framerate
            // 
            Framerate.Location = new Point(98, 588);
            Framerate.Name = "Framerate";
            Framerate.Size = new Size(100, 23);
            Framerate.TabIndex = 1;
            // 
            // audioFrameCounter
            // 
            audioFrameCounter.Location = new Point(419, 578);
            audioFrameCounter.Name = "audioFrameCounter";
            audioFrameCounter.Size = new Size(100, 23);
            audioFrameCounter.TabIndex = 2;
            // 
            // Form1
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(1024, 740);
            Controls.Add(audioFrameCounter);
            Controls.Add(Framerate);
            Controls.Add(pictureBox);
            Margin = new Padding(2);
            Name = "Form1";
            Text = "Form1";
            ((System.ComponentModel.ISupportInitialize)pictureBox).EndInit();
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private PictureBox pictureBox;
        private TextBox Framerate;
        private TextBox audioFrameCounter;
    }
}
