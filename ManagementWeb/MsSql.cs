using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.SqlClient;
using System.Collections.Specialized;
using System.Configuration;
using System.Data;

namespace EDC.DBO
{
    /// <summary>
    /// MsSql 連線
    /// </summary>
    public class MsSql
    {
        private SqlConnection _scConnection;
        private SqlCommand _scCommand;
        private SqlDataAdapter _sdaDataAdapter;
        private SqlTransaction _stTransaction;
        private bool _IsTransaction;
        private StringCollection _SQLCommand;
        private String _SQLCommandText;
        private Params _Params;
        /// <summary>
        /// MsSql connect
        /// </summary>
        public MsSql()
        {
            string ConnectName = ConfigurationManager.AppSettings["ConDB"];

            _IsTransaction = false;
            _SQLCommand = new StringCollection();
            _scConnection = new SqlConnection();
            _scConnection.ConnectionString = ConnectName;
            _scCommand = new SqlCommand();
            _scCommand.CommandTimeout = 50;
            _scCommand.CommandText = string.Empty;
            _SQLCommandText = string.Empty;
            _scCommand.Connection = _scConnection;
            _sdaDataAdapter = new SqlDataAdapter();
            _scConnection.Open();
            _Params = new Params(_scCommand.Parameters);
        }
        /// <summary>
        /// 連線字串
        /// </summary>
        /// <param name="ConnectName"></param>
        public MsSql(string ConnectName)
        {
            //string ConnectName = ConfigurationManager.AppSettings["ConTrtcias"];
            //ConnectName = PPMS.PWD.pwd.getDESDecrypt(ConnectName, 2);

            _IsTransaction = false;
            _SQLCommand = new StringCollection();
            _scConnection = new SqlConnection();
            _scConnection.ConnectionString = ConnectName;
            _scCommand = new SqlCommand();
            _scCommand.CommandTimeout = 50;
            _scCommand.CommandText = string.Empty;
            _SQLCommandText = string.Empty;
            _scCommand.Connection = _scConnection;
            _sdaDataAdapter = new SqlDataAdapter();
            _scConnection.Open();
            _Params = new Params(_scCommand.Parameters);
        }
        /// <summary>
        /// 
        /// </summary>
        public bool Status
        {
            get
            {
                return true;
            }
        }
        /// <summary>
        /// 
        /// </summary>
        public StringCollection Command
        {
            get
            {
                return _SQLCommand;
            }
        }
        /// <summary>
        /// 
        /// </summary>
        public String CommandText
        {
            get
            {
                return _SQLCommandText;
            }
            set
            {
                _SQLCommand.Clear();
                _SQLCommand.Add(value);
            }

        }
        /// <summary>
        /// 執行查詢
        /// </summary>
        public DataTable ExecuteQuery()
        {
            if (_scConnection.State == ConnectionState.Closed)
            {
                _scConnection.Open();
            }

            try
            {
                DataTable dtDataTable = new DataTable("ExecuteQuery1");

                _SQLCommandText = string.Empty;
                for (int i = 0; i < _SQLCommand.Count; i++)
                {
                    _SQLCommandText += _SQLCommand[i];
                }
                _scCommand.CommandText = _SQLCommandText;
                _sdaDataAdapter.SelectCommand = _scCommand;

                _sdaDataAdapter.Fill(dtDataTable);

                _scCommand.Parameters.Clear();
                if (_IsTransaction == false)
                {
                    _scConnection.Close();
                }
                return dtDataTable;
            }
            catch (Exception ex)
            {
                _sdaDataAdapter.SelectCommand.Parameters.Clear();

                _scCommand.Parameters.Clear();
                if (_IsTransaction == false)
                {
                    _scConnection.Close();
                }
                throw ex;
            }
        }
        /// <summary>
        /// 執行查詢
        /// </summary>
        public DataTable ExecuteQuery(int StartRecord, int RecordCount)
        {
            if (_scConnection.State == ConnectionState.Closed)
            {
                _scConnection.Open();
            }

            try
            {
                DataTable dtDataTable = new DataTable("ExecuteQuery2");


                _SQLCommandText = string.Empty;
                for (int i = 0; i < _SQLCommand.Count; i++)
                {
                    _SQLCommandText += _SQLCommand[i];
                }
                _scCommand.CommandText = _SQLCommandText;
                _sdaDataAdapter.SelectCommand = _scCommand;


                try
                {
                    _sdaDataAdapter.Fill(StartRecord, RecordCount, dtDataTable);
                }
                catch
                {
                    _sdaDataAdapter.Fill(StartRecord, RecordCount, dtDataTable);
                }
                _scCommand.Parameters.Clear();
                if (_IsTransaction == false)
                {
                    _scConnection.Close();
                }
                return dtDataTable;
            }
            catch (Exception ex)
            {
                _sdaDataAdapter.SelectCommand.Parameters.Clear();

                _scCommand.Parameters.Clear();
                if (_IsTransaction == false)
                {
                    _scConnection.Close();
                }
                throw ex;
            }
        }
        /// <summary>
        /// 執行查詢-Dirty Read
        /// </summary>
        public DataTable ExecuteDirtyQuery()
        {
            if (_scConnection.State == ConnectionState.Closed)
            {
                _scConnection.Open();
            }

            try
            {
                DataTable dtDataTable = new DataTable("ExecuteDirtyQuery1");

                _SQLCommandText = string.Empty;
                _SQLCommandText = "SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED ";
                for (int i = 0; i < _SQLCommand.Count; i++)
                {
                    _SQLCommandText += _SQLCommand[i];
                }
                _scCommand.CommandText = _SQLCommandText;
                _sdaDataAdapter.SelectCommand = _scCommand;

                _sdaDataAdapter.Fill(dtDataTable);

                _scCommand.Parameters.Clear();
                if (_IsTransaction == false)
                {
                    _scConnection.Close();
                }
                return dtDataTable;
            }
            catch (Exception ex)
            {
                _sdaDataAdapter.SelectCommand.Parameters.Clear();

                _scCommand.Parameters.Clear();
                if (_IsTransaction == false)
                {
                    _scConnection.Close();
                }
                throw ex;
            }
        }
        /// <summary>
        /// 執行查詢-Dirty Read
        /// </summary>
        public DataTable ExecuteDirtyQuery(int StartRecord, int RecordCount)
        {
            if (_scConnection.State == ConnectionState.Closed)
            {
                _scConnection.Open();
            }

            try
            {
                DataTable dtDataTable = new DataTable("ExecuteDirtyQuery2");

                _SQLCommandText = string.Empty;
                _SQLCommandText = "SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED ";
                for (int i = 0; i < _SQLCommand.Count; i++)
                {
                    _SQLCommandText += _SQLCommand[i];
                }
                _scCommand.CommandText = _SQLCommandText;
                _sdaDataAdapter.SelectCommand = _scCommand;


                try
                {
                    _sdaDataAdapter.Fill(StartRecord, RecordCount, dtDataTable);
                }
                catch
                {
                    _sdaDataAdapter.Fill(StartRecord, RecordCount, dtDataTable);
                }
                _scCommand.Parameters.Clear();
                if (_IsTransaction == false)
                {
                    _scConnection.Close();
                }
                return dtDataTable;
            }
            catch (Exception ex)
            {
                _sdaDataAdapter.SelectCommand.Parameters.Clear();

                _scCommand.Parameters.Clear();
                if (_IsTransaction == false)
                {
                    _scConnection.Close();
                }
                throw ex;
            }
        }

