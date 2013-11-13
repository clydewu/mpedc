using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Sockets;
using System.Threading;
using System.Net;
using System.Collections;
using System.Data;
using System.Data.SqlClient;
using System.Windows.Forms;

namespace EDCServer
{
    class TcpServer
    {
        private TcpListener tcpListener;
        private Thread listenThread;
        private SqlConnection sqlConn;
        private List<TcpClient> connectClient;
        private int port;
        private Form1 serverfrm;

        public TcpServer(Config config, Form1 frm)
        {
            this.connectClient = new List<TcpClient>();
            this.sqlConn = new SqlConnection(System.String.Format("Data Source={0};User Id={2}; Password={3}; Initial Catalog={1}",
                config.source, config.db, config.user, config.password));
            this.serverfrm = frm;
        }

        public void Start()
        {
            this.sqlConn.Open();
            this.tcpListener = new TcpListener(IPAddress.Any, 3000);
            this.listenThread = new Thread(new ThreadStart(ListenForClients));
            this.listenThread.Start();
        }

        public void Close()
        {
            foreach (TcpClient client in connectClient)
            {
                System.Diagnostics.Debug.WriteLine("Close connect" + client.GetHashCode());
                client.Close();
            }
            sqlConn.Close();
            this.listenThread.Abort();
        }

        private void ListenForClients()
        {
            if (!tcpListener.Server.IsBound)
                this.tcpListener.Start();

            while (true)
            {
                //blocks until a client has connected to the server
                TcpClient client = this.tcpListener.AcceptTcpClient();
                connectClient.Add(client);

                System.Diagnostics.Debug.WriteLine("New Connect, total:" + connectClient.Count);
                //create a thread to handle communication 
                //with connected client
                Thread clientThread = new Thread(new ParameterizedThreadStart(HandleClientComm));
                clientThread.Start(client);
            }
        }

        private void HandleClientComm(object client)
        {
            TcpClient tcpClient = (TcpClient)client;
            NetworkStream clientStream;

            byte[] message = new byte[4096];
            byte[] send_buf;
            int bytesRead;
            string recv;

            clientStream = tcpClient.GetStream();
            System.Diagnostics.Debug.WriteLine("Start read from client" + client.GetHashCode());
            try
            {
                while (true)
                {
                    bytesRead = 0;

                    try
                    {
                        //blocks until a client sends a message
                        bytesRead = clientStream.Read(message, 0, 4096);
                    }
                    catch
                    {
                        //a socket error has occured
                        System.Diagnostics.Debug.WriteLine("IOError");
                        break;
                    }

                    if (bytesRead == 0)
                    {
                        //the client has disconnected from the server
                        System.Diagnostics.Debug.WriteLine("Disconnect");
                        break;
                    }

                    //message has successfully been received
                    ASCIIEncoding encoder = new ASCIIEncoding();
                    recv = encoder.GetString(message, 0, bytesRead);
                    System.Diagnostics.Debug.WriteLine(recv);

                    string[] protocol_list = recv.Split('\n')[0].Split('\t');
                    string cmd = protocol_list[0];

                    switch (cmd)
                    {
                        case C.kSyncEmpCmd:
                            send_buf = encoder.GetBytes(get_employee_list(protocol_list));
                            System.Diagnostics.Debug.WriteLine(send_buf);
                            clientStream.Write(send_buf, 0, send_buf.Length);
                            //clientStream.Write(send_buf, 0, 0);
                            clientStream.Flush();
                            break;
                        case C.kSyncEDCCmd:
                            send_buf = encoder.GetBytes(get_edc_list(protocol_list));
                            System.Diagnostics.Debug.WriteLine(send_buf);
                            clientStream.Write(send_buf, 0, send_buf.Length);
                            //clientStream.Write(send_buf, 0, 0);
                            clientStream.Flush();
                            break;
                        case C.kSyncProjCmd:
                            send_buf = encoder.GetBytes(get_proj_list(protocol_list));
                            System.Diagnostics.Debug.WriteLine(send_buf);
                            clientStream.Write(send_buf, 0, send_buf.Length);
                            //clientStream.Write(send_buf, 0, 0);
                            clientStream.Flush();
                            break;
                        case C.kSyncLogCmd:
                            sync_edc_log(recv);

                            break;
                        default:
                            break;
                    }
                }
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
                MessageBox.Show(e.StackTrace);

            }

            tcpClient.Close();
            connectClient.Remove(tcpClient);
            System.Diagnostics.Debug.WriteLine("Stop read from client" + client.GetHashCode());
        }

