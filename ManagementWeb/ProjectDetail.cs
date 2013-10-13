using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Data;

namespace EDC.DBO
{
    public class ProjectDetail
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
            Query.Command.Add(" FROM DataProject ");

            DataTable dt = Query.ExecuteQuery();
            return dt;
        }

        public bool Insert(DTO.ProjectDTO dto)
        {
            NonQuery.Command.Clear();
            NonQuery.Command.Add(" INSERT INTO DataProject ");
            NonQuery.Command.Add(" (ProjectID, ProjectName, ProjectNO, ProjectYear, LatestDT) ");
            NonQuery.Command.Add(" VALUES ");
            NonQuery.Command.Add(" (@ProjectID, @ProjectName, @ProjectNO, @ProjectYear, GETDATE()) ");

            NonQuery.ParamByName("ProjectID").AsString = dto.ProjectID;
            NonQuery.ParamByName("ProjectName").AsString = dto.ProjectName;
            NonQuery.ParamByName("ProjectNO").AsString = dto.ProjectNO;
            NonQuery.ParamByName("ProjectYear").AsString = dto.ProjectYear;

            return NonQuery.ExecuteNonQuery() > 0;
        }
        public bool Update(DTO.ProjectDTO dto)
        {
            NonQuery.Command.Clear();
            NonQuery.Command.Add(" UPDATE DataProject ");
            NonQuery.Command.Add(" SET ProjectName = @ProjectName,ProjectYear = @ProjectYear ");
            NonQuery.Command.Add(" ,LatestDT = GETDATE() ");
            NonQuery.Command.Add(" WHERE ProjectID=@ProjectID ");

            NonQuery.ParamByName("ProjectID").AsString = dto.ProjectID;
            NonQuery.ParamByName("ProjectName").AsString = dto.ProjectName;
            NonQuery.ParamByName("ProjectYear").AsString = dto.ProjectYear;

            return NonQuery.ExecuteNonQuery() > 0;
        }

        public bool Del(DTO.ProjectDTO dto)
        {
            NonQuery.Command.Clear();
            NonQuery.Command.Add(" DELETE FROM DataProject ");
            NonQuery.Command.Add(" WHERE ProjectID=@ProjectID ");

            NonQuery.ParamByName("ProjectID").AsString = dto.ProjectID;

            return NonQuery.ExecuteNonQuery() > 0;
        }
    }
}