        /// <summary>
        /// 執行交易
        /// </summary>
        /// <returns></returns>
        public int ExecuteNonQuery()
        {
            if (_scConnection.State == ConnectionState.Closed)
            {
                _scConnection.Open();
            }

            try
            {
                _SQLCommandText = String.Empty;
                for (int i = 0; i < _SQLCommand.Count; i++)
                {
                    _SQLCommandText += _SQLCommand[i];
                }
                _scCommand.CommandText = _SQLCommandText;
                int iCount = _scCommand.ExecuteNonQuery();
                _scCommand.Parameters.Clear();
                if (_IsTransaction == false)
                {
                    _scConnection.Close();
                }
                return iCount;
            }
            catch (Exception ex)
            {
                _scCommand.Parameters.Clear();
                if (_IsTransaction == false)
                {
                    _scConnection.Close();
                }
                throw ex;
            }
        }

        /// <summary>
        /// 執行交易
        /// SCOPE_IDENTITY 會傳回只在目前範圍內插入的值
        /// </summary>
        /// <param name="IdentityNo"></param>
        /// <returns></returns>
        public int ExecuteNonQuery(out int IdentityNo)
        {
            IdentityNo = 0; // 預設為0 
            if (_scConnection.State == ConnectionState.Closed)
            {
                _scConnection.Open();
            }

            try
            {
                _SQLCommandText = String.Empty;
                for (int i = 0; i < _SQLCommand.Count; i++)
                {
                    _SQLCommandText += _SQLCommand[i];
                }

                // 增加取得流水號的SQL語法
                _scCommand.CommandText = string.Format("{0};{1}", _SQLCommandText, "SELECT @IdentityNo=SCOPE_IDENTITY()");
                _scCommand.Parameters.Add(new SqlParameter("IdentityNo", SqlDbType.Int));
                _scCommand.Parameters["IdentityNo"].Direction = ParameterDirection.Output;

                int iCount = _scCommand.ExecuteNonQuery();

                // 取得新增後的流水號
                IdentityNo = Convert.ToInt32(_scCommand.Parameters["IdentityNo"].Value);


                _scCommand.Parameters.Clear();
                if (_IsTransaction == false)
                {
                    _scConnection.Close();
                }
                return iCount;
            }
            catch (Exception ex)
            {
                _scCommand.Parameters.Clear();
                if (_IsTransaction == false)
                {
                    _scConnection.Close();
                }
                throw ex;
            }
        }
        /// <summary>
        /// @@IDENTITY 值，因為此值會用於複寫觸發程序和預存程序中。
        /// IDENT_CURRENT 不受範圍和工作階段的限制。
        /// </summary>
        /// <param name="TableName"></param>
        /// <returns></returns>
        public int GetIdentityNo(string TableName)
        {
            if (_scConnection.State == ConnectionState.Closed)
            {
                _scConnection.Open();
            }

            try
            {
                DataTable dtDataTable = new DataTable();


                if (_IsTransaction == true)
                {
                    _SQLCommandText = "SELECT @@IDENTITY AS [@@IDENTITY]";
                }
                else
                {
                    _SQLCommandText = String.Format("SELECT IDENT_CURRENT ('{0}') AS IDENT_CURRENT", TableName);
                }


                _scCommand.CommandText = _SQLCommandText;
                _sdaDataAdapter.SelectCommand = _scCommand;
                try
                {
                    _sdaDataAdapter.Fill(dtDataTable);
                    return int.Parse(dtDataTable.Rows[0][0].ToString());
                }
                catch
                {
                    return 0;
                }
            }
            finally
            {
                _scCommand.Parameters.Clear();
                if (_IsTransaction == false)
                {
                    _scConnection.Close();
                }
            }

        }

