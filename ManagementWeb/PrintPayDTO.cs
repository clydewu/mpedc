using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace EDC.DTO
{
    public class PrintPayDTO
    {
        public string EDCNO { get; set; }
        public string PrintType { get; set; }
        public string PrintPay { get; set; }

        private string _SEQ = "";
        public string SEQ
        {
            get { return this._SEQ; }
            set { this._SEQ = value; }
        }
    }
}