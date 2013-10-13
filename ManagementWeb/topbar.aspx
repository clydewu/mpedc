<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="topbar.aspx.cs" Inherits="EDC.topbar" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<script language="JavaScript" type="text/javascript">

    ///狀態欄顯示 
    ///keynameValue：顯示信訊
    ///labType：顯示之位置
    ///isErr：是否為出錯信訊,true表示出錯為紅色顯示
    function setLabMag(keynameValue, labType, isErr) {
        var tdName = ""

        if (labType == "1") {
            //功能名稱
            tdName = "tbfunName";
        }
        else if (labType == "2") {
            //提示信訊
            tdName = "tbProMag";
        }

        //取得相關的元件
        var objCell = document.getElementById(tdName);

        if (objCell == null) {
            return;
        }

        //當為出錯提示時，顯示為紅色 
        if (isErr == "Y") {
            keynameValue = "<font color='#FF0000'>" + keynameValue + "</font>";
        }

        if (tdName == "tbfunName") {
            keynameValue = "<b>" + keynameValue + "</b>";
        }

        objCell.innerHTML = keynameValue;

    }

    ///設定使用者名稱
    function setUserName(useName) {
        var obj = document.getElementById("tdUserName");

        if (obj == null) {
            return;
        }

        obj.value = useName
    }

    ///設定使用者名稱
    function setUserBranch(userBranch) {
        var obj = document.getElementById("tdUserBranch");

        if (obj == null) {
            return;
        }

        obj.value = userBranch
    }




    ///畫面提示顯示說明 
    ///funName：功能名稱 
    ///funNo：功能編號
    function openFun(funName, funNo) {
        if (funNo == null) {
            return;
        }

        if (funNo.length == null) {
            return;
        }


        if (funNo.length < 3) {
            return;
        }

        var dataList = funNo.split("&");

        var funNamTmp = dataList[0] + " " + funName + "(" + dataList[1] + ")";
        // var funNamTmp =  dataList[0] + " " + funName ;

        //顯示功能之名稱
        setLabMag(funNamTmp, "1", "N");


        //組成功能的路徑
        var funLink;
        funLink = "../" + dataList[1].substr(0, 2) + "/" + dataList[1] + "/" + dataList[1] + "01.aspx";

        //打開功能畫面
        frm.target = "main";
        frm.action = funLink;
        frm.submit();
        return;
    }

    ///登出到進入畫面
    function exitClick() {
        top.location.href = '../login.aspx';
    }
</script>

<html xmlns="http://www.w3.org/1999/xhtml">
<head id="Head1" runat="server">
    <title>topbar</title>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <%--<table id="tableObject" style="width: 100%;display: none;" cellpadding="0" cellspacing="0" border="0" >
        <object   id="CrystalPrintControl"   classid="CLSID:BAEE131D-290A-4541-A50A-8936F159563A"   codebase="PrintControl.cab"   #Version="10,2,0,1078"   viewastext></object> 
    </table>--%>
</head>
<body style="min-width: 720px;background-image: url(../images/title.jpg);background-repeat: no-repeat; background-position: top left; background-color:#339933;">
    <form id="frm" action="" method="get">
        
        <table id="menu" style="width: 100%" cellpadding="0" cellspacing="0" border="0">
            <tr style="height: 50px">
                <td style="width: 167px; background-position: left center;">
                   <%--<img src="../image/logo.jpg" width="167px"/>--%>
                </td>
                <td style="width: 202px; height: 26px; background-repeat: no-repeat; background-position: center center;">
                   <%-- <img src="../image/logo_c_2008.jpg" width="202px" />--%>
                </td>
                <td style="width: 10px;">
                </td>
                <td valign="middle" style="width: 380px; font-size: 16pt; font-weight: bold; text-align: center;
                    color: #0255A3" id="tbfunName">
                </td>
                <td>
                </td>
                <td align="right">
                    <table style="width: 200px" cellpadding="0" cellspacing="0" border="0">
                        <tr style="height: 19px">
                            <td>
                            </td>
                        </tr>
                        <tr style="height: 22px" valign="bottom">
                            <td style="background-image: url(../image/bar.jpg); background-repeat: no-repeat;
                                text-align: left; height: 22px;">
                                <table style="width: 200px" cellpadding="0" cellspacing="0" border="0">
                                    <tr style="height: 22px">
                                        <td style="width: 80px">
                                            &nbsp;&nbsp;<span style="font-size: 10pt; color: #339933">&gt;&gt;使用者</span></td>
                                        <td style="width: 55px">
                                            <input id="tdUserName" type="text" style="width: 120px; height: 13px; font-size: 8pt"
                                                value="" readonly="readonly" />
                                        </td>
                                    </tr>
                                </table>
                            </td>
                        </tr>
                        <tr style="height: 1px">
                            <td style="height: 1px">
                            </td>
                        </tr>
                        <tr style="height: 22px">
                            <td style="vertical-align: middle; background-image: url(../image/bar.jpg); background-repeat: no-repeat;
                                text-align: left; height: 22px;">
                                <table cellpadding="0" cellspacing="0" border="0" style="width: 200px; height: 22px">
                                    <tr>
                                        <td style="width: 80px">
                                            &nbsp;&nbsp;<span style="font-size: 10pt; color: #339933">&gt;&gt;單位</span></td>
                                        <td style="width: 55px">
                                            <input id="tdUserBranch" type="text" style="width: 120px; height: 13px; font-size: 8pt"
                                                value="" readonly="readonly" />
                                        </td>
                                    </tr>
                                </table>
                            </td>
                    </table>
                </td>
                <td align="center">
                    <img src="../Image/members.gif" style="border: 0" alt='&gt;' /><br>
                    <button id="btnExit" onclick="exitClick()" style="height: 25px; background-color: PaleTurquoise;
                        border-left-color: paleturquoise; border-bottom-color: paleturquoise; border-top-style: solid;
                        border-top-color: paleturquoise; border-right-style: solid; border-left-style: solid;
                        border-right-color: paleturquoise; border-bottom-style: solid;">
                        登出</button>
                </td>
            </tr>
        </table>
    </form>
    <!--------設定由伺服器返回資料之處理---------->
    <form id="sevfrm" runat="server">
    </form>
</body>
</html>
