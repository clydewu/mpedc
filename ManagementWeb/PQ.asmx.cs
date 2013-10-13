using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Services;
using System.Data;

namespace EDC.WS
{
    /// <summary>
    /// PQ 的摘要描述
    /// </summary>
    [WebService(Namespace = "http://tempuri.org/")]
    [WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
    [System.ComponentModel.ToolboxItem(false)]
    // 若要允許使用 ASP.NET AJAX 從指令碼呼叫此 Web 服務，請取消註解下一行。
    // [System.Web.Script.Services.ScriptService]
    public class PQ : System.Web.Services.WebService
    {
        DBO.SyncPQDetail dbo = new DBO.SyncPQDetail();

        [WebMethod]
        public DataTable SyncUser()
        {
            dbo.updateSyncUsers();
            
            DataTable dt = dbo.getSyncUsers();
            return dt;
        }

        [WebMethod]
        public void SyncUserSuccess()
        {
            dbo.delSyncUsers();
        }

        [WebMethod]
        public DataTable SyncMachine()
        {
            dbo.updateSyncMachine();

            DataTable dt = dbo.getSyncMachine();
            return dt;
        }

        [WebMethod]
        public void SyncMachineSuccess()
        {
            dbo.delSyncMachine();
        }

        [WebMethod]
        public DataTable CheckEDC()
        {
            DataTable dt = dbo.getEDCStatus();
            return dt;
        }
        [WebMethod]
        public DataTable CheckEDC(string MachineNO)
        {
            DataTable dt = dbo.getEDCStatus(MachineNO);
            return dt;
        }
        [WebMethod]
        public bool InsertPQLog(string PQLog)
        {
            return dbo.InsertPQLog(PQLog);
        }
        
    }
}
