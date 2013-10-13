using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace EDC.DTO
{
    public class DepartmentDTO
    {
        public string DepartmentNO { get; set; }
        public string DepartmentName { get; set; }
        public string DepartmentOrder { get; set; }

        private string _SEQ = "";
        public string SEQ
        {
            get { return this._SEQ; }
            set { this._SEQ = value; }
        }
    }
}