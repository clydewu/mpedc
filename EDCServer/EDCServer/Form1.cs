using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.IO;
using System.Drawing;
using System.Windows.Forms;
using System.Diagnostics;

namespace EDCServer
{
    public partial class Form1 : Form
    {
        TcpServer tcpServer;
        Config config;

        public Form1()
        {
            InitializeComponent();
            try
            {
                config = new Config(Path.Combine(Application.StartupPath, C.kIniFile));
            }
            catch (Exception ex)
            {
                MessageBox.Show(System.String.Format("³]©wÀÉÅª¨ú¿ù»~: {0}", ex.Message));
                Environment.Exit(-1);
            }
            tcpServer = new TcpServer(config, this);
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            
        }

        private void strSrvBtn_Click(object sender, EventArgs e)
        {
            tcpServer.Start();
            logList.Items.Add("Server Start");
        }

        private void form_close(object sender, FormClosingEventArgs e)
        {
            tcpServer.Close();
            Environment.Exit(0);
        }   
    }


}