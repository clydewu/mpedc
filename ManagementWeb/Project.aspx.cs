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
    public partial class Project : System.Web.UI.Page
    {
        #region private
        public DTO.ProjectDTO ProjectToDTO
        {
            get
            {
                DTO.ProjectDTO dto = new ProjectDTO();

                dto.ProjectID = this.tbxProjectID.Text;
                dto.ProjectName = this.tbxProjectName.Text;
                dto.ProjectNO = this.tbxProjectID.Text;
                dto.ProjectYear = this.tbxProjectYear.Text;

                return dto;
            }
        }

        private void Bind()
        {
            DataTable dt = new DBO.ProjectDetail().getData();
            gridView.DataSource = dt;
            gridView.DataBind();
        }

        private void gridViewBindData(GridViewRow gvr)
        {
            this.tbxProjectID.Text = gvr.Cells[4].Text.Trim();
            this.tbxProjectName.Text = gvr.Cells[5].Text.Trim();
            this.tbxProjectYear.Text = gvr.Cells[7].Text.Trim();
        }

        private void setAdd()
        {
            this.tbxProjectID.Text = "";
            this.tbxProjectName.Text = "";
            this.tbxProjectYear.Text = "";
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
            DBO.ProjectDetail dbo = new ProjectDetail();
            if (dbo.Insert(ProjectToDTO))
            {
                DBO.SyncEDCProjectDetail dbo2 = new SyncEDCProjectDetail();
                dbo2.Insert(ProjectToDTO.ProjectID, "I");
                
                setAdd();
                Bind();
                funMessage.messageBox("新增成功！", ClientScript);
            }
            else
                funMessage.messageBox("新增失敗！", ClientScript);
        }

        protected void btnFind_Click(object sender, ImageClickEventArgs e)
        {
            Bind();
        }

        protected void btnModi_Click(object sender, ImageClickEventArgs e)
        {
            DBO.ProjectDetail dbo = new ProjectDetail();
            if (dbo.Update(ProjectToDTO))
            {
                DBO.SyncEDCProjectDetail dbo2 = new SyncEDCProjectDetail();
                dbo2.Insert(ProjectToDTO.ProjectID, "U");

                setAdd();
                Bind();
                funMessage.messageBox("修改成功！", ClientScript);
            }
            else
                funMessage.messageBox("修改失敗！", ClientScript);
        }

        protected void btnDel_Click(object sender, ImageClickEventArgs e)
        {
            DBO.ProjectDetail dbo = new ProjectDetail();
            if (dbo.Del(ProjectToDTO))
            {
                DBO.SyncEDCProjectDetail dbo2 = new SyncEDCProjectDetail();
                dbo2.Insert(ProjectToDTO.ProjectID, "D");

                setAdd();
                Bind();
                funMessage.messageBox("刪除成功！", ClientScript);
            }
            else
                funMessage.messageBox("刪除失敗！", ClientScript);
        }

        protected void gridView_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (this.gridView.Rows.Count > 0)
            {
                this.hidProjectNo.Value = this.gridView.SelectedRow.Cells[4].Text.ToString().Trim();

                GridViewRow cvr = gridView.SelectedRow;

                gridViewBindData(gridView.SelectedRow);
            }
        }

        protected void gridView_RowCreated(object sender, GridViewRowEventArgs e)
        {

            if (e.Row.RowType == DataControlRowType.DataRow || e.Row.RowType == DataControlRowType.Header)
            {
                //Disable Cells
                for (int i = 4; i <= e.Row.Cells.Count - 1; i++)
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
                //e.Row.Cells[6].Text = Double.Parse(e.Row.Cells[6].Text).ToString("###,###,###,##0.0##");
            }
        }
    }
}