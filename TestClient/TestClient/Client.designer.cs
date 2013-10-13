namespace TestClient
{
    partial class Client
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
            this.ConectBtn = new System.Windows.Forms.Button();
            this.SendBtn = new System.Windows.Forms.Button();
            this.DisconnectBtn = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.sendTxt = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.recvTxt = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // ConectBtn
            // 
            this.ConectBtn.Location = new System.Drawing.Point(23, 12);
            this.ConectBtn.Name = "ConectBtn";
            this.ConectBtn.Size = new System.Drawing.Size(75, 23);
            this.ConectBtn.TabIndex = 0;
            this.ConectBtn.Text = "Connect";
            this.ConectBtn.UseVisualStyleBackColor = true;
            this.ConectBtn.Click += new System.EventHandler(this.ConectBtn_Click);
            // 
            // SendBtn
            // 
            this.SendBtn.Location = new System.Drawing.Point(23, 51);
            this.SendBtn.Name = "SendBtn";
            this.SendBtn.Size = new System.Drawing.Size(75, 23);
            this.SendBtn.TabIndex = 1;
            this.SendBtn.Text = "Send Data";
            this.SendBtn.UseVisualStyleBackColor = true;
            this.SendBtn.Click += new System.EventHandler(this.SendBtn_Click);
            // 
            // DisconnectBtn
            // 
            this.DisconnectBtn.Location = new System.Drawing.Point(120, 12);
            this.DisconnectBtn.Name = "DisconnectBtn";
            this.DisconnectBtn.Size = new System.Drawing.Size(75, 23);
            this.DisconnectBtn.TabIndex = 2;
            this.DisconnectBtn.Text = "Disconnect";
            this.DisconnectBtn.UseVisualStyleBackColor = true;
            this.DisconnectBtn.Click += new System.EventHandler(this.DisconnectBtn_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(21, 82);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(55, 12);
            this.label1.TabIndex = 3;
            this.label1.Text = "Send Data:";
            // 
            // sendTxt
            // 
            this.sendTxt.Location = new System.Drawing.Point(23, 97);
            this.sendTxt.Multiline = true;
            this.sendTxt.Name = "sendTxt";
            this.sendTxt.Size = new System.Drawing.Size(221, 65);
            this.sendTxt.TabIndex = 4;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(21, 182);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(69, 12);
            this.label2.TabIndex = 5;
            this.label2.Text = "Receive Data:";
            // 
            // recvTxt
            // 
            this.recvTxt.Location = new System.Drawing.Point(23, 197);
            this.recvTxt.Multiline = true;
            this.recvTxt.Name = "recvTxt";
            this.recvTxt.ReadOnly = true;
            this.recvTxt.Size = new System.Drawing.Size(640, 65);
            this.recvTxt.TabIndex = 6;
            // 
            // Client
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(716, 273);
            this.Controls.Add(this.recvTxt);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.sendTxt);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.DisconnectBtn);
            this.Controls.Add(this.SendBtn);
            this.Controls.Add(this.ConectBtn);
            this.Name = "Client";
            this.Text = "Client";
            this.Load += new System.EventHandler(this.Client_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button ConectBtn;
        private System.Windows.Forms.Button SendBtn;
        private System.Windows.Forms.Button DisconnectBtn;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox sendTxt;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox recvTxt;
    }
}