        /// <summary>
        /// 建立欄位及給值　obj.FieldByName("欄位").String="Val";
        /// </summary>
        /// <param name="sParamName"></param>
        public Params ParamByName(string sParamName)
        {
            _Params.ParamName = "@" + sParamName;
            return _Params;
        }
        /// <summary>
        /// 
        /// </summary>
        public class Params
        {
            private string _ParamName;
            private string _AsString;
            private int _AsInteger;
            private Double _AsDouble;
            private Decimal _AsDecimal;
            private DateTime _AsDateTime;
            private string _AsText;
            private bool _AsBool;
            private DBNull _AsDBNull;
            private SqlParameterCollection _Parameters;
            /// <summary>
            /// 
            /// </summary>
            /// <param name="Parameters"></param>
            public Params(SqlParameterCollection Parameters)
            {
                _Parameters = Parameters;
            }
            /// <summary>
            /// 取得欄位名稱
            /// </summary>
            public string ParamName
            {
                get
                {
                    return _ParamName;
                }
                set
                {
                    _ParamName = value;
                }
            }

            #region AsValues
            /// <summary>
            /// 給值(String)
            /// </summary>
            public string AsString
            {
                set
                {
                    _AsString = value;
                    _Parameters.AddWithValue(_ParamName, DbType.AnsiString).Value = _AsString;
                }
                get
                {
                    return _AsString;
                }
            }
            /// <summary>
            /// 給值(Integer)
            /// </summary>
            public int AsInteger
            {
                set
                {
                    _AsInteger = value;
                    _Parameters.AddWithValue(_ParamName, DbType.Int32).Value = _AsInteger;
                }
                get
                {
                    return _AsInteger;
                }
            }
            /// <summary>
            /// 給值(Double) 
            /// </summary>
            public Double AsDouble
            {
                set
                {
                    _AsDouble = value;
                    _Parameters.AddWithValue(_ParamName, DbType.Double).Value = _AsDouble;
                }
                get
                {
                    return _AsDouble;
                }
            }
            /// <summary>
            /// 給值(Double) 
            /// </summary>
            public Decimal AsDecimal
            {
                set
                {
                    _AsDecimal = value;
                    _Parameters.AddWithValue(_ParamName, DbType.Decimal).Value = _AsDecimal;
                }
                get
                {
                    return _AsDecimal;
                }
            }
            /// <summary>
            /// 給值(DateTime)
            /// </summary>
            public DateTime AsDateTime
            {
                set
                {
                    if (value.Equals(Convert.ToDateTime("0001/01/01")) == true)
                    {
                        _AsDateTime = Convert.ToDateTime("1912/01/01 00:00:00");
                    }
                    else
                    {
                        _AsDateTime = value;
                    }
                    _Parameters.AddWithValue(_ParamName, DbType.DateTime).Value = _AsDateTime;
                }
                get
                {
                    return _AsDateTime;
                }
            }
            /// <summary>
            /// 給值(Text)
            /// </summary>
            public string AsText
            {
                set
                {
                    _AsText = value;
                    _Parameters.AddWithValue(_ParamName, DbType.AnsiString).Value = _AsText;
                }
                get
                {
                    return _AsText;
                }
            }
            /// <summary>
            /// 給值(Bit)
            /// </summary>
            public bool AsBool
            {
                set
                {
                    _AsBool = value;
                    _Parameters.AddWithValue(_ParamName, DbType.Boolean).Value = _AsBool;
                }
                get
                {
                    return _AsBool;
                }
            }
            /// <summary>
            /// 給值(DBNull)
            /// </summary>
            public DBNull AsDBNull
            {
                set
                {
                    _AsDBNull = value;
                    _Parameters.AddWithValue(_ParamName, DbType.Object).Value = _AsDBNull;
                }
                get
                {
                    return _AsDBNull;
                }
            }
            #endregion

