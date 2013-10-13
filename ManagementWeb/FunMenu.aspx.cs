using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Data;
using EDC.Base;
using EDC.DBO;
using EDC.DTO;
using System.Data.SqlClient;
using System.Configuration;

namespace EDC
{
    public partial class FunMenu : System.Web.UI.Page
    {
        public string sHtml = "";
        private string sUserNo { get; set; }

        //#region private

        //private void Bind()
        //{
        //    string strHtml = "";
        //    string strF = "";

        //    strF = "</table></div></td></tr>";

        //    TopUser topUser = new TopUser();
        //    DataTable dt;
        //    if (sUserNo == topUser.User)
        //        dt = new DBO.FuncDetail().getData();
        //    else
        //        dt = new DBO.RelUsersFuncDetail().getDataByLoginUserNo(sUserNo);

        //    int i = 0;
        //    foreach (DataRow dr in dt.Rows)
        //    {
        //        i += 1;
        //        if (dr["FUNC_NO"].ToString().Trim().Length == 1)
        //        {
        //            if (strHtml != "")
        //            {
        //                strHtml += strF;
        //            }
        //        }
        //        strHtml += getHtml(dr["FUNC_NO"].ToString().Trim());
        //        if (dt.Rows.Count == i)
        //            strHtml += strF;
        //    }
        //    //divHtml.InnerHtml = strHtml;
        //    sHtml = strHtml;
        //}

