using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Data;

namespace EDC.DBO
{
    public class EmployeeDetail
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
            Query.Command.Add(" SELECT *");
            Query.Command.Add(" FROM DataLevel ");

            DataTable dt = Query.ExecuteQuery();
            return dt;
        }

        public bool Insert(DTO.LevelDTO dto)
        {
            NonQuery.Command.Clear();
            NonQuery.Command.Add(" INSERT INTO DataLevel ");
            NonQuery.Command.Add(" (LevelQuota, LevelDef, LatestDT) ");
            NonQuery.Command.Add(" VALUES ");
            NonQuery.Command.Add(" (@LevelQuota, @LevelDef, GETDATE()) ");

            NonQuery.ParamByName("LevelQuota").AsString = dto.LevelQuota;
            NonQuery.ParamByName("LevelDef").AsString = dto.LevelDef;

            return NonQuery.ExecuteNonQuery() > 0;
        }
        public bool Update(DTO.LevelDTO dto)
        {
            NonQuery.Command.Clear();
            NonQuery.Command.Add(" UPDATE DataLevel ");
            NonQuery.Command.Add(" SET LevelQuota = @LevelQuota, LevelDef = @LevelDef ");
            NonQuery.Command.Add(" ,LatestDT = GETDATE() ");
            NonQuery.Command.Add(" WHERE SEQ=@SEQ ");

            NonQuery.ParamByName("SEQ").AsString = dto.SEQ;
            NonQuery.ParamByName("LevelQuota").AsString = dto.LevelQuota;
            NonQuery.ParamByName("LevelDef").AsString = dto.LevelDef;

            return NonQuery.ExecuteNonQuery() > 0;
        }

        public bool Del(DTO.LevelDTO dto)
        {
            NonQuery.Command.Clear();
            NonQuery.Command.Add(" DELETE FROM DataLevel ");
            NonQuery.Command.Add(" WHERE SEQ=@SEQ ");

            NonQuery.ParamByName("SEQ").AsString = dto.SEQ;

            return NonQuery.ExecuteNonQuery() > 0;
        }
    }
}