            #region SetValues
            /// <summary>
            /// 給值(String)
            /// </summary>
            public void SetString(object Value)
            {
                SetString(Value, string.Empty);
            }
            /// <summary>
            /// 
            /// </summary>
            /// <param name="Value"></param>
            /// <param name="DefValue"></param>
            public void SetString(object Value, object DefValue)
            {
                _Parameters.AddWithValue(_ParamName, DbType.AnsiString).Value = DbConvert.ToString(Value, DefValue.ToString());
            }

            /// <summary>
            /// 給值(Text)
            /// </summary>
            public void SetText(object Value)
            {
                SetText(Value, string.Empty);
            }
            /// <summary>
            /// 
            /// </summary>
            /// <param name="Value"></param>
            /// <param name="DefValue"></param>
            public void SetText(object Value, object DefValue)
            {
                _Parameters.AddWithValue(_ParamName, DbType.AnsiString).Value = DbConvert.ToString(Value, DefValue.ToString());
            }

            /// <summary>
            /// 給值(Integer16)
            /// </summary>
            public void SetInt16(object Value)
            {
                SetInt16(Value, 0);
            }
            /// <summary>
            /// 
            /// </summary>
            /// <param name="Value"></param>
            /// <param name="DefValue"></param>
            public void SetInt16(object Value, object DefValue)
            {
                Int16 i = Convert.ToInt16(DefValue);
                _Parameters.AddWithValue(_ParamName, DbType.Int16).Value = DbConvert.ToInt16(Value, i);
            }

            /// <summary>
            /// 給值(Integer32)
            /// </summary>
            public void SetInt32(object Value)
            {
                SetInt32(Value, 0);
            }

            /// <summary>
            /// 
            /// </summary>
            /// <param name="Value"></param>
            /// <param name="DefValue"></param>
            public void SetInt32(object Value, object DefValue)
            {
                _AsInteger = Convert.ToInt32(DefValue);
                _Parameters.AddWithValue(_ParamName, DbType.Int32).Value = DbConvert.ToInt32(Value, _AsInteger);
            }
            /// <summary>
            /// 給值(Double) 
            /// </summary>
            public void SetDouble(object Value)
            {
                SetDouble(Value, 0);
            }