        //private string getHtml(string strFuncNo)
        //{
        //    string strHtml = "";
        //    if (strFuncNo.Equals("A"))
        //    {
        //        strHtml = "";
        //        strHtml += "<tr id=\"row1\" class=\"title\" title=\"計畫管理\" style=\"cursor: pointer\" onclick=\"CheckDisplay('div1','row1','image2');\"> ";
        //        strHtml += "<td align=\"left\" valign=\"top\"> ";
        //        strHtml += "<a href=\"#\" onmouseout=\"MM_swapImgRestoreA('div1')\" onmouseover=\"MM_swapImage('image2','','../images/a2.jpg',1)\"> ";
        //        strHtml += "<img src=\"../images/a1.jpg\" name=\"image2\" width=\"185\" height=\"58\" border=\"0\" id=\"image2\"></a> ";
        //        strHtml += "</td>";
        //        strHtml += "</tr>";
        //        strHtml += "<tr>";
        //        strHtml += "<td>";
        //        strHtml += "<div id=\"div1\" style=\"display: none\" runat=\"server\">";
        //        strHtml += "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">";
        //    }
        //    else if (strFuncNo.Equals("A01"))
        //    {
        //        strHtml = "";
        //        strHtml += "<tr onclick=\"GoPage('../Plan/PlanItemA.aspx' , '0');\" style=\"cursor: hand\"> ";
        //        strHtml += "<td align=\"left\" valign=\"middle\" width=\"199\" height=\"30\" background=\"images/P2_EMPTY_UP.gif\"> ";
        //        strHtml += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font ";
        //        strHtml += "style=\"font-size: 10pt; cursor: hand; color: blue; text-decoration: none\" onclick=\"return Click_Color(this)\">計畫項目</font> ";
        //        strHtml += "</td> ";
        //        strHtml += "</tr>";
        //    }
        //    else if (strFuncNo.Equals("A02"))
        //    {
        //        strHtml = "";
        //        strHtml += "<tr onclick=\"GoPage('../Plan/PlanItemA_CheckPoint.aspx' , '0');\" style=\"cursor: hand\"> ";
        //        strHtml += "<td align=\"left\" valign=\"middle\" width=\"199\" height=\"30\" background=\"images/P2_EMPTY_UP.gif\"> ";
        //        strHtml += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font ";
        //        strHtml += "style=\"font-size: 10pt; cursor: hand; color: blue; text-decoration: none\" onclick=\"return Click_Color(this)\">計畫項目里程碑</font> ";
        //        strHtml += "</td> ";
        //        strHtml += "</tr>";
        //    }
        //    else if (strFuncNo.Equals("A03"))
        //    {
        //        strHtml = "";
        //        strHtml += "<tr onclick=\"GoPage('../Plan/PlanItem_CPSch.aspx' , '0');\" style=\"cursor: hand\"> ";
        //        strHtml += "<td align=\"left\" valign=\"middle\" width=\"199\" height=\"30\" background=\"images/P2_EMPTY_UP.gif\"> ";
        //        strHtml += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font ";
        //        strHtml += "style=\"font-size: 10pt; cursor: hand; color: blue; text-decoration: none\" onclick=\"return Click_Color(this)\">計畫里程碑進度填報</font> ";
        //        strHtml += "</td> ";
        //        strHtml += "</tr>";
        //    }
        //    else if (strFuncNo.Equals("B"))
        //    {
        //        strHtml = "";
        //        strHtml += "<tr id=\"row2\" class=\"title\" title=\"工程案件管理\" style=\"cursor: pointer\" onclick=\"CheckDisplay('div2','row2','image3');\"> ";
        //        strHtml += "<td align=\"left\" valign=\"top\"> ";
        //        strHtml += "<a href=\"#\" onmouseout=\"MM_swapImgRestoreA('div2')\" onmouseover=\"MM_swapImage('image3','','../images/b2.jpg',1)\"> ";
        //        strHtml += "<img src=\"../images/b1.jpg\" name=\"image3\" width=\"185\" height=\"58\" border=\"0\" id=\"image3\"></a> ";
        //        strHtml += "</td> ";
        //        strHtml += "</tr> ";
        //        strHtml += "<tr> ";
        //        strHtml += "<td> ";
        //        strHtml += "<div id=\"div2\" style=\"display: none\" runat=\"server\"> ";
        //        strHtml += "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">";
        //    }
        //    else if (strFuncNo.Equals("B01"))
        //    {
        //        strHtml = "";
        //        strHtml += "<tr onclick=\"GoPage('../Proj/ProjEngSearch.aspx' , '0');\" style=\"cursor: hand\"> ";
        //        strHtml += "<td align=\"left\" valign=\"middle\" width=\"199\" height=\"30\" background=\"images/P2_EMPTY_UP.gif\"> ";
        //        strHtml += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font ";
        //        strHtml += "style=\"font-size: 10pt; cursor: hand; color: blue; text-decoration: none\" onclick=\"return Click_Color(this)\">工程案件</font> ";
        //        strHtml += "</td> ";
        //        strHtml += "</tr>";
        //    }
        //    else if (strFuncNo.Equals("B02"))
        //    {
        //        strHtml = "";
        //        strHtml += "<tr onclick=\"GoPage('../Proj/ProjEngBudgetSearch.aspx' , '0');\" style=\"cursor: hand\"> ";
        //        strHtml += "<td align=\"left\" valign=\"middle\" width=\"199\" height=\"30\" background=\"images/P2_EMPTY_UP.gif\"> ";
        //        strHtml += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font ";
        //        strHtml += "style=\"font-size: 10pt; cursor: hand; color: blue; text-decoration: none\" onclick=\"return Click_Color(this)\">工程執行經費登錄</font> ";
        //        strHtml += "</td> ";
        //        strHtml += "</tr>";
        //    }
        //    else if (strFuncNo.Equals("C"))
        //    {
        //        strHtml = "";
        //        strHtml += "<tr id=\"row3\" class=\"title\" title=\"委辦案件管理\" style=\"cursor: pointer\" onclick=\"CheckDisplay('div3','row3','image4');\"> ";
        //        strHtml += "<td align=\"left\" valign=\"top\"> ";
        //        strHtml += "<a href=\"#\" onmouseout=\"MM_swapImgRestoreA('div3')\" onmouseover=\"MM_swapImage('image4','','../images/c2.jpg',1)\"> ";
        //        strHtml += "<img src=\"../images/c1.jpg\" name=\"image4\" width=\"185\" height=\"58\" border=\"0\" id=\"image4\"></a> ";
        //        strHtml += "</td> ";
        //        strHtml += "</tr> ";
        //        strHtml += "<tr> ";
        //        strHtml += "<td> ";
        //        strHtml += "<div id=\"div3\" style=\"display: none\" runat=\"server\"> ";
        //        strHtml += "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">";
        //    }
        //    else if (strFuncNo.Equals("C01"))
        //    {
        //        strHtml = "";
        //        strHtml += "<tr onclick=\"GoPage('../Proj/ProjEntSearch.aspx' , '0');\" style=\"cursor: hand\"> ";
        //        strHtml += "<td align=\"left\" valign=\"middle\" width=\"199\" height=\"30\" background=\"images/P2_EMPTY_UP.gif\"> ";
        //        strHtml += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font ";
        //        strHtml += "style=\"font-size: 10pt; cursor: hand; color: blue; text-decoration: none\" onclick=\"return Click_Color(this)\">委辦案件</font> ";
        //        strHtml += "</td> ";
        //        strHtml += "</tr>";
        //    }
        //    else if (strFuncNo.Equals("C02"))
        //    {
        //        strHtml = "";
        //        strHtml += "<tr onclick=\"GoPage('../Proj/ProjEntBudgetSearch.aspx' , '0');\" style=\"cursor: hand\"> ";
        //        strHtml += "<td align=\"left\" valign=\"middle\" width=\"199\" height=\"30\" background=\"images/P2_EMPTY_UP.gif\"> ";
        //        strHtml += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font ";
        //        strHtml += "style=\"font-size: 10pt; cursor: hand; color: blue; text-decoration: none\" onclick=\"return Click_Color(this)\">委辦執行經費登錄</font> ";
        //        strHtml += "</td> ";
        //        strHtml += "</tr>";
        //    }
        //    else if (strFuncNo.Equals("D"))
        //    {
        //        strHtml = "";
        //        strHtml += "<tr id=\"row5\" class=\"title\" title=\"報表列印\" style=\"cursor: pointer\" onclick=\"CheckDisplay('div5','row5','image6');\"> ";
        //        strHtml += "<td align=\"left\" valign=\"top\"> ";
        //        strHtml += "<a href=\"#\" onmouseout=\"MM_swapImgRestoreA('div5')\" onmouseover=\"MM_swapImage('image6','','../images/d2.jpg',1)\"> ";
        //        strHtml += "<img src=\"../images/d1.jpg\" name=\"image6\" width=\"185\" height=\"58\" border=\"0\" id=\"image6\"></a> ";
        //        strHtml += "</td> ";
        //        strHtml += "</tr> ";
        //        strHtml += "<tr> ";
        //        strHtml += "<td> ";
        //        strHtml += "<div id=\"div5\" style=\"display: none\" runat=\"server\"> ";
        //        strHtml += "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">";
        //    }
        //    else if (strFuncNo.Equals("D01"))
        //    {
        //        strHtml = "";
        //        strHtml += "<tr onclick=\"GoPage('../Rpt/Rpt01.aspx' , '0');\" style=\"cursor: hand\"> ";
        //        strHtml += "<td align=\"left\" valign=\"middle\" width=\"199\" height=\"30\" background=\"images/P2_EMPTY_UP.gif\"> ";
        //        strHtml += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font ";
        //        strHtml += "style=\"font-size: 10pt; cursor: hand; color: blue; text-decoration: none\" onclick=\"return Click_Color(this)\">工項進度管控表</font> ";
        //        strHtml += "</td> ";
        //        strHtml += "</tr>";
        //    }
        //    else if (strFuncNo.Equals("D02"))
        //    {
        //        strHtml = "";
        //        strHtml += "<tr onclick=\"GoPage('../Rpt/Rpt02.aspx' , '0');\" style=\"cursor: hand\"> ";
        //        strHtml += "<td align=\"left\" valign=\"middle\" width=\"199\" height=\"30\" background=\"images/P2_EMPTY_UP.gif\"> ";
        //        strHtml += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font ";
        //        strHtml += "style=\"font-size: 10pt; cursor: hand; color: blue; text-decoration: none\" onclick=\"return Click_Color(this)\">計畫經費支用統計表</font> ";
        //        strHtml += "</td> ";
        //        strHtml += "</tr>";
        //    }
        //    else if (strFuncNo.Equals("D03"))
        //    {
        //        strHtml = "";
        //        strHtml += "<tr onclick=\"GoPage('../Rpt/Rpt03.aspx' , '0');\" style=\"cursor: hand\"> ";
        //        strHtml += "<td align=\"left\" valign=\"middle\" width=\"199\" height=\"30\" background=\"images/P2_EMPTY_UP.gif\"> ";
        //        strHtml += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font ";
        //        strHtml += "style=\"font-size: 10pt; cursor: hand; color: blue; text-decoration: none\" onclick=\"return Click_Color(this)\">單位經費支用統計表</font> ";
        //        strHtml += "</td> ";
        //        strHtml += "</tr>";
        //    }
        //    else if (strFuncNo.Equals("D04"))
        //    {
        //        strHtml = "";
        //        strHtml += "<tr onclick=\"GoPage('../Rpt/Rpt04.aspx' , '0');\" style=\"cursor: hand\"> ";
        //        strHtml += "<td align=\"left\" valign=\"middle\" width=\"199\" height=\"30\" background=\"images/P2_EMPTY_UP.gif\"> ";
        //        strHtml += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font ";
        //        strHtml += "style=\"font-size: 10pt; cursor: hand; color: blue; text-decoration: none\" onclick=\"return Click_Color(this)\">南水局-工項進度管控表</font> ";
        //        strHtml += "</td> ";
        //        strHtml += "</tr>";
        //    }
        //    else if (strFuncNo.Equals("D05"))
        //    {
        //        strHtml = "";
        //        strHtml += "<tr onclick=\"GoPage('../Rpt/Rpt05.aspx' , '0');\" style=\"cursor: hand\"> ";
        //        strHtml += "<td align=\"left\" valign=\"middle\" width=\"199\" height=\"30\" background=\"images/P2_EMPTY_UP.gif\"> ";
        //        strHtml += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font ";
        //        strHtml += "style=\"font-size: 10pt; cursor: hand; color: blue; text-decoration: none\" onclick=\"return Click_Color(this)\">南水局-計畫按月分配表</font> ";
        //        strHtml += "</td> ";
        //        strHtml += "</tr>";
        //    }
        //    else if (strFuncNo.Equals("E"))
        //    {
        //        strHtml = "";
        //        strHtml += "<tr id=\"row4\" class=\"title\" title=\"整體計畫樹狀顯示\" style=\"cursor: pointer\" onclick=\"GoPage('../Overview/OverviewList.aspx' , '0');\" ";
        //        strHtml += "onclick=\"CheckDisplay('div4','row4','image5');\"> ";
        //        strHtml += "<td align=\"left\" valign=\"top\"> ";
        //        strHtml += "<a href=\"#\"> ";
        //        strHtml += "<img src=\"../images/e1.jpg\" name=\"image5\" width=\"185\" height=\"58\" border=\"0\" id=\"image5\"></a> ";
        //        strHtml += "</td> ";
        //        strHtml += "</tr> ";
        //        strHtml += "<tr> ";
        //        strHtml += "<td> ";
        //        strHtml += "<div id=\"div4\" style=\"display: none\" runat=\"server\"> ";
        //        strHtml += "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\"> ";
        //        strHtml += "<tr onclick=\"GoPage('../PlanChart/PlanChart.aspx?do=01' , '0');\" style=\"cursor: hand\"> ";
        //        strHtml += "<td align=\"left\" valign=\"middle\" width=\"199\" height=\"30\" background=\"images/P2_EMPTY_UP.gif\"> ";
        //        strHtml += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font ";
        //        strHtml += "style=\"font-size: 10pt; cursor: hand; color: blue; text-decoration: none\" onclick=\"return Click_Color(this)\">計畫大項</font> ";
        //        strHtml += "</td> ";
        //        strHtml += "</tr>";
        //    }
        //    else if (strFuncNo.Equals("F"))
        //    {
        //        strHtml = "";
        //        strHtml += "<tr id=\"row6\" class=\"title\" title=\"整體計畫甘特圖顯示\" style=\"cursor: pointer\" onclick=\"CheckDisplay('div6','row6','image7');\"> ";
        //        strHtml += "<td align=\"left\" valign=\"top\"> ";
        //        strHtml += "<a href=\"#\" onmouseout=\"MM_swapImgRestoreA('div6')\" onmouseover=\"MM_swapImage('image7','','../images/f2.jpg',1)\"> ";
        //        strHtml += "<img src=\"../images/f1.jpg\" name=\"image7\" width=\"185\" height=\"58\" border=\"0\" id=\"image7\"></a> ";
        //        strHtml += "</td> ";
        //        strHtml += "</tr> ";
        //        strHtml += "<tr> ";
        //        strHtml += "<td> ";
        //        strHtml += "<div id=\"div6\" style=\"display: none\" runat=\"server\"> ";
        //        strHtml += "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">";
        //    }
        //    else if (strFuncNo.Equals("F01"))
        //    {
        //        strHtml = "";
        //        strHtml += "<tr onclick=\"GoPage('../PlanChart/PlanChart.aspx' , '0');\" style=\"cursor: hand\"> ";
        //        strHtml += "<td align=\"left\" valign=\"middle\" width=\"199\" height=\"30\" background=\"images/P2_EMPTY_UP.gif\"> ";
        //        strHtml += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font ";
        //        strHtml += "style=\"font-size: 10pt; cursor: hand; color: blue; text-decoration: none\" onclick=\"return Click_Color(this)\">十項計畫統計</font> ";
        //        strHtml += "</td> ";
        //        strHtml += "</tr>";
        //    }
        //    else if (strFuncNo.Equals("F02"))
        //    {
        //        strHtml = "";
        //        strHtml += "<tr onclick=\"GoPage('../PlanChart/PlanChartA.aspx' , '0');\" style=\"cursor: hand\"> ";
        //        strHtml += "<td align=\"left\" valign=\"middle\" width=\"199\" height=\"30\" background=\"images/P2_EMPTY_UP.gif\"> ";
        //        strHtml += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font ";
        //        strHtml += "style=\"font-size: 10pt; cursor: hand; color: blue; text-decoration: none\" onclick=\"return Click_Color(this)\">大項計畫統計</font> ";
        //        strHtml += "</td> ";
        //        strHtml += "</tr>";
        //    }
        //    else if (strFuncNo.Equals("F03"))
        //    {
        //        strHtml = "";
        //        strHtml += "<tr onclick=\"GoPage('../PlanChart/PlanChart.aspx' , '0');\" style=\"cursor: hand\"> ";
        //        strHtml += "<td align=\"left\" valign=\"middle\" width=\"199\" height=\"30\" background=\"images/P2_EMPTY_UP.gif\"> ";
        //        strHtml += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font ";
        //        strHtml += "style=\"font-size: 10pt; cursor: hand; color: blue; text-decoration: none\" onclick=\"return Click_Color(this)\">各年度計統計</font> ";
        //        strHtml += "</td> ";
        //        strHtml += "</tr>";
        //    }
        //    else if (strFuncNo.Equals("G"))
        //    {
        //        strHtml = "";
        //        strHtml += "<tr id=\"row7\" class=\"title\" title=\"權限管理\" style=\"cursor: pointer\" onclick=\"CheckDisplay('div7','row7','image8');\"> ";
        //        strHtml += "<td align=\"left\" valign=\"top\"> ";
        //        strHtml += "<a href=\"#\" onmouseout=\"MM_swapImgRestoreA('div7')\" onmouseover=\"MM_swapImage('image8','','../images/g2.jpg',1)\"> ";
        //        strHtml += "<img src=\"../images/g1.jpg\" name=\"image8\" width=\"185\" height=\"58\" border=\"0\" id=\"image8\"></a> ";
        //        strHtml += "</td> ";
        //        strHtml += "</tr> ";
        //        strHtml += "<tr> ";
        //        strHtml += "<td> ";
        //        strHtml += "<div id=\"div7\" style=\"display: none\" runat=\"server\"> ";
        //        strHtml += "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">";
        //    }
        //    else if (strFuncNo.Equals("G01"))
        //    {
        //        strHtml = "";
        //        strHtml += "<tr onclick=\"GoPage('../SYS/RightSearch.aspx' , '0');\" style=\"cursor: hand\"> ";
        //        strHtml += "<td align=\"left\" valign=\"middle\" width=\"199\" height=\"30\" background=\"images/P2_EMPTY_UP.gif\"> ";
        //        strHtml += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font ";
        //        strHtml += "style=\"font-size: 10pt; cursor: hand; color: blue; text-decoration: none\" onclick=\"return Click_Color(this)\">權限管理</font> ";
        //        strHtml += "</td> ";
        //        strHtml += "</tr>";
        //    }
        //    return strHtml;
        //}
        //#endregion

