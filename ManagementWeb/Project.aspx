<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Project.aspx.cs" Inherits="EDC.Data.Project" %>

<%@ Register Src="../UC/noFindData.ascx" TagName="noFindData" TagPrefix="uc1" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head id="Head1" runat="server">
    <title>專案資料維護</title>
    <link href="../css/main.css" rel="stylesheet" type="text/css" />
    <link href="../css/changetab.css" rel="stylesheet" type="text/css" />
</head>
<body>
    <form id="form1" runat="server">
    <div>
        <table id="table2" border="0" cellspacing="0" style="width: 750px">
            <tr>
                <td style="width: 80px; height: 26px">
                    <asp:ImageButton ID="btnFind" runat="server" ImageUrl="~/image/tools/select.jpg"
                        OnClick="btnFind_Click" />
                </td>
                <td style="width: 80px; height: 26px">
                    <asp:ImageButton ID="btnAdd" runat="server" ImageUrl="~/image/tools/add.jpg" OnClick="btnAdd_Click" />
                </td>
                <td style="width: 80px; height: 26px">
                    <asp:ImageButton ID="btnModi" runat="server" ImageUrl="~/image/tools/update.jpg"
                        OnClick="btnModi_Click" />
                </td>
                <td style="width: 96px; height: 26px">
                    &nbsp;<asp:ImageButton ID="btnDel" runat="server" ImageUrl="~/image/tools/delete.jpg"
                        OnClick="btnDel_Click" />
                </td>
                <td style="width: 80px; height: 26px">
                    &nbsp;
                </td>
                <td style="width: 80px; height: 26px">
                    &nbsp;
                </td>
                <td style="width: 392px; height: 26px">
                    &nbsp;
                </td>
            </tr>
        </table>
        <asp:HiddenField ID="hidProjectNo" runat="server" />
        <table border="0" cellpadding="0" cellspacing="0" style="width: 750px">
            <tr>
                <td style="width: 100px">
                    <asp:GridView ID="gridView" runat="server" AllowPaging="True" BackColor="Black" BorderWidth="1px"
                        CellPadding="4" CellSpacing="1" ForeColor="#333333" GridLines="None" Width="750px"
                        OnSelectedIndexChanged="gridView_SelectedIndexChanged" OnRowCreated="gridView_RowCreated"
                        OnPageIndexChanging="gridView_PageIndexChanging" OnRowDataBound="gridView_RowDataBound">
                        <RowStyle BackColor="White" />
                        <Columns>
                            <asp:TemplateField ItemStyle-Width="15px">
                                <ItemTemplate>
                                    <center>
                                        <%#Container.DataItemIndex+1%></center>
                                </ItemTemplate>
                            </asp:TemplateField>
                            <asp:BoundField DataField="ProjectID" HeaderText="專案編號" />
                            <asp:BoundField DataField="ProjectName" HeaderText="專案名稱" />
                            <asp:BoundField DataField="ProjectYear" HeaderText="專案年度" />
                        </Columns>
                        <PagerStyle BackColor="#EBEAED" ForeColor="Black" HorizontalAlign="Center" />
                        <SelectedRowStyle BackColor="DarkSlateBlue" Font-Bold="True" ForeColor="White" />
                        <HeaderStyle BackColor="#EBEAED" Font-Bold="True" ForeColor="Black" HorizontalAlign="Center" />
                        <AlternatingRowStyle BackColor="#FFFFCC" BorderColor="White" />
                    </asp:GridView>
                    &nbsp;
                </td>
            </tr>
        </table>
        <table border="1" cellpadding="0" cellspacing="0" style="width: 750px; border-collapse: collapse">
            <tr>
                <td align="center">
                    <span style="color: #ff0000">*</span>專案編號
                </td>
                <td>
                    <asp:TextBox ID="tbxProjectID" runat="server" Width="50px" MaxLength="4"></asp:TextBox>
                </td>
                <td align="center">
                    <span style="color: #ff0000">*</span>專案名稱
                </td>
                <td>
                    <asp:TextBox ID="tbxProjectName" runat="server" Width="200px" MaxLength="50"></asp:TextBox>
                </td>
            </tr>
            <tr>
                <td align="center">
                    <span style="color: #ff0000">*</span>專案年度
                </td>
                <td colspan="3">
                    <asp:TextBox ID="tbxProjectYear" runat="server" Width="50px" MaxLength="4"></asp:TextBox>
                </td>
            </tr>
        </table>
        <uc1:noFindData ID="NoFindData" runat="server" />
        <br />
    </div>
    </form>
</body>
</html>
