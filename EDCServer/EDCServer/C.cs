using System;
using System.Collections.Generic;
using System.Text;

namespace EDCServer
{
    class C
    {
        public const string kSyncVerCmd = "SYNC_VER";
        public const string kSyncEmpCmd = "SYNC_EMP";
        public const string kSyncEDCCmd = "SYNC_EDC";
        public const string kSyncProjCmd = "SYNC_PROJ";
        public const string kSyncLogCmd = "SYNC_LOG";
        public const string kSyncEmpDeltaCmd = "SYNC_EMP_DELTA";
        public const string kSyncEDCDeltaCmd = "SYNC_EDC_DELTA";
        public const string kSyncProjDeltaCmd = "SYNC_PROJ_DELTA";
        public const string kSyncEmpDeltaOkCmd = "SYNC_EMP_DELTA_OK";
        public const string kSyncEDCDeltaOkCmd = "SYNC_EDC_DELTA_OK";
        public const string kSyncProjDeltaOkCmd = "SYNC_PROJ_DELTA_OK";

        public const string kFieldEDCID = "@edc_id";
        public const string kFieldEDCVER = "@edc_ver";
        public const string kFieldEDCLog = "@edc_log";

        public const string kGrayBig = "GB";
        public const string kGraySmall = "GS";
        public const string kGrayOther = "GO";
        public const string kColorBig = "CB";
        public const string kColorSmall = "CS";
        public const string kColorOther = "CO";

        public const string kIniFile = "EDC_server_setup.ini";
        public const string kKeySection = "EDCServer";
        public const string kKeySource = "DataSource";
        public const string kKeyDB = "Database";
        public const string kKeyUser = "User";
        public const string kKeyPassword = "Password";
        public const string kKeyPort = "Port";
        public const int kSize = 256;

        public const int kDefPort = 3000;
        public const int kBufSize = 256;
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
