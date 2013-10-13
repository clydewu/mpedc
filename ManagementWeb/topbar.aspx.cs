using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

namespace EDC
{
    public partial class topbar : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            if (!IsPostBack)
            {
                EDC.Base.LoginUser loginUser = new EDC.Base.LoginUser(Session);

                if (!loginUser.checkSession())
                {
                    Response.Write("<script language='JavaScript' type='text/javascript'>");
                    Response.Write("alert('您沒有登錄或者操作超時，請登錄。');");
                    Response.Write("top.location.href = '../login.aspx';");
                    Response.Write("</script>");
                    Response.End();
                    return;
                }
                else
                {
                    string alertMeg;
                    alertMeg = "<script language='javascript'>";
                    alertMeg += "setUserName('" + loginUser.UsersInfo.UserName + "');";
                    alertMeg += "setUserBranch('" + loginUser.UsersInfo.ExecUnitName + "');";
                    alertMeg += "</script>";
                    ClientScript.RegisterStartupScript(ClientScript.GetType(), "topbar", alertMeg);

                    //    PPMS.DTO.userDTO userData;
                    //    userData = fUserPope.userData;
                    //    if (!userData.isDimission)
                    //    {
                    //        string alertMeg;
                    //        alertMeg = "<script language='javascript'>";
                    //        alertMeg += "setUserName('" + userData.strName + "(" + fUserPope.strUserID + ")');";
                    //        alertMeg += "setUserBranch('" + userData.strBranchName + "(" + userData.strBranchId + ")');";
                    //        alertMeg += "</script>";
                    //        ClientScript.RegisterStartupScript(ClientScript.GetType(), "topbar", alertMeg);
                    //    }
                }
            }
        }
    }
}