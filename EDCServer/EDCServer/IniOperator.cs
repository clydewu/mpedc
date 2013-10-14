using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.IO;

namespace EDCServer
{
    public class IniOperator
    {
        //Decalre API for read INI
        [DllImport("KERNEL32.DLL", EntryPoint = "WritePrivateProfileString", SetLastError = true)]
        public static extern long WritePrivateProfileString(string section, string key, string val, string filePath);
        [DllImport("KERNEL32.DLL", EntryPoint = "GetPrivateProfileString", SetLastError = true)]
        public static extern int GetPrivateProfileString(string section, string key, string def, StringBuilder retVal, int size, string filePath);
    }

    public class Config : IniOperator
    {
        public string source;
        public string db;
        public string user;
        public string password;
        public int port;

        public Config(string file_path)
        {
            int result;
            StringBuilder value = new StringBuilder(C.kSize);

            result = IniOperator.GetPrivateProfileString(C.kKeySection, C.kKeySource, "", value, C.kSize, file_path);
            if (value[value.Length - 1] == '\0' || Marshal.GetLastWin32Error() != 0)
            {
                throw new Exception("Load DataSource from ini file failure.");
            }
            source = value.ToString();
            value.Clear();

            result = IniOperator.GetPrivateProfileString(C.kKeySection, C.kKeyDB, "", value, C.kSize, file_path);
            if (value[value.Length - 1] == '\0' || Marshal.GetLastWin32Error() != 0)
            {
                throw new Exception("Load Database from ini file failure.");
            }
            db = value.ToString();
            value.Clear();

            result = IniOperator.GetPrivateProfileString(C.kKeySection, C.kKeyUser, "", value, C.kSize, file_path);
            if (value[value.Length - 1] == '\0' || Marshal.GetLastWin32Error() != 0)
            {
                throw new Exception("Load User from ini file failure.");
            }
            user = value.ToString();
            value.Clear();

            result = IniOperator.GetPrivateProfileString(C.kKeySection, C.kKeyPassword, "", value, C.kSize, file_path);
            if (value[value.Length - 1] == '\0' || Marshal.GetLastWin32Error() != 0)
            {
                throw new Exception("Load Password from ini file failure.");
            }
            password = value.ToString();
            value.Clear();

            result = IniOperator.GetPrivateProfileString(C.kKeySection, C.kKeyPort, "", value, C.kSize, file_path);
            if (value[value.Length - 1] == '\0' || Marshal.GetLastWin32Error() != 0)
            {
                throw new Exception("Load Port from ini file failure.");
            }
            int.TryParse(value.ToString(), out port);
            port = (port == 0) ? C.kDefPort : port;
        }
    }
}
