using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;


namespace EDCServer
{
    public partial class Form1 : Form
    {
        TcpServer tcpServer;

        public Form1()
        {
            InitializeComponent();
            tcpServer = new TcpServer();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            
        }

        private void strSrvBtn_Click(object sender, EventArgs e)
        {
            tcpServer.Start();
        }

        private void form_close(object sender, FormClosingEventArgs e)
        {
            tcpServer.Close();
        }
    }


}