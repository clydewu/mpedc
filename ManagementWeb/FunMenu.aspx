<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="FunMenu.aspx.cs" Inherits="EDC.FunMenu" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head id="Head1" runat="server">
    <title></title>
    <link href="../css/main.css" rel="stylesheet" type="text/css" />
    <style type="text/css">
        A:link
        {
            color: #3300ff;
            text-decoration: none;
        }
        A:visited
        {
            color: #3300ff;
            text-decoration: none;
        }
        A:active
        {
            color: #3300ff;
            text-decoration: none;
        }
        A:hover
        {
            font-size: 24;
            font-weight: bold;
            color: red;
        }
    </style>
    <script type="text/JavaScript">
<!--
        function MM_preloadImages() { //v3.0
            var d = document; if (d.images) {
                if (!d.MM_p) d.MM_p = new Array();
                var i, j = d.MM_p.length, a = MM_preloadImages.arguments; for (i = 0; i < a.length; i++)
                    if (a[i].indexOf("#") != 0) { d.MM_p[j] = new Image; d.MM_p[j++].src = a[i]; }
            }
        }

        function MM_swapImgRestore() { //v3.0
            var i, x, a = document.MM_sr; for (i = 0; a && i < a.length && (x = a[i]) && x.oSrc; i++) x.src = x.oSrc;
        }
        function MM_swapImgRestoreA(objDiv) { //v3.0
            if (document.getElementById(objDiv).style.display == 'none') {
                var i, x, a = document.MM_sr; for (i = 0; a && i < a.length && (x = a[i]) && x.oSrc; i++) x.src = x.oSrc;
            }
        }

        function MM_findObj(n, d) { //v4.01
            var p, i, x; if (!d) d = document; if ((p = n.indexOf("?")) > 0 && parent.frames.length) {
                d = parent.frames[n.substring(p + 1)].document; n = n.substring(0, p);
            }
            if (!(x = d[n]) && d.all) x = d.all[n]; for (i = 0; !x && i < d.forms.length; i++) x = d.forms[i][n];
            for (i = 0; !x && d.layers && i < d.layers.length; i++) x = MM_findObj(n, d.layers[i].document);
            if (!x && d.getElementById) x = d.getElementById(n); return x;
        }

        function MM_swapImage() { //v3.0
            var i, j = 0, x, a = MM_swapImage.arguments; document.MM_sr = new Array; for (i = 0; i < (a.length - 2); i += 3)
                if ((x = MM_findObj(a[i])) != null) { document.MM_sr[j++] = x; if (!x.oSrc) x.oSrc = x.src; x.src = a[i + 2]; }
        }
//-->
    </script>
    <script type="text/javascript">
        function CheckDisplay(objDiv, row, img) {
            if (document.getElementById(objDiv).style.display == 'none') {
                // 顯示
                document.getElementById(objDiv).style.display = 'block';
                if (img == 'Image2') { document.getElementById(img).src = "images/tr01_OVER.gif"; }
                else if (img == 'Image3') { document.getElementById(img).src = "images/tr02_OVER.gif"; }
                else if (img == 'Image4') { document.getElementById(img).src = "images/tr03_OVER.gif"; }
                document.getElementById(row).title = "隱藏欄位選單";
            }
            else if (document.getElementById(objDiv).style.display != 'none') {
                // 隱藏
                document.getElementById(objDiv).style.display = 'none';
                if (img == 'Image2') { document.getElementById(img).src = "images/tr01_UP.gif"; }
                else if (img == 'Image3') { document.getElementById(img).src = "images/tr02_UP.gif"; }
                else if (img == 'Image4') { document.getElementById(img).src = "images/tr03_UP.gif"; }
                document.getElementById(row).title = "展開欄位選單";
            }
        }
        function GoPage(URL, id) {
            //document.getElementById("RunMenuid").src = "add_fun.aspx?menuid=" +id;
            parent.document.getElementById('main').src = URL;
        }
        function OpenPage(URL) {
            //document.getElementById("RunMenuid").src = "add_fun.aspx?menuid=" +id;
            //eng = open(URL,"eng","width=420,height=340,menubar=no,toolbar=no,left=412,top=290");			
            eng = open(URL, "eng");
        }
      
    </script>
</head>
<body>
    <form id="form1" runat="server">
    <div>
        <div id="showMenu">
            <table width="199" border="0" cellspacing="0" cellpadding="0">
                <div id="divHtml" runat="server"></div>
                <%=sHtml%>
                <tr>
                    <td align="left" valign="top">
                        <img src="images/P2_bottom.gif" width="199" height="5">
                    </td>
                </tr>
            </table>
        </div>
        <script language="vbscript">
	Dim ObjTDGol
	Sub Click_Color(objBETD)
		IF IsObject(ObjTDGol) then
			ObjTDGol.style.backgroundcolor = "#ffffff"
		End IF
		objBETD.style.backgroundcolor = "#FFCCCC"
		Set ObjTDGol = objBETD
	End Sub
        </script>
        <script type="text/javascript">
            function MainTDOnMouseOver(obj) {
                obj.style.border = '1px solid #E2AF01';
                obj.style.backgroundColor = '#FFCCCC';
            }
            function MainTDOnMouseOut(obj) {
                obj.style.borderLeft = '0px solid #FCE7BA';
                obj.style.borderRight = '0px solid #FCE7BA';
                obj.style.borderTop = '0px solid #FCE7BA';
                obj.style.borderBottom = '0px dotted #CCCCCC';
                obj.style.backgroundColor = '#FFFFFF';
            }
        </script>
        <asp:Label ID="lblMessage" runat="server"></asp:Label>
        <asp:PlaceHolder ID="PlaceHolder1" runat="server"></asp:PlaceHolder>
    </div>
    </form>
</body>
</html>
