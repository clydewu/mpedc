using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Data;

namespace EDC.DBO
{
    public class DepartmentDetail
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
            Query.Command.Add(" FROM DataDepartment ");

            DataTable dt = Query.ExecuteQuery();
            return dt;
        }

        public bool Insert(DTO.DepartmentDTO dto)
        {
            NonQuery.Command.Clear();
            NonQuery.Command.Add(" INSERT INTO DataDepartment ");
            NonQuery.Command.Add(" (DepartmentNO, DepartmentName, DepartmentOrder, LatestDT) ");
            NonQuery.Command.Add(" VALUES ");
            NonQuery.Command.Add(" (@DepartmentNO, @DepartmentName, '1', GETDATE()) ");

            NonQuery.ParamByName("DepartmentNO").AsString = dto.DepartmentNO;
            NonQuery.ParamByName("DepartmentName").AsString = dto.DepartmentName;

            return NonQuery.ExecuteNonQuery() > 0;
        }
        public bool Update(DTO.DepartmentDTO dto)
        {
            NonQuery.Command.Clear();
            NonQuery.Command.Add(" UPDATE DataDepartment ");
            NonQuery.Command.Add(" SET DepartmentNO = @DepartmentNO, DepartmentName = @DepartmentName ");
            NonQuery.Command.Add(" ,LatestDT = GETDATE() ");
            NonQuery.Command.Add(" WHERE SEQ=@SEQ ");

            NonQuery.ParamByName("SEQ").AsString = dto.SEQ;
            NonQuery.ParamByName("DepartmentNO").AsString = dto.DepartmentNO;
            NonQuery.ParamByName("DepartmentName").AsString = dto.DepartmentName;

            return NonQuery.ExecuteNonQuery() > 0;
        }

        public bool Del(DTO.DepartmentDTO dto)
        {
            NonQuery.Command.Clear();
            NonQuery.Command.Add(" DELETE FROM DataDepartment ");
            NonQuery.Command.Add(" WHERE SEQ=@SEQ ");

            NonQuery.ParamByName("SEQ").AsString = dto.SEQ;

            return NonQuery.ExecuteNonQuery() > 0;
        }
    }
}