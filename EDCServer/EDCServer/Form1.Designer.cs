﻿namespace EDCServer
{
    partial class Form1
    {
        /// <summary>
        /// 設計工具所需的變數。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清除任何使用中的資源。
        /// </summary>
        /// <param name="disposing">如果應該公開 Managed 資源則為 true，否則為 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 設計工具產生的程式碼

        /// <summary>
        /// 此為設計工具支援所需的方法 - 請勿使用程式碼編輯器修改這個方法的內容。
        ///
        /// </summary>
        private void InitializeComponent()
        {
            this.strSrvBtn = new System.Windows.Forms.Button();
            this.logList = new System.Windows.Forms.ListBox();
            this.SuspendLayout();
            // 
            // strSrvBtn
            // 
            this.strSrvBtn.Location = new System.Drawing.Point(25, 27);
            this.strSrvBtn.Name = "strSrvBtn";
            this.strSrvBtn.Size = new System.Drawing.Size(75, 23);
            this.strSrvBtn.TabIndex = 0;
            this.strSrvBtn.Text = "Start Server";
            this.strSrvBtn.UseVisualStyleBackColor = true;
            this.strSrvBtn.Click += new System.EventHandler(this.strSrvBtn_Click);
            // 
            // logList
            // 
            this.logList.FormattingEnabled = true;
            this.logList.ItemHeight = 12;
            this.logList.Location = new System.Drawing.Point(25, 71);
            this.logList.Name = "logList";
            this.logList.Size = new System.Drawing.Size(241, 172);
            this.logList.TabIndex = 1;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(292, 273);
            this.Controls.Add(this.logList);
            this.Controls.Add(this.strSrvBtn);
            this.Name = "Form1";
            this.Text = "Form1";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.form_close);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button strSrvBtn;
        private System.Windows.Forms.ListBox logList;
    }
}

