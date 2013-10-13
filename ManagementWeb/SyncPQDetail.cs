using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Data;

namespace EDC.DBO
{
    public class SyncPQDetail
    {
        #region Query commend

        private MsSql _NonQuery;
        private MsSql _Query;
        /// <summary>
        /// NonQuery
        /// </summary>
        public MsSql NonQuery
        {
            get
            {
                if (_NonQuery == null)
                {
                    try
                    {
                        _NonQuery = new MsSql();
                    }
                    catch
                    {
                        _NonQuery = new MsSql();
                    }
                }
                return _NonQuery;
            }
        }
        /// <summary>
        /// Query
        /// </summary>
        public MsSql Query
        {
            get
            {
                if (_Query == null)
                {
                    try
                    {
                        _Query = new MsSql();
                    }
                    catch
                    {
                        _Query = new MsSql();
                    }
                }
                return _Query;
            }
        }
        #endregion

        #region Query Sync User
        public DataTable getSyncUsers()
        {
            Query.Command.Clear();
            Query.Command.Add(" SELECT A.StatusType, B.UserNumber, B.UserName, B.DepartmentNO, B.CardNumber");
            Query.Command.Add(" FROM dbo.Sync_PQ_Employee AS A INNER JOIN dbo.DataEmployee AS B ON A.UserNumber = B.UserNumber ");
            Query.Command.Add(" WHERE SyncFlag = 0 ");

            DataTable dt = Query.ExecuteQuery();
            return dt;
        }

        public bool updateSyncUsers()
        {
            NonQuery.Command.Clear();
            NonQuery.Command.Add(" UPDATE dbo.Sync_PQ_Employee  ");
            NonQuery.Command.Add(" SET SyncFlag = 0 ");
            NonQuery.Command.Add(" WHERE SyncFlag = 1 ");

            return NonQuery.ExecuteNonQuery() > 0;
        }

        public bool delSyncUsers()
        {
            NonQuery.Command.Clear();
            NonQuery.Command.Add(" DELETE dbo.Sync_PQ_Employee  ");
            NonQuery.Command.Add(" WHERE SyncFlag = 0 ");

            return NonQuery.ExecuteNonQuery() > 0;
        }
        #endregion

        #region Query Sync Machine
        public DataTable getSyncMachine()
        {
            Query.Command.Clear();
            Query.Command.Add(" SELECT StatusType, MachineNO, MachineIP ");
            Query.Command.Add(" FROM dbo.Sync_PQ_EDC ");
            Query.Command.Add(" WHERE SyncFlag = 0 ");

            DataTable dt = Query.ExecuteQuery();
            return dt;
        }

        public bool updateSyncMachine()
        {
            NonQuery.Command.Clear();
            NonQuery.Command.Add(" UPDATE dbo.Sync_PQ_EDC  ");
            NonQuery.Command.Add(" SET SyncFlag = 0 ");
            NonQuery.Command.Add(" WHERE SyncFlag = 1 ");

            return NonQuery.ExecuteNonQuery() > 0;
        }

        public bool delSyncMachine()
        {
            NonQuery.Command.Clear();
            NonQuery.Command.Add(" DELETE dbo.Sync_PQ_EDC  ");
            NonQuery.Command.Add(" WHERE SyncFlag = 0 ");

            return NonQuery.ExecuteNonQuery() > 0;
        }
        #endregion
        
        #region Query Check EDC Status
        public DataTable getEDCStatus()
        {
            Query.Command.Clear();
            Query.Command.Add(" SELECT * ");
            Query.Command.Add(" FROM dbo.EDCStatus ");

            DataTable dt = Query.ExecuteQuery();
            return dt;
        }

        public DataTable getEDCStatus(string MachineNO)
        {
            Query.Command.Clear();
            Query.Command.Add(" SELECT * ");
            Query.Command.Add(" FROM dbo.EDCStatus ");
            Query.Command.Add(" WHERE MachineNO = @MachineNO ");

            Query.ParamByName("MachineNO").AsString = MachineNO;

            DataTable dt = Query.ExecuteQuery();
            return dt;
        }
        #endregion

        #region Query Insert PQLog
        public bool InsertPQLog(string PQLog)
        {
            NonQuery.Command.Clear();
            NonQuery.Command.Add(" INSERT INTO PQLog ");
            NonQuery.Command.Add(" (PQLog, LatestDT) ");
            NonQuery.Command.Add(" VALUES ");
            NonQuery.Command.Add(" (@PQLog,GETDATE()) ");

            NonQuery.ParamByName("PQLog").AsString = PQLog;

            return NonQuery.ExecuteNonQuery() > 0;
        }
        #endregion
    }
}