        private string get_employee_list(string[] plist)
        {
            StringBuilder emp_list = new StringBuilder();
            SqlCommand sql_cmd;
            SqlDataReader sql_reader;

            string sql_select = "SELECT " +
                                "[dbo].[DataDepartment].[DepartmentName]," +
		                        "[dbo].[DataEmployee].[DepartmentNo]," +
		                        "[dbo].[DataEmployee].[UserNumber]," +
		                        "[dbo].[DataEmployee].[CardNumber]," +
		                        "[dbo].[DataEmployee].[IniQuota]," +
		                        "[dbo].[DataEmployee].[NowQuota] " +
	                            "FROM [dbo].[DataEmployee] JOIN [dbo].[DataDepartment]" +
	                            "ON [dbo].[DataEmployee].[DepartmentNo] = [dbo].[DataDepartment].[DepartmentNo]";
            sql_cmd = new SqlCommand(sql_select, sqlConn);
            sql_cmd.CommandType = System.Data.CommandType.Text;
            sql_reader = sql_cmd.ExecuteReader();

            while (sql_reader.Read())
            {
                emp_list.Append(sql_reader["DepartmentName"]);
                emp_list.Append("\t");
                emp_list.Append(sql_reader["DepartmentNo"]);
                emp_list.Append("\t");
                emp_list.Append(sql_reader["UserNumber"]);
                emp_list.Append("\t");
                emp_list.Append(sql_reader["CardNumber"]);
                emp_list.Append("\t");
                emp_list.Append(sql_reader["IniQuota"]);
                emp_list.Append("\t");
                emp_list.Append(sql_reader["NowQuota"]);
                emp_list.Append("\n");
            }
            sql_reader.Close();
            emp_list.Insert(0, emp_list.Length.ToString() + "|");
            return emp_list.ToString();
        }

        private string get_proj_list(string[] plist)
        {
            StringBuilder proj_list = new StringBuilder();
            SqlCommand sql_cmd;
            SqlDataReader sql_reader;

            sql_cmd = new SqlCommand("SELECT [dbo].[DataProject].[ProjectNO] FROM [dbo].[DataProject]", sqlConn);
            sql_cmd.CommandType = System.Data.CommandType.Text;
            sql_reader = sql_cmd.ExecuteReader();

            while (sql_reader.Read())
            {
                proj_list.Append(sql_reader["ProjectNO"]);
                proj_list.Append("\n");
            }
            sql_reader.Close();
            proj_list.Insert(0, proj_list.Length.ToString() + "|");
            return proj_list.ToString();
        }

