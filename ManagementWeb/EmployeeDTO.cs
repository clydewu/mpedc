using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace EDC.DTO
{
    public class EmployeeDTO
    {
        public string UserNumber { get; set; }
        public string UserName { get; set; }
        public string UserAccount { get; set; }
        public string Sex { get; set; }
        public string DepartmentNO { get; set; }
        public string CardNumber { get; set; }
        public string IniQuota { get; set; }
        public string NowQuota { get; set; }
        public string HoldQuota { get; set; }
        public string Role { get; set; }
    }
}