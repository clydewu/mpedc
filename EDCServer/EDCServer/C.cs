using System;
using System.Collections.Generic;
using System.Text;

namespace EDCServer
{
    class C
    {
        public const string kSyncEmpCmd = "SYNC_EMP";
        public const string kSyncEDCCmd = "SYNC_EDC";
        public const string kSyncProjCmd = "SYNC_PROJ";
        public const string kSyncLogCmd = "SYNC_LOG";
        public const string kSyncEmpOkCmd = "SYNC_EMP_OK";
        public const string kSyncEDCOkCmd = "SYNC_EDC_OK";
        public const string kSyncProjOkCmd = "SYNC_PROJ_OK";

        public const string kFieldEDCID = "@edc_id";
        public const string kFieldEDCLog = "@edc_log";

        public const string kMonoA3 = "1";
        public const string kMonoA4 = "2";
        public const string kColorA3 = "3";
        public const string kColorA4 = "4";

        public const string kIniFile = "EDC_server_setup.ini";
        public const string kKeySection = "EDCServer";
        public const string kKeySource = "DataSource";
        public const string kKeyDB = "Database";
        public const string kKeyUser = "User";
        public const string kKeyPassword = "Password";
        public const string kKeyPort = "Port";
        public const int kSize = 256;

        public const int kDefPort = 3000;
    }

    struct EDCLOG
    {
        public string type;
        public string edc_no;
        public string project_no;
        public string emp_no;
        public string log_time;
        public string content;
    }
}