            /// <summary>
            /// 
            /// </summary>
            /// <param name="Value"></param>
            /// <param name="DefValue"></param>
            public void SetDouble(object Value, object DefValue)
            {
                _AsDouble = Convert.ToDouble(DefValue);
                _Parameters.AddWithValue(_ParamName, DbType.Double).Value = DbConvert.ToDouble(Value, _AsDouble);
            }

            /// <summary>
            /// 給值(Double) 
            /// </summary>
            public void SetDecimal(object Value)
            {
                SetDecimal(Value, 0);
            }

            /// <summary>
            /// 
            /// </summary>
            /// <param name="Value"></param>
            /// <param name="DefValue"></param>
            public void SetDecimal(object Value, object DefValue)
            {
                _AsDecimal = Convert.ToDecimal(DefValue);
                _Parameters.AddWithValue(_ParamName, DbType.Decimal).Value = DbConvert.ToDecimal(Value, _AsDecimal);
            }
            /// <summary>
            /// 給值(DateTime)
            /// </summary>
            public void SetDateTime(object Value)
            {
                SetDateTime(Value, "1912/01/01 00:00:00");
            }

            /// <summary>
            /// 
            /// </summary>
            /// <param name="Value"></param>
            /// <param name="DefValue"></param>
            public void SetDateTime(object Value, object DefValue)
            {
                if (Value.Equals(Convert.ToDateTime("0001/01/01")) == true)
                {
                    Value = Convert.ToDateTime("1912/01/01 00:00:00");
                }

                DateTime dtDefValue = Convert.ToDateTime(DefValue);

                _Parameters.AddWithValue(_ParamName, DbType.DateTime).Value = DbConvert.ToDateTime(Value, dtDefValue);
            }

            /// <summary>
            /// 
            /// </summary>
            /// <param name="Value"></param>
            public void SetDateMinTime(object Value)
            {
                DateTime dt = Convert.ToDateTime(Value);

                object oTemp = string.Format("{0} 00:00:00", dt.ToShortDateString());

                SetDateTime(oTemp, "1912/01/01 00:00:00");
            }

            /// <summary>
            /// 
            /// </summary>
            /// <param name="Value"></param>
            public void SetDateMaxTime(object Value)
            {
                DateTime dt = Convert.ToDateTime(Value);

                object oTemp = string.Format("{0} 23:59:59", dt.ToShortDateString());

                SetDateTime(oTemp, "1912/01/01 00:00:00");
            }

            /// <summary>
            /// 
            /// </summary>
            /// <param name="Value"></param>
            /// <param name="DefValue"></param>
            public void SetDateMinTime(object Value, object DefValue)
            {
                DateTime dt = Convert.ToDateTime(Value);

                object oTemp = string.Format("{0} 00:00:00", dt.ToShortDateString());

                SetDateTime(oTemp, DefValue);
            }
            /// <summary>
            /// 
            /// </summary>
            /// <param name="Value"></param>
            /// <param name="DefValue"></param>
            public void SetDateMaxTime(object Value, object DefValue)
            {
                DateTime dt = Convert.ToDateTime(Value);

                object oTemp = string.Format("{0} 23:59:59", dt.ToShortDateString());

                SetDateTime(oTemp, DefValue);
            }

            /// <summary>
            /// 給值(Bit)
            /// </summary>
            public void SetBool(object Value)
            {
                SetBool(Value, false);
            }
            /// <summary>
            /// 
            /// </summary>
            /// <param name="Value"></param>
            /// <param name="DefValue"></param>
            public void SetBool(object Value, object DefValue)
            {
                _AsBool = Convert.ToBoolean(DefValue);
                _Parameters.AddWithValue(_ParamName, DbType.Boolean).Value = DbConvert.ToBoolean(Value, _AsBool);
            }
            /// <summary>
            /// 給值(DBNull)
            /// </summary>
            public void SetDBNull(object Value)
            {
                _Parameters.AddWithValue(_ParamName, DbType.Object).Value = DBNull.Value;
            }
            #endregion
        }
    }
}
