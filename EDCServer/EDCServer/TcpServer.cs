using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Sockets;
using System.Threading;
using System.Net;
using System.Collections;
using System.Data;
using System.Data.SqlClient;

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

            tcpClient.Close();
            connectClient.Remove(tcpClient);
            System.Diagnostics.Debug.WriteLine("Stop read from client" + client.GetHashCode());
        }

        private string get_employee_list(string[] plist)
        {
            StringBuilder emp_list = new StringBuilder();
            SqlCommand sql_cmd;
            SqlDataReader sql_reader;

            DataSet emp_dataset;
            SqlDataAdapter data_adapter;

            sql_cmd = new SqlCommand("GetAllEmployeeList", sqlConn);
            sql_cmd.CommandType = System.Data.CommandType.StoredProcedure;
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

            sql_cmd = new SqlCommand("GetProjectList", sqlConn);
            sql_cmd.CommandType = System.Data.CommandType.StoredProcedure;
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
            SqlCommand sql_cmd;
            SqlDataReader sql_reader;
            DataSet edc_dataset;
            SqlDataAdapter data_adapter;
            string edc_id = plist[1];
            string mono_a3 = "";
            string mono_a4 = "";
            string color_a3 = "";
            string color_a4 = "";

            sql_cmd = new SqlCommand("GetEDCList", sqlConn);
            sql_cmd.Parameters.Add(C.kFieldEDCID, SqlDbType.NVarChar);
            sql_cmd.Parameters[C.kFieldEDCID].Value = edc_id.Trim();
            sql_cmd.CommandType = System.Data.CommandType.StoredProcedure;
            sql_reader = sql_cmd.ExecuteReader();

            while (sql_reader.Read())
            {
                edc_list.Append(sql_reader["EDCNO"]);
                edc_list.Append("\t");
                edc_list.Append(sql_reader["EDCIP"]);
                edc_list.Append("\t");
                edc_list.Append(sql_reader["MachineIP"]);
                edc_list.Append("\t");
                edc_list.Append(sql_reader["EDCLimitTime"]);
                edc_list.Append("\t");
                edc_list.Append(sql_reader["EDCShowLimitTime"]);
                edc_list.Append("\t");
            }

            if (sql_reader.NextResult())
            {
                while (sql_reader.Read())
                {
                    if (sql_reader["PrintType"].ToString() == C.kMonoA3)
                    {
                        mono_a3 = sql_reader["PrintPay"].ToString();
                    }
                    else if (sql_reader["PrintType"].ToString() == C.kMonoA4)
                    {
                        mono_a4 = sql_reader["PrintPay"].ToString();
                    }
                    else if (sql_reader["PrintType"].ToString() == C.kColorA3)
                    {
                        color_a3 = sql_reader["PrintPay"].ToString();
                    }
                    else if (sql_reader["PrintType"].ToString() == C.kColorA4)
                    {
                        color_a4 = sql_reader["PrintPay"].ToString();
                    }
                }
            }
            edc_list.Append(mono_a3);
            edc_list.Append("\t");
            edc_list.Append(mono_a4);
            edc_list.Append("\t");
            edc_list.Append(color_a3);
            edc_list.Append("\t");
            edc_list.Append(color_a4);
            edc_list.Append("\n");
            sql_reader.Close();
            edc_list.Insert(0, edc_list.Length.ToString() + "|");
            return edc_list.ToString();
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
                            SqlCommand sql_edc_data = new SqlCommand("INSERT INTO [dbo].[PQCardInfo] (EDCNO, CardNumber, UserAccount, ProjectNO, MachineNO, MachineIP, CardDT)" +
                            "VALUES (SELECT T1.EDCNO, T2.Card  FROM [dbo].[DataEDC]", sqlConn);
                            sql_edc_data.Parameters.Add(C.kFieldEDCLog, SqlDbType.NVarChar);
                            sql_edc_data.Parameters[C.kFieldEDCLog].Value = recv_list[i].Trim();
                            sql_edc_data.CommandType = System.Data.CommandType.Text;
                            if (sql_cmd.ExecuteNonQuery() != 1)
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

    }
}
