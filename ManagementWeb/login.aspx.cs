using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.Security;
using System.Data;
using EDC.Base;

namespace EDC
{
    public partial class login : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            if (!IsPostBack)
            {
                Session.Abandon();
                FormsAuthentication.SignOut();
            }
        }
        protected void LogUser_Authenticate(object sender, AuthenticateEventArgs e)
        {
            int iNum;
            string strName = LogUser.UserName;

            if (Session[strName] == null)
            {
                iNum = 1;
                Session[strName] = iNum.ToString();
            }
            else
            {
                try
                {
                    iNum = Convert.ToInt16(Session[strName].ToString());
                }
                catch
                {
                    iNum = 0;
                }
                if (iNum != 10)
                    iNum += 1;
                Session[strName] = iNum.ToString();
            }

            if (iNum > 3)
            {
                if (iNum == 4)
                {
                    iNum = 10;
                    Session[strName] = iNum.ToString();
                }

                e.Authenticated = false;
                LogUser.FailureText = "使用者代碼輸入密碼出錯3次，不再進行密碼驗證。";
                return;
            }

            Boolean blnLogin = false;
            TopUser topUser = new TopUser();

            if (LogUser.UserName.Trim() == topUser.User)
            {
                if (!LogUser.Password.Trim().Equals(topUser.Pwd))
                    blnLogin = true;
            }
            else
            {
                DataTable dt = new DBO.UsersDetail().CheckLogin(LogUser.UserName.Trim(), LogUser.Password.Trim());

                if (dt != null)
                {
                    if (dt.Rows.Count == 0)
                    {
                        blnLogin = true;
                    }
                }
                else
                {
                    blnLogin = true;
                }
            }

            if (blnLogin)
            {
                LogUser.FailureText = "你輸入的使用者代碼或密碼錯誤，請再試一次。";
                e.Authenticated = false;
                return;
            }
            e.Authenticated = true;

            if (e.Authenticated)
            {
                Session.RemoveAll();
                Session["LoginUserNo"] = strName;
            }
        }
    }
}