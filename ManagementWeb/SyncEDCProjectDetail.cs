using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Data;

namespace EDC.DBO
{
    public class SyncEDCProjectDetail
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

        public DataTable getData()
        {
            Query.Command.Clear();
            Query.Command.Add(" SELECT StatusType, EDCNO, ProjectNO ");
            Query.Command.Add(" FROM dbo.Sync_EDC_Project ");
            Query.Command.Add(" WHERE SyncFlag = 0 ");

            DataTable dt = Query.ExecuteQuery();
            return dt;
        }

        public bool Insert(string ProjectNO,string StatusType)
        {
            NonQuery.Command.Clear();
            NonQuery.Command.Add(" INSERT dbo.Sync_EDC_Project  ");
            NonQuery.Command.Add(" (EDCNO, ProjectNO, StatusType, SyncFlag, LatestDT) ");
            NonQuery.Command.Add(" SELECT EDCNO, @ProjectNO, @StatusType, 1, GETDATE() ");
            NonQuery.Command.Add(" FROM dbo.DataEDC ");

            NonQuery.ParamByName("ProjectNO").AsString = ProjectNO;
            NonQuery.ParamByName("StatusType").AsString = StatusType;

            return NonQuery.ExecuteNonQuery() > 0;
        }

        public bool Update()
        {
            NonQuery.Command.Clear();
            NonQuery.Command.Add(" UPDATE dbo.Sync_EDC_Project  ");
            NonQuery.Command.Add(" SET SyncFlag = 0 ");
            NonQuery.Command.Add(" WHERE SyncFlag = 1 ");

            return NonQuery.ExecuteNonQuery() > 0;
        }

        public bool Del()
        {
            NonQuery.Command.Clear();
            NonQuery.Command.Add(" DELETE dbo.Sync_EDC_Project  ");
            NonQuery.Command.Add(" WHERE SyncFlag = 0 ");

            return NonQuery.ExecuteNonQuery() > 0;
        }
    }
}