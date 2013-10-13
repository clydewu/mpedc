using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Configuration;

namespace EDC.Base
{
    public class TopUser
    {
        private string _USER = "";
        public string User
        {
            get
            {
                return ConfigurationManager.AppSettings["TopId"]; ;
            }
        }

        private string _PWD = "";
        public string Pwd
        {
            get
            {
                return ConfigurationManager.AppSettings["TopPwd"]; ;
            }
        }
    }
}