        private string get_edc_list(string[] plist)
        {
            StringBuilder edc_list = new StringBuilder();
            SqlCommand cmd_heartbeat;
            SqlCommand cmd_edc;
            SqlCommand cmd_pp;
            //SqlDataReader sql_reader;
            DataSet edc_dataset = new DataSet();
            DataSet pp_dataset = new DataSet();
            SqlDataAdapter adapter_edc;
            SqlDataAdapter adapter_pp;

            string edc_id = plist[1];
            string gray_big = "";
            string gray_small = "";
            string color_big = "";
            string color_small = "";
            int[] paper_size = new int[]{4, 5};     //NOTE default value is here!!!

            string sql_heartbeat = string.Format("UPDATE [dbo].[DataEDC] SET [dbo].[DataEDC].[EDCDT] = getdate() WHERE [dbo].[DataEDC].[EDCNO] = '{0}'", edc_id);
            cmd_heartbeat = new SqlCommand(sql_heartbeat, sqlConn);
            cmd_heartbeat.CommandType = System.Data.CommandType.Text;
            if (cmd_heartbeat.ExecuteNonQuery() != 1)
            {
                System.Diagnostics.Debug.WriteLine("Write EDC heartbeat to DB error");
            }

            string sql_select = string.Format("SELECT * FROM [dbo].[DataEDC] WHERE [dbo].[DataEDC].[EDCNO] = '{0}'", edc_id);
            cmd_edc = new SqlCommand(sql_select, sqlConn);
            cmd_edc.CommandType = System.Data.CommandType.Text;
            adapter_edc = new SqlDataAdapter(cmd_edc);
            adapter_edc.Fill(edc_dataset, "EDC");

            string sql_printpay = string.Format("SELECT * FROM [dbo].[DataPrintPay]");
            cmd_pp = new SqlCommand(sql_printpay, sqlConn);
            cmd_pp.CommandType = System.Data.CommandType.Text;
            adapter_pp = new SqlDataAdapter(cmd_pp);
            adapter_pp.Fill(pp_dataset, "PrintPay");

            edc_list.Append(edc_dataset.Tables["EDC"].Rows[0]["EDCNO"]);
            edc_list.Append("\t");
            edc_list.Append(edc_dataset.Tables["EDC"].Rows[0]["EDCIP"]);
            edc_list.Append("\t");
            edc_list.Append(edc_dataset.Tables["EDC"].Rows[0]["MachineIP"]);
            edc_list.Append("\t");
            edc_list.Append(edc_dataset.Tables["EDC"].Rows[0]["EDCLimitTime"]);
            edc_list.Append("\t");
            edc_list.Append(edc_dataset.Tables["EDC"].Rows[0]["EDCShowLimitTime"]);
            edc_list.Append("\t");
            
            foreach (DataRow pp_row in pp_dataset.Tables["Printpay"].Rows)
            {
                if (pp_row["PrintType"].ToString() == C.kGrayBig)
                {
                    gray_big = pp_row["PrintPay"].ToString();
                }
                else if (pp_row["PrintType"].ToString() == C.kGraySmall)
                {
                    gray_small = pp_row["PrintPay"].ToString();
                    // !NOTE! only use gray_small setup to determine whole setup
                    paper_size = get_smallesr_paper_size(pp_row["PaperType"].ToString());
                }
                else if (pp_row["PrintType"].ToString() == C.kColorBig)
                {
                    color_big = pp_row["PrintPay"].ToString();
                }
                else if (pp_row["PrintType"].ToString() == C.kColorSmall)
                {
                    color_small = pp_row["PrintPay"].ToString();
                }
            }

            edc_list.Append(gray_big);
            edc_list.Append("\t");
            edc_list.Append(gray_small);
            edc_list.Append("\t");
            edc_list.Append(color_big);
            edc_list.Append("\t");
            edc_list.Append(color_small);
            edc_list.Append("\t");
            edc_list.Append(paper_size[0].ToString());
            edc_list.Append("\t");
            edc_list.Append(paper_size[1].ToString());
            edc_list.Append("\n");
            edc_list.Insert(0, edc_list.Length.ToString() + "|");
            return edc_list.ToString();
        }

        private int[] get_smallesr_paper_size(string paper_type)
        {
            string[] tokens = paper_type.Split(';');
            int[] smallest_paper_size = new int[2];
            smallest_paper_size[0] = 4;
            smallest_paper_size[1] = 5;

            foreach (string token in tokens)
            {
                string psize = token.ToLower();
                int val;

                try
                {
                    val = int.Parse(psize.Substring(1));
                }
                catch
                {
                    // If format error, skip it
                    continue;
                }

                if (psize.StartsWith("a"))
                {
                    if (val < smallest_paper_size[0])
                    {
                        smallest_paper_size[0] = val;
                    }
                }
                else if (psize.StartsWith("b"))
                {
                    if (val < smallest_paper_size[1])
                    {
                        smallest_paper_size[1] = val;
                    }
                }
            }

            return smallest_paper_size;
        }

