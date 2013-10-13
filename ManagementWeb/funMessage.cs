using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI.WebControls;
using System.Web.UI;

namespace EDC.Base
{
    public class funMessage
    {
        public static void messageBox(string strMag, ClientScriptManager clientScriptTmp)
        {
            if (clientScriptTmp == null)
            {
                return;
            }

            if (strMag == null)
            {
                return;
            }

            if (strMag == "")
            {
                return;
            }

            strMag = strMag.Replace("'", "‘");
            strMag = strMag.Replace("\"", "“");


            string alertMeg;
            alertMeg = "<script language='javascript'>alert('";
            alertMeg += strMag;
            alertMeg += "');</script>";
            clientScriptTmp.RegisterStartupScript(clientScriptTmp.GetType(), alertMeg, alertMeg);
        }
        public static void messageBox(string strMag, string strUrl, ClientScriptManager clientScriptTmp)
        {
            if (clientScriptTmp == null)
            {
                return;
            }

            if (strMag == null)
            {
                return;
            }

            if (strMag == "")
            {
                return;
            }

            strMag = strMag.Replace("'", "‘");
            strMag = strMag.Replace("\"", "“");


            string alertMeg;
            alertMeg = "<script language='javascript'>alert('";
            alertMeg += strMag;
            alertMeg += "');";
            alertMeg += "location.replace('";
            alertMeg += strUrl;
            alertMeg += "');";
            alertMeg += "</script>";
            clientScriptTmp.RegisterStartupScript(clientScriptTmp.GetType(), alertMeg, alertMeg);
        }
        //location.replace(OptURL);
        public static void messageBox(Button btnDelete, string strMag)
        {
            string strScript = "Javascript:return confirm('" + strMag + "');";
            btnDelete.Attributes.Add("onclick", strScript);
        }

        public static void messageBox(ImageButton btnDelete, string strMag)
        {
            string strScript = "Javascript:return confirm('" + strMag + "');";
            btnDelete.Attributes.Add("onclick", strScript);
        }
    }
}