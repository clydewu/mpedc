using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace EDC.DBO
{
    /// <summary>
    /// 型態轉換,強制轉換為預設值
    /// </summary>
    public class DbConvert
    {
        /// <summary>
        /// 將Sender轉換為string型態值,Sender如為Null或DBNull設定為預設值
        /// </summary>
        public static string ToString(object Sender, string DefValue)
        {
            if (Sender == DBNull.Value || Sender == null)
            {
                return DefValue;
            }
            else
            {
                return Sender.ToString().Trim();
            }
        }

        /// <summary>
        /// 將Sender轉換為datetime型態值,Sender如為Null或DBNull設定為預設值
        /// </summary>
        public static DateTime ToDateTime(object Sender, DateTime DefValue)
        {
            try
            {
                if (Sender == DBNull.Value || Sender == null || Convert.ToDateTime(Sender.ToString()) < Convert.ToDateTime("1753/01/01 12:00:00"))
                {
                    return DefValue;
                }
                else
                {
                    return Convert.ToDateTime(Sender);
                }
            }
            catch
            {
                return DefValue;
            }
        }

        /// <summary>
        /// 將Sender轉換為Int16型態值,Sender如為Null或DBNull設定為預設值
        /// </summary>
        public static Int16 ToInt16(object Sender, Int16 DefValue)
        {
            if (Sender == DBNull.Value || Sender == null || Sender.ToString().Trim().Length == 0)
            {
                return DefValue;
            }
            else
            {
                try
                {
                    return Convert.ToInt16(Sender);
                }
                catch
                {
                    return DefValue;
                }
            }
        }

        /// <summary>
        /// 將Sender轉換為Int32型態值,Sender如為Null或DBNull設定為預設值
        /// </summary>
        public static int ToInt32(object Sender, Int32 DefValue)
        {
            if (Sender == DBNull.Value || Sender == null || Sender.ToString().Trim().Length == 0)
            {
                return DefValue;
            }
            else
            {
                try
                {
                    return Convert.ToInt32(Sender);
                }
                catch
                {
                    return DefValue;
                }
            }
        }

        /// <summary>
        /// 將Sender轉換為double型態值,Sender如為Null或DBNull設定為預設值
        /// </summary>
        public static double ToDouble(object Sender, double DefValue)
        {
            if (Sender == DBNull.Value || Sender == null || Sender.ToString().Trim().Length == 0)
            {
                return DefValue;
            }
            else
            {
                try
                {
                    return Convert.ToDouble(Sender);
                }
                catch
                {
                    return DefValue;
                }
            }
        }

        /// <summary>
        /// 將Sender轉換為float型態值,Sender如為Null或DBNull設定為預設值
        /// </summary>
        public static float ToFloat(object Sender, float DefValue)
        {
            try
            {
                if (Sender == DBNull.Value || Sender == null || Sender.ToString().Trim().Length == 0)
                {
                    return DefValue;
                }
                else
                {
                    return Convert.ToSingle(Sender);
                }
            }
            catch
            {
                return DefValue;
            }
        }

        /// <summary>
        /// 將Sender轉換為decimal型態值,Sender如為Null或DBNull設定為預設值
        /// </summary>
        public static decimal ToDecimal(object Sender, decimal DefValue)
        {
            try
            {
                if (Sender == DBNull.Value || Sender == null || Sender.ToString().Trim().Length == 0)
                {
                    return DefValue;
                }
                else
                {
                    return Convert.ToDecimal(Sender);
                }
            }
            catch
            {
                return DefValue;
            }
        }
        /// <summary>
        /// 將Sender轉換為boolean型態值,Sender如為Null或DBNull設定為預設值
        /// </summary>
        public static bool ToBoolean(object Sender, bool DefValue)
        {
            try
            {
                if (Sender == DBNull.Value || Sender == null || Sender.ToString().Trim().Length == 0)
                {
                    return DefValue;
                }
                else
                {
                    return Convert.ToBoolean(Sender);
                }
            }
            catch
            {
                return DefValue;
            }
        }
    }
}
