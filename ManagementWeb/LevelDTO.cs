using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace EDC.DTO
{
    public class LevelDTO
    {
        public string LevelQuota { get; set; }
        public string LevelDef { get; set; }

        private string _SEQ = "";
        public string SEQ
        {
            get { return this._SEQ; }
            set { this._SEQ = value; }
        }
    }
}