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

namespace EDC.Data
{
    public partial class Level : System.Web.UI.Page
    {
        #region private
        //public DTO.PlanItemDTO PlanItemToDTO
        //{
        //    get
        //    {
        //        DTO.PlanItemDTO piDTO = new PlanItemDTO();

        //        piDTO.ItemNo = this.tbxItemNo.Text;
        //        piDTO.ItemName = this.tbxItemName.Text;
        //        piDTO.SYear = this.tbxSYear.Text;
        //        piDTO.EYear = this.tbxEYear.Text;
        //        piDTO.Budget = this.tbxBudget.Text;
        //        piDTO.OrderRpt = "0";
        //        piDTO.Memo = "";
        //        piDTO.UpdUserNo = "";

        //        return piDTO;
        //    }
        //}

        private void Bind()
        {
            //DataTable dt = new PlanItemDetail().getPlanItem("2");
            //gridView.DataSource = dt;
            //gridView.DataBind();
        }

        private void gridViewBindData(GridViewRow gvr)
        {
            //this.tbxItemNo.Text = gvr.Cells[7].Text.Trim();
            //this.tbxItemName.Text = gvr.Cells[8].Text.Trim();
            //this.tbxSYear.Text = gvr.Cells[9].Text.Trim();
            //this.tbxEYear.Text = gvr.Cells[10].Text.Trim();
            //this.tbxBudget.Text = gvr.Cells[11].Text.Trim();
        }

        private void setAdd()
        {
            //this.tbxItemNo.Text = "";
            //this.tbxItemName.Text = "";
            //this.tbxSYear.Text = "";
            //this.tbxEYear.Text = "";
            //this.tbxBudget.Text = "";
        }

        #endregion
        protected void Page_Load(object sender, EventArgs e)
        {
            if (!IsPostBack)
            {
                Bind();
            }
        }

        /// <summary>
        /// 設定GridView中的單選事項為選取的處理
        /// </summary>
        /// <param name="writer"></param>
        protected override void Render(HtmlTextWriter writer)
        {
            // GridView處理
            foreach (GridViewRow row in this.gridView.Rows)
            {
                if (row.RowType == DataControlRowType.DataRow)
                {
                    // 設定腳本，單選時為選取的設定。
                    row.Attributes["onclick"] = ClientScript.GetPostBackEventReference(this.gridView, "Select$" + row.RowIndex.ToString(), true);
                }
            }

            base.Render(writer);
        }
        protected void btnAdd_Click(object sender, ImageClickEventArgs e)
        {
            //DBO.PlanItemDetail piDetail = new PlanItemDetail();
            //if (piDetail.Insert(PlanItemToDTO))
            //{
            //    setAdd();
            //    Bind();
            //    funMessage.messageBox("新增成功！", ClientScript);
            //}
            //else
            //    funMessage.messageBox("新增失敗！", ClientScript);
        }

        protected void btnFind_Click(object sender, ImageClickEventArgs e)
        {
            Bind();
        }

        protected void btnModi_Click(object sender, ImageClickEventArgs e)
        {
            //DBO.PlanItemDetail piDetail = new PlanItemDetail();
            //if (piDetail.Update(PlanItemToDTO))
            //{
            //    setAdd();
            //    Bind();
            //    funMessage.messageBox("修改成功！", ClientScript);
            //}
            //else
            //    funMessage.messageBox("修改失敗！", ClientScript);
        }

        protected void btnDel_Click(object sender, ImageClickEventArgs e)
        {
            //DBO.PlanItemDetail piDetail = new PlanItemDetail();
            //if (piDetail.Del(PlanItemToDTO))
            //{
            //    setAdd();
            //    Bind();
            //    funMessage.messageBox("刪除成功！", ClientScript);
            //}
            //else
            //    funMessage.messageBox("刪除失敗！", ClientScript);
        }

        protected void gridView_SelectedIndexChanged(object sender, EventArgs e)
        {
            //if (this.gridView.Rows.Count > 0)
            //{
            //    this.hidItemNo.Value = this.gridView.SelectedRow.Cells[3].Text.ToString().Trim();

            //    GridViewRow cvr = gridView.SelectedRow;

            //    gridViewBindData(gridView.SelectedRow);
            //}
        }

        protected void gridView_RowCreated(object sender, GridViewRowEventArgs e)
        {

            if (e.Row.RowType == DataControlRowType.DataRow || e.Row.RowType == DataControlRowType.Header)
            {
                //Disable Cells
                for (int i = 7; i <= e.Row.Cells.Count - 1; i++)
                {
                    e.Row.Cells[i].Visible = false;
                }
            }

        }

        protected void gridView_PageIndexChanging(object sender, GridViewPageEventArgs e)
        {
            gridView.PageIndex = e.NewPageIndex;
            Bind();
        }
        protected void gridView_RowDataBound(object sender, GridViewRowEventArgs e)
        {
            if (e.Row.RowType == DataControlRowType.DataRow)
            {
                e.Row.Cells[6].Text = Double.Parse(e.Row.Cells[6].Text).ToString("###,###,###,##0.0##");
            }
        }
    }
}