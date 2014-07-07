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
using System.Diagnostics;
    
namespace EDCServer
{
    class TcpServer
    {
        private TcpListener tcpListener;
        private Thread listenThread;
        private string sqlConnStr;
        private List<TcpClient> connectClient;
        private int port;
        private Form1 serverfrm;

        public TcpServer(Config config, Form1 frm)
        {
            this.connectClient = new List<TcpClient>();
            this.sqlConnStr = String.Format("Data Source={0};User Id={2}; Password={3}; Initial Catalog={1}; Max Pool Size=200; Pooling=true;Min Pool Size=5",
                config.source, config.db, config.user, config.password);
            this.serverfrm = frm;   
            this.port = config.port;
        }


        public void Start()
        {
            try
            {
                this.tcpListener = new TcpListener(IPAddress.Any, port);
                this.listenThread = new Thread(new ThreadStart(ListenForClients));
                this.listenThread.Start();
                EventLog.WriteEntry("EDCAgent", "Start EDCAgent", EventLogEntryType.Information);
            }
            catch (Exception ex)
            {
                EventLog.WriteEntry("EDCAgent", String.Format("Error occur when start Agent: {0}", ex.Message), EventLogEntryType.Error);
                Application.Exit();
            }
        }


        public void Close()
        {       
            foreach (TcpClient client in connectClient)
            {
                System.Diagnostics.Debug.WriteLine("Close connect" + client.GetHashCode());
                EventLog.WriteEntry("EDCAgent", "Close Connect", EventLogEntryType.Information);
                client.Close();
            }

            if (listenThread != null && listenThread.ThreadState == System.Threading.ThreadState.Running)
            {
                this.listenThread.Abort();
            }
        }


        private void ListenForClients()
        {
            if (!tcpListener.Server.IsBound)
                this.tcpListener.Start();

            while (true)
            {
                //blocks until a client has connected to the server
                try
                {
                    TcpClient client = this.tcpListener.AcceptTcpClient();
                    connectClient.Add(client);

                    System.Diagnostics.Debug.WriteLine("New Connect, total:" + connectClient.Count);
                    EventLog.WriteEntry("EDCAgent", "New Client connected: " + client.Client.RemoteEndPoint.ToString(), EventLogEntryType.Information);
                    //create a thread to handle communication 
                    //with connected client
                    Thread clientThread = new Thread(new ParameterizedThreadStart(HandleClientComm));
                    clientThread.Start(client);
                }
                catch
                {
                    EventLog.WriteEntry("EDCAgent", "Error occur when establish client thread.", EventLogEntryType.Information);
                }


            }
        }


