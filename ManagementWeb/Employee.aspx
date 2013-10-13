<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Employee.aspx.cs" Inherits="EDC.Data.Employee" %>

<%@ Register Src="../UC/noFindData.ascx" TagName="noFindData" TagPrefix="uc1" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head id="Head1" runat="server">
    <title>員工資料維護</title>
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
                            <asp:BoundField DataField="ITEM_NO" HeaderText="大項代碼" />
                            <asp:BoundField DataField="ITEM_NAME" HeaderText="大項名稱" />
                            <asp:BoundField DataField="S_YEAR" HeaderText="期程起" />
                            <asp:BoundField DataField="E_YEAR" HeaderText="期程迄" />
                            <asp:BoundField DataField="BUDGET" HeaderText="總預算(億元) " />
                            <asp:BoundField DataField="DOWN_PLAN_BUDGET" HeaderText="項下金額彙總" />
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
                    <span style="color: #ff0000">*</span>員工編號
                </td>
                <td>
                    <asp:TextBox ID="tbxItemNo" runat="server" Width="50px" MaxLength="2"></asp:TextBox>
                </td>
                <td align="center">
                    <span style="color: #ff0000">*</span>員工名稱
                </td>
                <td>
                    <asp:TextBox ID="tbxItemName" runat="server" Width="200px" MaxLength="50"></asp:TextBox>
                </td>
            </tr>
            <tr>
                <td align="center">
                    <span style="color: #ff0000">*</span>員工帳號
                </td>
                <td>
                    <asp:TextBox ID="TextBox1" runat="server" Width="50px" MaxLength="2"></asp:TextBox>
                </td>
                <td align="center">
                    <span style="color: #ff0000">*</span>性別
                </td>
                <td>
                    <asp:TextBox ID="TextBox2" runat="server" Width="200px" MaxLength="50"></asp:TextBox>
                </td>
            </tr>
            <tr>
                <td align="center">
                    <span style="color: #ff0000">*</span>部門編號
                </td>
                <td>
                    <asp:TextBox ID="TextBox3" runat="server" Width="50px" MaxLength="2"></asp:TextBox>
                </td>
                <td align="center">
                    <span style="color: #ff0000">*</span>卡號
                </td>
                <td>
                    <asp:TextBox ID="TextBox4" runat="server" Width="200px" MaxLength="50"></asp:TextBox>
                </td>
            </tr>
            <tr>
                <td align="center">
                    <span style="color: #ff0000">*</span>初始額度
                </td>
                <td>
                    <asp:TextBox ID="TextBox5" runat="server" Width="50px" MaxLength="2"></asp:TextBox>
                </td>
                <td align="center">
                    <span style="color: #ff0000">*</span>現有額度
                </td>
                <td>
                    <asp:TextBox ID="TextBox6" runat="server" Width="200px" MaxLength="50"></asp:TextBox>
                </td>
            </tr>
            <tr>
                <td align="center">
                    <span style="color: #ff0000">*</span>是否自動回復初始額度
                </td>
                <td>
                    <asp:TextBox ID="TextBox7" runat="server" Width="50px" MaxLength="2"></asp:TextBox>
                </td>
                <td align="center">
                    <span style="color: #ff0000">*</span>角色
                </td>
                <td>
                    <asp:TextBox ID="TextBox8" runat="server" Width="200px" MaxLength="50"></asp:TextBox>
                </td>
            </tr>
        </table>
        <uc1:noFindData ID="NoFindData" runat="server" />
        <br />
    </div>
    </form>
</body>
</html>
