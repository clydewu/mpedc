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
        public const string kSyncHeartBeat = "HEARTBEAT";

        public const string kFieldEDCID = "@edc_id";
        public const string kFieldEDCLog = "@edc_log";

        public const string kMonoA3 = "1";
        public const string kMonoA4 = "2";
        public const string kColorA3 = "3";
        public const string kColorA4 = "4";
    }
}