        //protected void Page_Load(object sender, EventArgs e)
        //{
        //    if (!IsPostBack)
        //    {
        //        WCAPMS.Base.LoginUser loginUser = new WCAPMS.Base.LoginUser(Session);
        //        sUserNo = loginUser.UserNo;
        //        Bind();
        //    }
        //}

        protected void Page_Load(object sender, EventArgs e)
        {
            //*****取得Tree物件*****
            //利用Session 當Tree已經建立過，不重新建立，直接從Session中取得，加快速度
            if (Session["Tree1"] == null)
            {
                //Tree不曾建立過→呼叫建立TreeView程序
                InitTree();
                //初始化Tree
                BuildTree();
                //建立Tree內容
            }
            //宣告Tree物件
            TreeView Tree1 = default(TreeView);
            //從Session中取得Tree物件
            Tree1 = (TreeView)Session["Tree1"];

            //設定Tree的ImageSet
            Tree1.ImageSet = TreeViewImageSet.XPFileExplorer;

            Tree1.ParentNodeStyle.Font.Bold = false;
            //Tree1.HoverNodeStyle.Font.Underline = true;
            //Tree1.HoverNodeStyle.ForeColor = System.Drawing.Color.FromArgb(66,66,66);
            Tree1.SelectedNodeStyle.BackColor = System.Drawing.Color.Yellow;
            Tree1.SelectedNodeStyle.Font.Underline = false;

            //置放於PlaceHolder
            this.PlaceHolder1.Controls.Add(Tree1);
        }
        public void InitTree()
        {
            //********初始化Tree********

            //定義TreeView物件並實體化
            TreeView Tree1 = new TreeView();
            //定義一個TreeNode並實體化
            TreeNode tmpNote = new TreeNode();
            //設定【根目錄】相關屬性內容
            tmpNote.Text = "列印漫遊系統";
            tmpNote.Value = "0";
            tmpNote.NavigateUrl = "~/welcome.aspx";
            tmpNote.Target = "main";

            //Tree建立該Node
            Tree1.Nodes.Add(tmpNote);

            //將Tree存放入Session中
            Session["Tree1"] = Tree1;

        }
        public void BuildTree()
        {
            //********建立樹狀結構********

            //宣告TreeView
            TreeView Tree1 = default(TreeView);

            //如果Session中沒有Tree,初始化Tree
            //if (Session["Tree1"] == null)
            //{
            InitTree();
            //}
            Tree1 = (TreeView)Session["Tree1"];

            //取得根目錄節點
            TreeNode RootNode = default(TreeNode);
            RootNode = Tree1.Nodes[0];
            string rc = null;

            //呼叫建立子節點的函數
            rc = AddNodes(RootNode, 0);
            Session["Tree1"] = Tree1;

        }
        public void GetDataTable()
        {
            //取得DataTable

            //宣告相關變數
            string ConnStr = null;
            SqlConnection Conn = default(SqlConnection);
            SqlDataAdapter Da = default(SqlDataAdapter);
            DataSet Ds = null;
            DataTable dt = null;
            string SqlTxt = null;

            try
            {
                //設定連接字串，請修改符合您的資料來源的ConnectionString
                ConnStr = ConfigurationManager.AppSettings["ConDB"];
                //建立Connection
                Conn = new SqlConnection(ConnStr);
                Conn.Open();

                //設定資料來源T-SQL
                SqlTxt = "SELECT * FROM FunMenu WHERE isShow='1' Order by sOrder";
                //請修改您的資料表名稱
                //實體化DataAdapter並且取得資料
                Da = new SqlDataAdapter(SqlTxt, Conn);
                //實體化DataSet
                Ds = new DataSet();
                //資料填入DataSet
                Da.Fill(Ds);
                //設定DataTable
                dt = new DataTable();
                dt = Ds.Tables[0];
                //將DataTable放入Session中
                Session["Dt"] = dt;
                //關閉連線
                Conn.Close();

            }
            catch (Exception ex)
            {
                this.lblMessage.Text = ex.Message;

            }
            finally
            {
                //資源回收
                Ds = null;
                Da = null;
                Conn = null;

            }

        }
        public string AddNodes(TreeNode tNode, int PId)
        {
            string functionReturnValue = null;
            //******** 遞迴增加樹結構節點 ********
            try
            {
                //如果Session中沒有DataTable→取得DataTable
                if (Session["dt"] == null)
                {
                    GetDataTable();
                }
                //定義DataTable
                DataTable Dt = null;
                //從Session中取得DataTable
                Dt = (DataTable)Session["Dt"];

                //定義DataRow承接DataTable篩選的結果
                DataRow[] rows = null;
                //定義篩選的條件
                string filterExpr = null;
                filterExpr = "ParentId = " + PId;
                //資料篩選並把結果傳入Rows
                rows = Dt.Select(filterExpr);

                //如果篩選結果有資料

                if (rows.GetUpperBound(0) >= 0)
                {

                    DataRow row = null;
                    int tmpNodeId = 0;
                    string tmpsText = null;
                    string tmpsValue = null;
                    string tmpsUrl = null;
                    string tmpsTarget = null;
                    TreeNode NewNode = default(TreeNode);
                    string rc = null;

                    //逐筆取出篩選後資料
                    foreach (DataRow row_loopVariable in rows)
                    {
                        row = row_loopVariable;
                        //放入相關變數中
                        tmpNodeId = Convert.ToInt16(row[0].ToString());
                        tmpsText = row[2].ToString();
                        tmpsValue = row[3].ToString();
                        tmpsUrl = row[4].ToString();
                        tmpsTarget = row[5].ToString();

                        //實體化新節點
                        NewNode = new TreeNode();
                        //設定節點各屬性
                        NewNode.Text = tmpsText;
                        NewNode.Value = tmpsValue;
                        NewNode.NavigateUrl = tmpsUrl;
                        NewNode.Target = tmpsTarget;
                        //將節點加入Tree中
                        tNode.ChildNodes.Add(NewNode);

                        //呼叫遞回取得子節點
                        rc = AddNodes(NewNode, tmpNodeId);

                    }
                }
                //傳回成功訊息
                functionReturnValue = "Success";

            }
            catch (Exception ex)
            {
                this.lblMessage.Text = ex.Message;
                functionReturnValue = "False";

            }
            return functionReturnValue;
        }
    }
}