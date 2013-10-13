using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace EDC.DTO
{
    public class EDCDTO
    {
        public string EDCNO { get; set; }
        public string EDCIP { get; set; }
        public string EDCState { get; set; }
        public string EDCLimitTime { get; set; }
        public string EDCShowLimitTime { get; set; }
        public string MachineNO { get; set; }
        public string MachineIP { get; set; }
        public string MachineOnline { get; set; }

        private string _SEQ = "";
        public string SEQ
        {
            get { return this._SEQ; }
            set { this._SEQ = value; }
        }
    }
}