        private void HandleClientComm(object client)
        {
            TcpClient tcpClient = (TcpClient)client;
            string clientIP = tcpClient.Client.RemoteEndPoint.ToString();
            NetworkStream clientStream;
            byte[] buf_read;
            byte[] send_buf;
            int bytes_read;
            string recv_str = "";
            
            List<string> command_list = new List<string>();

            try
            {
                clientStream = tcpClient.GetStream();
                System.Diagnostics.Debug.WriteLine("Start read from client" + clientIP);
                EventLog.WriteEntry("EDCAgent", "Client thread start: " + clientIP, EventLogEntryType.Information);

                while (true)
                {
                    bytes_read = 0;
                    try
                    {
                        //blocks until a client sends a message 
                        buf_read = new byte[C.kBufSize];
                        bytes_read = clientStream.Read(buf_read, 0, C.kBufSize);
                    }
                    catch
                    {
                        //a socket error has occured
                        System.Diagnostics.Debug.WriteLine("IOError");
                        EventLog.WriteEntry("EDCAgent", "Client stream read failure: " + clientIP, EventLogEntryType.FailureAudit);
                        break;
                    }

                    if (bytes_read == 0)
                    {
                        //the client has disconnected from the server
                        System.Diagnostics.Debug.WriteLine("Disconnect");
                        EventLog.WriteEntry("EDCAgent", "EDCClient disconnect: " + clientIP, EventLogEntryType.Information);
                        break;
                        //throw new Exception("Disconnect");
                    }

                    //message has successfully been received
                    ASCIIEncoding encoder = new ASCIIEncoding();
                    recv_str += encoder.GetString(buf_read, 0, bytes_read);
                    //EventLog.WriteEntry("EDCAgent", "Client thread receive:" + recv_str, EventLogEntryType.Information);

                    while (true)
                    {
                        // header include '|'
                        int header_len = recv_str.IndexOf("|") + 1;
                        int content_length = 0;
                        if (header_len <= 1)
                        {
                            //Can't find '|', read more
                            break;
                        }

                        try
                        {
                            content_length = int.Parse(recv_str.Substring(0, header_len - 1));
                        }
                        catch
                        {
                            //Protocol error, skip current receive payload
                            recv_str = "";
                            break;
                        }

                        if (recv_str.Length - header_len < content_length)
                        {
                            //There are still unread payload, read more
                            break;
                        }

                        // OK, now I have at least one command
                        string command = recv_str.Substring(header_len, content_length);

                        string[] cmd_tokens = command.Split('\n')[0].Split('\t');
                        string cmd = cmd_tokens[0];
                        string send_str = "";

                        switch (cmd)
                        {
                            case C.kSyncVerCmd:
                                update_edc_version(cmd_tokens);
                                EventLog.WriteEntry("EDCAgent", "Client Send: " + cmd, EventLogEntryType.Information);
                                break;
                            case C.kSyncEmpCmd:
                                send_str = get_employee_list(cmd_tokens);
                                send_buf = encoder.GetBytes(send_str);
                                System.Diagnostics.Debug.WriteLine(send_str);
                                clientStream.Write(send_buf, 0, send_buf.Length);
                                //clientStream.Write(send_buf, 0, 0);
                                clientStream.Flush();
                                //EventLog.WriteEntry("EDCAgent", "Client thread send:" + send_str, EventLogEntryType.Information);
                                break;
                            case C.kSyncEDCCmd:
                                send_str = get_edc_list(cmd_tokens);
                                send_buf = encoder.GetBytes(send_str);
                                System.Diagnostics.Debug.WriteLine(send_str);
                                clientStream.Write(send_buf, 0, send_buf.Length);
                                //clientStream.Write(send_buf, 0, 0);
                                clientStream.Flush();
                                //EventLog.WriteEntry("EDCAgent", "Client thread send:" + send_str, EventLogEntryType.Information);
                                break;
                            case C.kSyncProjCmd:
                                send_str = get_proj_list(cmd_tokens);
                                send_buf = encoder.GetBytes(send_str);
                                System.Diagnostics.Debug.WriteLine(send_str);
                                clientStream.Write(send_buf, 0, send_buf.Length);
                                //clientStream.Write(send_buf, 0, 0);
                                clientStream.Flush();
                                //EventLog.WriteEntry("EDCAgent", "Client thread send:" + send_str, EventLogEntryType.Information);
                                break;
                            case C.kSyncLogCmd:
                                sync_edc_log(command);
                                break;
                            case C.kSyncEmpDeltaCmd:
                                send_str = get_emp_delta(cmd_tokens);
                                send_buf = encoder.GetBytes(send_str);
                                System.Diagnostics.Debug.WriteLine(send_str);
                                //TODO write error check
                                clientStream.Write(send_buf, 0, send_buf.Length);
                                   clientStream.Flush();
                                break;
                            case C.kSyncEDCDeltaCmd:
                                break;
                            case C.kSyncProjDeltaCmd:
                                send_str = get_proj_delta(cmd_tokens);
                                send_buf = encoder.GetBytes(send_str);
                                System.Diagnostics.Debug.WriteLine(send_str);
                                //TODO write error check
                                clientStream.Write(send_buf, 0, send_buf.Length);
                                clientStream.Flush();
                                break;
                            case C.kSyncEmpDeltaOkCmd:
                                emp_delta_ok(cmd_tokens);
                                break;
                            case C.kSyncEDCDeltaOkCmd:
                                edc_delta_ok(cmd_tokens);
                                break;
                            case C.kSyncProjDeltaOkCmd:
                                proj_delta_ok(cmd_tokens);
                                break;
                            default:    
                                break;
                        }

                        int protocol_len = header_len + content_length;
                        if (recv_str.Length > protocol_len)
                        {
                            recv_str = recv_str.Substring(header_len + content_length);
                        }
                        else
                        {
                            recv_str = "";
                            break;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                EventLog.WriteEntry("EDCAgent", string.Format("Unexception error occur in client thread: {0}", ex.Message), EventLogEntryType.Error);
            }
            finally
            {
                tcpClient.Close();
            }

            connectClient.Remove(tcpClient);
            EventLog.WriteEntry("EDCAgent", "Client thread terminated", EventLogEntryType.Information);
        }


        private string get_emp_delta(string[] plist)
        {
            StringBuilder emp_list = new StringBuilder();

            using (SqlConnection sql_conn = new SqlConnection(this.sqlConnStr))
            {
                sql_conn.Open();
                using (SqlCommand sql_cmd = new SqlCommand("sp_SyncEDCInfo", sql_conn))
                {
                    sql_cmd.CommandTimeout = 0;
                    sql_cmd.CommandType = CommandType.StoredProcedure;
                    sql_cmd.Parameters.Add("@state", SqlDbType.VarChar, 20).Value = "get_sync_emp";
                    sql_cmd.Parameters.Add("@EDCNO", SqlDbType.VarChar, 50).Value = plist[1];
                    using (SqlDataReader sql_reader = sql_cmd.ExecuteReader())
                    {
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
                            emp_list.Append("\t");
                            emp_list.Append(sql_reader["IsColorPrint"]);
                            emp_list.Append("\t");
                            emp_list.Append(sql_reader["StatusType"]);
                            emp_list.Append("\n");
                        }
                        emp_list.Insert(0, emp_list.Length.ToString() + "|");
                    }
                }
            }

            return emp_list.ToString();
        }


        private string get_proj_delta(string[] plist)
        {
            StringBuilder proj_list = new StringBuilder();

            using (SqlConnection sql_conn = new SqlConnection(this.sqlConnStr))
            {
                sql_conn.Open();
                using (SqlCommand sql_cmd = new SqlCommand("sp_SyncEDCInfo", sql_conn))
                {
                    sql_cmd.CommandTimeout = 0;
                    sql_cmd.CommandType = CommandType.StoredProcedure;
                    sql_cmd.Parameters.Add("@state", SqlDbType.VarChar, 20).Value = "get_sync_prj";
                    sql_cmd.Parameters.Add("@EDCNO", SqlDbType.VarChar, 50).Value = plist[1];
                    using (SqlDataReader sql_reader = sql_cmd.ExecuteReader())
                    {
                        while (sql_reader.Read())
                        {
                            proj_list.Append(sql_reader["ProjectNO"]);
                            proj_list.Append("\t");
                            proj_list.Append(sql_reader["StatusType"]);
                            proj_list.Append("\n");
                        }
                        proj_list.Insert(0, proj_list.Length.ToString() + "|");
                    }
                }
            }
            return proj_list.ToString();
        }


        private void emp_delta_ok(string[] plist)
        {
            using (SqlConnection sql_conn = new SqlConnection(this.sqlConnStr))
            {
                sql_conn.Open();
                using (SqlCommand sql_cmd = new SqlCommand("sp_SyncEDCInfo", sql_conn))
                {
                    sql_cmd.CommandTimeout = 0;
                    sql_cmd.CommandType = CommandType.StoredProcedure;
                    sql_cmd.Parameters.Add("@state", SqlDbType.VarChar, 20).Value = "del_sync_emp";
                    sql_cmd.Parameters.Add("@EDCNO", SqlDbType.VarChar, 50).Value = plist[1];
                    sql_cmd.ExecuteNonQuery();
                }
            }
        }


        private void edc_delta_ok(string[] plist)
        {
            EventLog.WriteEntry("EDCAgent", "Not implement this function.", EventLogEntryType.Warning);
            return;
            /*
            SqlCommand sql_cmd;
             * 
            sql_cmd = new SqlCommand("sp_SyncEDCInfo", sqlConn);
            sql_cmd.CommandType = CommandType.StoredProcedure;
            sql_cmd.Parameters.Add("@state", SqlDbType.VarChar, 20).Value = "del_sync_edc";
            sql_cmd.Parameters.Add("@EDCNO", SqlDbType.VarChar, 50).Value = plist[1];
            sql_cmd.ExecuteNonQuery();
            */
        }


        private void proj_delta_ok(string[] plist)
        {
            using (SqlConnection sql_conn = new SqlConnection(this.sqlConnStr))
            {
                sql_conn.Open();
                using (SqlCommand sql_cmd = new SqlCommand("sp_SyncEDCInfo", sql_conn))
                {
                    sql_cmd.CommandTimeout = 0;
                    sql_cmd.CommandType = CommandType.StoredProcedure;
                    sql_cmd.Parameters.Add("@state", SqlDbType.VarChar, 20).Value = "del_sync_prj";
                    sql_cmd.Parameters.Add("@EDCNO", SqlDbType.VarChar, 50).Value = plist[1];
                    sql_cmd.ExecuteNonQuery();
                }
            }
        }


        private string get_employee_list(string[] plist)
        {
            StringBuilder emp_list = new StringBuilder();
            //呼叫SP
            //依據資料組成一樣的list, 以\n分隔
            string sql_select = "SELECT " +
                                "[dbo].[DataDepartment].[DepartmentName]," +
                                "[dbo].[DataEmployee].[DepartmentNo]," +
                                "[dbo].[DataEmployee].[UserNumber]," +
                                "[dbo].[DataEmployee].[CardNumber]," +
                                "[dbo].[DataEmployee].[IniQuota]," +
                                "[dbo].[DataEmployee].[NowQuota]," +
                                "[dbo].[DataEmployee].[IsColorPrint] " +
                                "FROM [dbo].[DataEmployee] JOIN [dbo].[DataDepartment]" +
                                "ON [dbo].[DataEmployee].[DepartmentNo] = [dbo].[DataDepartment].[DepartmentNo]";

            using (SqlConnection sql_conn = new SqlConnection(this.sqlConnStr))
            {
                sql_conn.Open();
                using (SqlCommand sql_cmd = new SqlCommand(sql_select, sql_conn))
                {
                    sql_cmd.CommandTimeout = 0;
                    sql_cmd.CommandType = System.Data.CommandType.Text;
                    using (SqlDataReader sql_reader = sql_cmd.ExecuteReader())
                    {
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
                            emp_list.Append("\t");
                            emp_list.Append(sql_reader["IsColorPrint"]);
                            emp_list.Append("\n");
                        }
                        emp_list.Insert(0, emp_list.Length.ToString() + "|");
                    }
                }
            }
            return emp_list.ToString();
        }


        private string get_proj_list(string[] plist)
        {
            StringBuilder proj_list = new StringBuilder();
            string sql_str = "SELECT [dbo].[DataProject].[ProjectNO] FROM [dbo].[DataProject]";
            using (SqlConnection sql_conn = new SqlConnection(this.sqlConnStr))
            {
                sql_conn.Open();
                using (SqlCommand sql_cmd = new SqlCommand(sql_str, sql_conn))
                {
                    sql_cmd.CommandTimeout = 0;
                    sql_cmd.CommandType = System.Data.CommandType.Text;
                    using (SqlDataReader sql_reader = sql_cmd.ExecuteReader())
                    {
                        while (sql_reader.Read())
                        {
                            proj_list.Append(sql_reader["ProjectNO"]);
                            proj_list.Append("\n");
                        }
                        proj_list.Insert(0, proj_list.Length.ToString() + "|");
                    }
                }
            }
            return proj_list.ToString();
        }


        private string get_edc_list(string[] plist)
        {
            StringBuilder edc_list = new StringBuilder();
            //SqlDataReader sql_reader;
            DataSet edc_dataset = new DataSet();
            DataSet pp_dataset = new DataSet();

            string edc_id = plist[1];
            string gray_big = "";
            string gray_small = "";
            string gray_other = "";
            string color_big = "";
            string color_small = "";
            string color_other = "";
            int[] paper_size = new int[]{4, 5};     //NOTE default value is here!!!

            using (SqlConnection sql_conn = new SqlConnection(this.sqlConnStr))
            {
                sql_conn.Open();
                string sql_select = string.Format("SELECT * FROM [dbo].[DataEDC] WHERE [dbo].[DataEDC].[EDCNO] = '{0}'", edc_id);
                using (SqlCommand sql_cmd = new SqlCommand(sql_select, sql_conn))
                {
                    sql_cmd.CommandType = System.Data.CommandType.Text;
                    using (SqlDataAdapter sql_adapter = new SqlDataAdapter(sql_cmd))
                    {
                        sql_cmd.CommandTimeout = 0;
                        sql_adapter.Fill(edc_dataset, "EDC");
                    }
                }

                if (edc_dataset.Tables[0].Rows.Count == 0)
                {
                    EventLog.WriteEntry("EDCAgent", "Write EDC heartbeat to DB error", EventLogEntryType.Information);
                }
                else
                {
                    string sql_heartbeat = string.Format("UPDATE [dbo].[DataEDC] SET [dbo].[DataEDC].[EDCDT] = getdate() WHERE [dbo].[DataEDC].[EDCNO] = '{0}'", edc_id);
                    using (SqlCommand sql_cmd = new SqlCommand(sql_heartbeat, sql_conn))
                    {
                        sql_cmd.CommandType = System.Data.CommandType.Text;
                        if (sql_cmd.ExecuteNonQuery() != 1)
                        {
                            EventLog.WriteEntry("EDCAgent", "Write EDC heartbeat to DB error", EventLogEntryType.Error);
                        }
                    }
                    
                    string sql_printpay = string.Format("SELECT * FROM [dbo].[DataPrintPay]");
                    using (SqlCommand sql_cmd = new SqlCommand(sql_printpay, sql_conn))
                    {
                        sql_cmd.CommandType = System.Data.CommandType.Text;
                        using (SqlDataAdapter sql_adapter = new SqlDataAdapter(sql_cmd))
                        {
                            sql_adapter.Fill(pp_dataset, "PrintPay");

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
                                    paper_size = get_smaller_paper_size(pp_row["PaperType"].ToString());
                                }
                                else if (pp_row["PrintType"].ToString() == C.kGrayOther)
                                {
                                    gray_other = pp_row["PrintPay"].ToString();
                                }
                                else if (pp_row["PrintType"].ToString() == C.kColorBig)
                                {
                                    color_big = pp_row["PrintPay"].ToString();
                                }
                                else if (pp_row["PrintType"].ToString() == C.kColorSmall)
                                {
                                    color_small = pp_row["PrintPay"].ToString();
                                }
                                else if (pp_row["PrintType"].ToString() == C.kColorOther)
                                {
                                    color_other = pp_row["PrintPay"].ToString();
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
                        }
                    }
                }
            }

            edc_list.Insert(0, edc_list.Length.ToString() + "|");
            return edc_list.ToString();
        }


        private void update_edc_version(string[] plist)
        {
            if (plist.Length < 3)
            {
                throw new Exception(string.Format("EDC version command is malform: '{0}'", string.Join(" ", plist)));
            }
            string edc_version = plist[1];
            string edc_no = plist[2];

            using (SqlConnection sql_conn = new SqlConnection(this.sqlConnStr))
            {
                sql_conn.Open();
                using (SqlCommand sql_update = new SqlCommand("UPDATE [dbo].[DataEDC] SET [EDCVer] = @edc_ver WHERE [EDCNO] = @edc_no", sql_conn))
                {
                    sql_update.CommandTimeout = 0;
                    sql_update.Parameters.AddWithValue("@edc_ver", edc_version);
                    sql_update.Parameters.AddWithValue("@edc_no", edc_no);
                    sql_update.CommandType = System.Data.CommandType.Text;
                    sql_update.ExecuteNonQuery();
                }
            }
        }


        private int[] get_smaller_paper_size(string paper_type)
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

            using (SqlConnection sql_conn = new SqlConnection(this.sqlConnStr))
            {
                sql_conn.Open();
                //Start from 2nd line
                for (int i = 1; i < recv_list.Length; i++)
                {
                    if (recv_list[i].Trim().Length != 0)
                    {
                        EDCLOG edc_log = parse_log(recv_list[i].Trim());
                        using (SqlCommand sql_insert_log = new SqlCommand("INSERT INTO [dbo].[EDCLogTmp] (EDCLog) VALUES (@edc_log)", sql_conn))
                        {
                            sql_insert_log.CommandTimeout = 0;
                            sql_insert_log.Parameters.Add(C.kFieldEDCLog, SqlDbType.NVarChar);
                            sql_insert_log.Parameters[C.kFieldEDCLog].Value = recv_list[i].Trim();
                            sql_insert_log.CommandType = System.Data.CommandType.Text;
                            if (sql_insert_log.ExecuteNonQuery() != 1)
                            {
                                EventLog.WriteEntry("EDCAgent", "Insert EDCLOG to DB failure", EventLogEntryType.Error);
                            }
                            else
                            {
                                EventLog.WriteEntry("EDCAgent", "Client thread insert EDC_log: " + recv_list[i], EventLogEntryType.SuccessAudit);
                            }
                        }
                        
                        if (edc_log.type == "CARD")
                        {
                            string[] content_token = edc_log.content.Split(' ');
                            if (content_token[0] == "VALID")
                            {
                                //TODO Exception here!!!其他資訊: 索引在陣列的界限之外。
                                //getdate()改用edc_log.log_time寫入EDC的時間
                                string sql_insert_pq = string.Format("INSERT INTO [dbo].[PQCardInfo] (EDCNO, CardNumber, UserNumber, ProjectNO, CardDT)" +
                                    "VALUES ('{0}', '{1}', '{2}', '{3}', '{4}')", edc_log.edc_no, content_token[1], edc_log.emp_no, edc_log.project_no, edc_log.log_time);
                                using (SqlCommand cmd_Insert_pq = new SqlCommand(sql_insert_pq, sql_conn))
                                {
                                    cmd_Insert_pq.CommandType = System.Data.CommandType.Text;
                                    if (cmd_Insert_pq.ExecuteNonQuery() != 1)
                                    {
                                        EventLog.WriteEntry("EDCAgent", "Insert PQCardInfo to DB error", EventLogEntryType.Error);
                                    }
                                    else
                                    {
                                        EventLog.WriteEntry("EDCAgent", "Client thread insert into PQCardInfo: " + recv_list[i], EventLogEntryType.SuccessAudit);
                                    }
                                }
                            }
                        }
                        else if (edc_log.type == "PRINT" || edc_log.type == "COPY")
                        {
                            List<KeyValuePair<string, int>> paper_usage = parse_count_content(edc_log.content);
                            foreach (KeyValuePair<string, int> usage in paper_usage)
                            {
                                string sql_insert_cc = string.Format("INSERT INTO [dbo].[CopyCount] (EDCNO, ProjectNO, UserNumber, PrintType, PrintCount, UseDT)" +
                                        "VALUES ('{0}', '{1}', '{2}', '{3}', '{4}', '{5}')",
                                        edc_log.edc_no, edc_log.project_no, edc_log.emp_no, usage.Key, usage.Value, edc_log.log_time);
                                //MessageBox.Show(sql_insert_cc);
                                using (SqlCommand cmd_insert_cc = new SqlCommand(sql_insert_cc, sql_conn))
                                {
                                    cmd_insert_cc.CommandType = System.Data.CommandType.Text;
                                    if (cmd_insert_cc.ExecuteNonQuery() != 1)
                                    {
                                        EventLog.WriteEntry("EDCAgent", "Client thread insert into SEFScanInfo failure." + recv_list[i], EventLogEntryType.Error);
                                    }
                                    else
                                    {
                                        EventLog.WriteEntry("EDCAgent", "Client thread insert into CopyCount: " + recv_list[i], EventLogEntryType.SuccessAudit);
                                    }
                                }
                            }
                        }
                        else if (edc_log.type == "SCAN")
                        {
                            string sql_scan = string.Format("INSERT INTO [dbo].[SEFScanInfo] (EDCNO, UserNumber, ProjectNo, ScanDT)" +
                                "VALUES ( '{0}', '{1}', '{2}', '{3}')", edc_log.edc_no, edc_log.emp_no, edc_log.project_no, edc_log.log_time);
                            using (SqlCommand cmd_scan = new SqlCommand(sql_scan, sql_conn))
                            {
                                cmd_scan.CommandType = System.Data.CommandType.Text;
                                if (cmd_scan.ExecuteNonQuery() != 1)
                                {
                                    EventLog.WriteEntry("EDCAgent", "Client thread insert into SEFScanInfo failure." + recv_list[i], EventLogEntryType.Error);
                                }
                                else
                                {
                                    EventLog.WriteEntry("EDCAgent", "Client thread insert into SEFScanInfo Success." + recv_list[i], EventLogEntryType.SuccessAudit);
                                }
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

            edc_log.content = (token.Length > 5)?token[5]:"";
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
