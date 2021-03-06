using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Net.Sockets;
using System.Net;
using System.Collections;

namespace TestClient
{
    public partial class Client : Form
    {
        private TcpClient client;

        public Client()
        {
            InitializeComponent();
            client = new TcpClient();
        }

        private void Client_Load(object sender, EventArgs e)
        {
            DisconnectBtn.Enabled = false;
        }

        private void ConectBtn_Click(object sender, EventArgs e)
        {
            IPEndPoint serverEndPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 3000);
            client.Connect(serverEndPoint);
            ConectBtn.Enabled = false;
            DisconnectBtn.Enabled = true;
        }

        private void SendBtn_Click(object sender, EventArgs e)
        {
            NetworkStream clientStream = client.GetStream();
            ASCIIEncoding encoder = new ASCIIEncoding();
            string read_str;
            byte[] read_buf = new byte[4096];
            int read_len;
            byte[] buffer = encoder.GetBytes(escape(sendTxt.Text));
            //byte[] buffer = encoder.GetBytes("SYNC_PROJ\t001");

            try
            {
                clientStream.Write(buffer, 0, buffer.Length);
                clientStream.Flush();
            }
            catch
            {
                clientStream.Close();
                client.Close();
            }
            //read_len = clientStream.Read(read_buf, 0, 4096);
            //read_str = encoder.GetString(read_buf, 0, read_len);
            //recvTxt.Text = read_str;
        }

        private string escape(string str)
        {
            string ret_str = "";
            ret_str = str.Replace("\\t", "\t")
                        .Replace("\\n", "\n");
            return ret_str;
        }

        private void DisconnectBtn_Click(object sender, EventArgs e)
        {
            this.client.Close();
        }
    }
}