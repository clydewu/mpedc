using System;
using System.Data;

namespace EDC.DBO
{
    public class UsersDetail
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

        public DataTable getUsers(string userNo)
        {
            Query.Command.Clear();
            Query.Command.Add(" SELECT *");
            Query.Command.Add(" FROM USERS ");
            Query.Command.Add(" WHERE USER_NO=@USER_NO ");

            Query.ParamByName("USER_NO").AsString = userNo;

            DataTable dt = Query.ExecuteQuery();
            return dt;
        }
        public DataTable CheckLogin(string userNo, string userPwd)
        {
            Query.Command.Clear();
            Query.Command.Add(" SELECT * ");
            Query.Command.Add(" FROM USERS ");
            Query.Command.Add(" WHERE USER_NO=@USER_NO AND USER_PWD=@USER_PWD ");

            Query.ParamByName("USER_NO").AsString = userNo;
            Query.ParamByName("USER_PWD").AsString = userPwd;

            DataTable dt = Query.ExecuteQuery();
            return dt;
        }

        public DataTable getUsersSearch(string planUnit, string execUnit)
        {
            string strSQL = "";
            strSQL += " SELECT * FROM USERS WHERE 1=1 ";

            if (!planUnit.Equals(""))
                strSQL += " AND PLAN_UNIT_NO=@PLAN_UNIT_NO ";

            if (!execUnit.Equals(""))
                strSQL += " AND EXEC_UNIT_NO=@EXEC_UNIT_NO ";

            Query.Command.Clear();
            Query.Command.Add(strSQL);

            if (!planUnit.Equals(""))
                Query.ParamByName("PLAN_UNIT_NO").AsString = planUnit;

            if (!execUnit.Equals(""))
                Query.ParamByName("EXEC_UNIT_NO").AsString = execUnit;

            DataTable dt = Query.ExecuteQuery();
            return dt;
        }

        public bool Insert(DTO.UsersDTO dto)
        {
            NonQuery.Command.Clear();
            NonQuery.Command.Add(" INSERT INTO USERS ");
            NonQuery.Command.Add(" (USER_NO,USER_PWD,USER_NAME,PLAN_UNIT_NO,PLAN_UNIT_NAME ");
            NonQuery.Command.Add(" ,EXEC_UNIT_NO,EXEC_UNIT_NAME,UNIT_NAME ");
            NonQuery.Command.Add(" ,TEL,FAX,EMAIL,ROLE ");
            NonQuery.Command.Add(" ,MEMO,UPD_USER_NO,UPD_DT) ");
            NonQuery.Command.Add(" VALUES ");
            NonQuery.Command.Add(" (@USER_NO,@USER_PWD,@USER_NAME,@PLAN_UNIT_NO,@PLAN_UNIT_NAME ");
            NonQuery.Command.Add(" ,@EXEC_UNIT_NO,@EXEC_UNIT_NAME,@UNIT_NAME ");
            NonQuery.Command.Add(" ,@TEL,@FAX,@EMAIL,@ROLE ");
            NonQuery.Command.Add(" ,@MEMO,@UPD_USER_NO,GETDATE()) ");

            NonQuery.ParamByName("USER_NO").AsString = dto.UserNo;
            NonQuery.ParamByName("USER_PWD").AsString = dto.UserPwd;
            NonQuery.ParamByName("USER_NAME").AsString = dto.UserName;
            NonQuery.ParamByName("PLAN_UNIT_NO").AsString = dto.PlanUnitNo;
            NonQuery.ParamByName("PLAN_UNIT_NAME").AsString = dto.PlanUnitName;
            NonQuery.ParamByName("EXEC_UNIT_NO").AsString = dto.ExecUnitNo;
            NonQuery.ParamByName("EXEC_UNIT_NAME").AsString = dto.ExecUnitName;
            NonQuery.ParamByName("UNIT_NAME").AsString = dto.UnitName;
            NonQuery.ParamByName("TEL").AsString = dto.Tel;
            NonQuery.ParamByName("FAX").AsString = dto.Fax;
            NonQuery.ParamByName("EMAIL").AsString = dto.Email;
            NonQuery.ParamByName("ROLE").AsString = dto.Role;
            NonQuery.ParamByName("MEMO").AsString = dto.Memo;
            NonQuery.ParamByName("UPD_USER_NO").AsString = dto.UpdUserNo;

            return NonQuery.ExecuteNonQuery() > 0;
        }
        public bool Update(DTO.UsersDTO dto)
        {
            NonQuery.Command.Clear();
            NonQuery.Command.Add(" UPDATE USERS ");
            NonQuery.Command.Add(" SET USER_PWD = @USER_PWD,USER_NAME = @USER_NAME ");
            NonQuery.Command.Add(" ,PLAN_UNIT_NO = @PLAN_UNIT_NO,PLAN_UNIT_NAME = @PLAN_UNIT_NAME ");
            NonQuery.Command.Add(" ,EXEC_UNIT_NO = @EXEC_UNIT_NO,EXEC_UNIT_NAME = @EXEC_UNIT_NAME ");
            NonQuery.Command.Add(" ,UNIT_NAME = @UNIT_NAME ");
            NonQuery.Command.Add(" ,TEL = @TEL,FAX = @FAX,EMAIL = @EMAIL,ROLE = @ROLE ");
            NonQuery.Command.Add(" ,MEMO = @MEMO,UPD_USER_NO = @UPD_USER_NO,UPD_DT = GETDATE() ");
            NonQuery.Command.Add(" WHERE USER_NO=@USER_NO ");

            NonQuery.ParamByName("USER_NO").AsString = dto.UserNo;
            NonQuery.ParamByName("USER_PWD").AsString = dto.UserPwd;
            NonQuery.ParamByName("USER_NAME").AsString = dto.UserName;
            NonQuery.ParamByName("PLAN_UNIT_NO").AsString = dto.PlanUnitNo;
            NonQuery.ParamByName("PLAN_UNIT_NAME").AsString = dto.PlanUnitName;
            NonQuery.ParamByName("EXEC_UNIT_NO").AsString = dto.ExecUnitNo;
            NonQuery.ParamByName("EXEC_UNIT_NAME").AsString = dto.ExecUnitName;
            NonQuery.ParamByName("UNIT_NAME").AsString = dto.UnitName;
            NonQuery.ParamByName("TEL").AsString = dto.Tel;
            NonQuery.ParamByName("FAX").AsString = dto.Fax;
            NonQuery.ParamByName("EMAIL").AsString = dto.Email;
            NonQuery.ParamByName("ROLE").AsString = dto.Role;
            NonQuery.ParamByName("MEMO").AsString = dto.Memo;
            NonQuery.ParamByName("UPD_USER_NO").AsString = dto.UpdUserNo;

            return NonQuery.ExecuteNonQuery() > 0;
        }

        public bool Del(DTO.UsersDTO dto)
        {
            NonQuery.Command.Clear();
            NonQuery.Command.Add(" DELETE FROM USERS ");
            NonQuery.Command.Add(" WHERE USER_NO=@USER_NO ");

            NonQuery.ParamByName("USER_NO").AsString = dto.UserNo;

            return NonQuery.ExecuteNonQuery() > 0;
        }
    }
}
