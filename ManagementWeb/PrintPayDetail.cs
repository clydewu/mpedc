using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Data;

namespace EDC.DBO
{
    public class PrintPayDetail
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

        public DataTable getData()
        {
            Query.Command.Clear();
            Query.Command.Add(" SELECT *");
            Query.Command.Add(" FROM DataPrintPay ");

            DataTable dt = Query.ExecuteQuery();
            return dt;
        }

        public bool Insert(DTO.PrintPayDTO dto)
        {
            NonQuery.Command.Clear();
            NonQuery.Command.Add(" INSERT INTO DataPrintPay ");
            NonQuery.Command.Add(" (EDCNO, PrintType, PrintPay, LatestDT) ");
            NonQuery.Command.Add(" VALUES ");
            NonQuery.Command.Add(" (@EDCNO, @PrintType, @PrintPay, GETDATE()) ");

            NonQuery.ParamByName("EDCNO").AsString = dto.EDCNO;
            NonQuery.ParamByName("PrintType").AsString = dto.PrintType;
            NonQuery.ParamByName("PrintPay").AsString = dto.PrintPay;

            return NonQuery.ExecuteNonQuery() > 0;
        }
        public bool Update(DTO.PrintPayDTO dto)
        {
            NonQuery.Command.Clear();
            NonQuery.Command.Add(" UPDATE DataPrintPay ");
            NonQuery.Command.Add(" SET EDCNO = @EDCNO, PrintType = @PrintType, PrintPay = @PrintPay ");
            NonQuery.Command.Add(" ,LatestDT = GETDATE() ");
            NonQuery.Command.Add(" WHERE SEQ=@SEQ ");

            NonQuery.ParamByName("SEQ").AsString = dto.SEQ;
            NonQuery.ParamByName("EDCNO").AsString = dto.EDCNO;
            NonQuery.ParamByName("PrintType").AsString = dto.PrintType;
            NonQuery.ParamByName("PrintPay").AsString = dto.PrintPay;

            return NonQuery.ExecuteNonQuery() > 0;
        }

        public bool Del(DTO.PrintPayDTO dto)
        {
            NonQuery.Command.Clear();
            NonQuery.Command.Add(" DELETE FROM DataPrintPay ");
            NonQuery.Command.Add(" WHERE SEQ=@SEQ ");

            NonQuery.ParamByName("SEQ").AsString = dto.SEQ;

            return NonQuery.ExecuteNonQuery() > 0;
        }
    }
}