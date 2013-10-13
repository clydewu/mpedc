using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using EDC.DTO;
using System.Data;

namespace EDC.Base
{
    public class LoginUser
    {
        private string _UserNo = "";
        private System.Web.SessionState.HttpSessionState _SessionTmp;

        public DTO.UsersDTO UsersInfo
        {
            get
            {
                DTO.UsersDTO UsersToDTO = new UsersDTO();
                TopUser topUser = new TopUser();

                DataTable dt;

                if (UserNo == topUser.User)
                {
                    if (UserNo == topUser.User)
                    {
                        UsersToDTO.UserNo = UserNo;
                        UsersToDTO.UserName = "系統管理者";
                        UsersToDTO.PlanUnitNo = "";
                        UsersToDTO.PlanUnitName = "";
                        UsersToDTO.ExecUnitNo = "";
                        UsersToDTO.ExecUnitName = "";
                        UsersToDTO.UnitName = "";
                        UsersToDTO.Tel = "";
                        UsersToDTO.Fax = "";
                        UsersToDTO.Email = "";
                        UsersToDTO.Role = "1";
                    }
                }
                else
                {
                    if (_SessionTmp["UserInfo"] == null)
                    {
                        dt = new DBO.UsersDetail().getUsers(UserNo);
                        if (dt != null)
                            _SessionTmp["UserInfo"] = dt;
                    }
                    else
                    {
                        dt = (DataTable)_SessionTmp["UserInfo"];
                    }

                    if (dt != null)
                    {
                        foreach (DataRow dr in dt.Rows)
                        {
                            UsersToDTO.UserNo = dr["USER_NAME"].ToString();
                            UsersToDTO.UserName = dr["USER_NAME"].ToString();
                            UsersToDTO.PlanUnitNo = dr["PLAN_UNIT_NO"].ToString();
                            UsersToDTO.PlanUnitName = dr["PLAN_UNIT_NAME"].ToString();
                            UsersToDTO.ExecUnitNo = dr["EXEC_UNIT_NO"].ToString();
                            UsersToDTO.ExecUnitName = dr["EXEC_UNIT_NAME"].ToString();
                            UsersToDTO.UnitName = dr["UNIT_NAME"].ToString();
                            UsersToDTO.Tel = dr["TEL"].ToString();
                            UsersToDTO.Fax = dr["FAX"].ToString();
                            UsersToDTO.Email = dr["EMAIL"].ToString();
                            UsersToDTO.Role = dr["ROLE"].ToString();
                        }
                    }
                }


                return UsersToDTO;
            }
        }

        public LoginUser(System.Web.SessionState.HttpSessionState SessionTmp)
        {
            _SessionTmp = SessionTmp;
            getUser(SessionTmp);
        }

        public bool checkSession()
        {
            if (_SessionTmp["LoginUserNo"] == null)
                return false;
            return true;
        }

        private bool getUser(System.Web.SessionState.HttpSessionState SessionTmp)
        {
            string sUserNoTmp;

            if (SessionTmp["LoginUserNo"] == null) return false;

            sUserNoTmp = SessionTmp["LoginUserNo"].ToString();
            if (sUserNoTmp != UserNo)
                _UserNo = sUserNoTmp;

            return true;
        }

        public string UserNo
        {
            get
            {
                return _UserNo;
            }
        }
    }
}