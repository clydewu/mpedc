using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace EDC.DBO
{
    public class CopyCountDetail
    {
        #region Query commend

        private MsSql _NonQuery;
        private MsSql _Query;
        /// <summary>
        /// NonQuery
        /// </summary>
        public MsSql NonQuery
        {
            get
            {
                if (_NonQuery == null)
                {
                    try
                    {
                        _NonQuery = new MsSql();
                    }
                    catch
                    {
                        _NonQuery = new MsSql();
                    }
                }
                return _NonQuery;
            }
        }
        /// <summary>
        /// Query
        /// </summary>
        public MsSql Query
        {
            get
            {
                if (_Query == null)
                {
                    try
                    {
                        _Query = new MsSql();
                    }
                    catch
                    {
                        _Query = new MsSql();
                    }
                }
                return _Query;
            }
        }
        #endregion

    }
}