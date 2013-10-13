using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace EDC.DTO
{
    public class UsersDTO
    {
        public string UserNo { get; set; }
        public string UserPwd { get; set; }
        public string UserName { get; set; }
        public string PlanUnitNo { get; set; }
        public string PlanUnitName { get; set; }
        public string ExecUnitNo { get; set; }
        public string ExecUnitName { get; set; }
        public string UnitName { get; set; }
        public string Tel { get; set; }
        public string Fax { get; set; }
        public string Email { get; set; }
        public string Role { get; set; }

        #region member
        private string _SEQ = "";
        private string _Memo = "";
        private string _UpdUserNo = "";
        #endregion
        #region Properties
        /// <summary>
        /// 序號
        /// </summary>
        public string Seq
        {
            get { return this._SEQ; }
            set { this._SEQ = value; }
        }
        /// <summary>
        /// 備註
        /// </summary>
        public string Memo
        {
            get { return this._Memo; }
            set { this._Memo = value; }
        }
        /// <summary>
        /// 編輯人員
        /// </summary>
        public string UpdUserNo
        {
            get { return this._UpdUserNo; }
            set { this._UpdUserNo = value; }
        }
        #endregion
    }
}
