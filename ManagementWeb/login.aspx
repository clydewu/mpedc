<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="login.aspx.cs" Inherits="EDC.login" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">
<head id="Head1" runat="server">
    <meta http-equiv="content-type" content="text/html; charset=UTF-8 ;" />
    <title>登錄主畫面 V1.0</title>
    <link href="../css/main.css" rel="stylesheet" type="text/css" />
    <style type="text/css">
	.TableBorder{ border:1px solid #345487;background-color:#FFF; padding:0; margin:0; width:60%}
	.TableFill  { border:1px solid #345487;background-color:#F5F9FD;padding:6px;  }
	.TablePad   { background-color:#F5F9FD;padding:6px }
	.TableBasic { width:100%; padding:0px 0px 0px 0px; margin:0px; border:0px }
	.MainTitle  { vertical-align:middle;font-weight:bold; color:#FFF; background-image: url('Image/title_bg.gif'); padding-left:5px; padding-right:0px; padding-top:8px; padding-bottom:8px}
	.FormStrip  { background-color: #D1DCEB; color:#3A4F6C;font-weight:bold;padding:7px;margin-top:1px }
	.FormLeft   { background-color: #F5F9FD; padding:6px; margin-top:1px;width:35%; border-top:1px solid #C2CFDF; border-right:1px solid #C2CFDF; }
	.FormRight  { background-color: #F5F9FD; padding:6px; margin-top:1px;border-top:1px solid #C2CFDF; }
	.Login Input {color:darkblue;padding-left:4px;margin:1px;border:1px solid black;background-image:url('Image/shadow.gif');}
	.Login Input.Focus {margin:0px 0px 0px 0px;border-bottom: #ffdead solid 2px;border-right: #ffdead solid 2px;border-left: #c07300 solid 2px;border-top:  #c07300 solid 2px;color:#000000;}
	Table {font-size: 11pt; color: #555555;}
	.btn01 {background-color: #EEEEEE; cursor:hand; color: #854330; border:1px solid #C59B76;}

</style>
</head>
<body style="background-image: url(image/0007.gif); vertical-align: middle; text-align: center;">
    <form id="loginform" runat="server">
        <!--onsubmit ="return checkData();" -->
        <center>
            <table cellspacing="0" cellpadding="0" border="0" width="100%" ">
                <!--最大的表格-->
                <tr>
                    <td style="height: 210px; width: 722px;" align="center">
                        <table cellspacing="0" cellpadding="0" border="0" width="100%" style="height: 210px">
                            <!--標題表格-->
                            <tr>
                                <td style="height: 100px">
                                </td>
                            </tr>
                            <tr>
                                <td align="center" style="height: 110px" >
                                
                                    <b></b><br />
                                    <font color="black">版本：V1.0<br />
                                        <br />
                                        <br />
                                        </font></td>
                            </tr>
                        </table>
                    </td>
                </tr>
                <tr>
                    <td style="width: 722px" align="center">
                        <table style="width: 350px; height: 200px;" class="TableBorder" border="0"  cellspacing="0">
                            <tr >
                                <td>
                                    <table style="width: 350px; height: 200px;" border="0"  cellspacing="0">
                                        <!--登錄表格-->
                                        <tr class='MainTitle'>
                                            <td style="height: 30px;">
                                                <table id="tabShow" width="350" cellspacing="0" cellpadding="0">
                                                    <!--登錄圖片和登錄信息提示表格-->
                                                    <tr class='MainTitle'>
                                                        <td style="width: 32px; height: 22px;">
                                                            <img src="Image/title_nav.gif" style="border: 0" alt='&gt;' width='8' height='10' /><img
                                                                src="Image/members.gif" style="border: 0" alt='&gt;' />&nbsp;
                                                        </td>
                                                        <td>
                                                        </td>
                                                    </tr>
                                                </table>
                                            </td>
                                        </tr>
                                        <tr>
                                            <td>
                                                <asp:Login ID="LogUser" runat="server" PasswordLabelText="密　　　碼:" UserNameLabelText="使用者代號:"
                                                    UserNameRequiredErrorMessage="必須提供使用者代號。" Height="128px" Width="352px" OnAuthenticate="LogUser_Authenticate"
                                                    TitleText="" DestinationPageUrl="~/index.aspx" EnableTheming="True" DisplayRememberMe="False">
                                                    <LoginButtonStyle Width="90px" />
                                                    <CheckBoxStyle HorizontalAlign="Left" />
                                                    <TextBoxStyle Width="160px" />
                                                </asp:Login>
                                            </td>
                                        </tr>
                                    </table>
                                </td>
                            </tr>
                        </table>
                    </td>
                </tr>
            </table>
        </center>
    </form>
</body>
</html>