        private void sync_edc_log(string recv)
        {

            string[] recv_list;

            recv_list = recv.Split('\n');

            //Start from 2nd line
            for (int i = 1; i < recv_list.Length; i++)
            {
                if (recv_list[i].Trim().Length != 0)
                {
                    EDCLOG edc_log = parse_log(recv_list[i].Trim());
                    SqlCommand sql_insert_log = new SqlCommand("INSERT INTO [dbo].[EDCLogTmp] (EDCLog) VALUES (@edc_log)", sqlConn);
                    sql_insert_log.Parameters.Add(C.kFieldEDCLog, SqlDbType.NVarChar);
                    sql_insert_log.Parameters[C.kFieldEDCLog].Value = recv_list[i].Trim();
                    sql_insert_log.CommandType = System.Data.CommandType.Text;
                    if (sql_insert_log.ExecuteNonQuery() != 1)
                    {
                        System.Diagnostics.Debug.WriteLine("Write log to DB error");
                    }

                    if (edc_log.type == "CARD")
                    {
                        string[] content_token = edc_log.content.Split(' ');
                        if (content_token[0] == "VALID")
                        {
                            string sql_insert_pq = string.Format("INSERT INTO [dbo].[PQCardInfo] (EDCNO, CardNumber, UserNumber, ProjectNO, CardDT)" +
                                "VALUES ('{0}', '{1}', '{2}', '{3}', getdate())", edc_log.edc_no, content_token[1], edc_log.emp_no, edc_log.project_no);
                            SqlCommand cmd_Insert_pq = new SqlCommand(sql_insert_pq, sqlConn);
                            cmd_Insert_pq.CommandType = System.Data.CommandType.Text;
                            if (cmd_Insert_pq.ExecuteNonQuery() != 1)
                            {
                                System.Diagnostics.Debug.WriteLine("Write PQCardInfo to DB error");
                            }
                        }
                    }
                    else if (edc_log.type == "PRINT" || edc_log.type == "COPY")
                    {
                        List<KeyValuePair<string, int>> paper_usage = parse_count_content(edc_log.content);
                        foreach (KeyValuePair<string, int> usage in paper_usage)
                        {
                            string sql_insert_cc = string.Format("INSERT INTO [dbo].[CopyCount] (EDCNO, ProjectNO, UserNumber, PrintType, PrintCount, UseDT)" +
                                    "VALUES ('{0}', '{1}', '{2}', '{3}', '{4}', getdate())", edc_log.edc_no, edc_log.project_no, edc_log.emp_no, usage.Key, usage.Value);
                            //MessageBox.Show(sql_insert_cc);
                            SqlCommand cmd_insert_cc = new SqlCommand(sql_insert_cc, sqlConn);
                            cmd_insert_cc.CommandType = System.Data.CommandType.Text;
                            if (cmd_insert_cc.ExecuteNonQuery() != 1)   
                            {
                                System.Diagnostics.Debug.WriteLine("Write CopyCount to DB error");
                                MessageBox.Show("Write CopyCount to DB error");
                            }
                        }
                    }
                }
            }

            //return true;
        }

        private EDCLOG parse_log(string log)
        {
            EDCLOG edc_log = new EDCLOG();
            string[] token = log.Split('\t');

            edc_log.type = token[0];
            edc_log.edc_no = token[1];
            edc_log.project_no = token[2];
            edc_log.emp_no = token[3];
            edc_log.log_time = token[4];
            edc_log.content = token[5];
            return edc_log;
        }

        private List<KeyValuePair<string, int>> parse_count_content(string content)
        {
            List<KeyValuePair<string, int>> usage_list = new List<KeyValuePair<string,int>>();
            string[] paper_usages = content.Split(' ');
            foreach (string paper_usage in paper_usages)
            {
                string[] token = paper_usage.Split(':');
                usage_list.Add(new KeyValuePair<string, int>(token[0], Int16.Parse(token[1])));
            }

            return usage_list;
        }

    }
}
