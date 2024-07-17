//響應頭
String responseHeaders =
    String("") +
    "HTTP/1.1 200 OK\r\n" +
    "Content-Type: text/html\r\n" +
    "Connection: close\r\n" +
    "\r\n";

//網頁
String myhtmlPage =
    String("") +
"<!DOCTYPE html>"+
"<html>"+
"<head>"+
"<meta content=\"text/html; charset=UTF-8\" http-equiv=\"content-type\">  "+
"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"+
"<style>"+
"* {"+
"  box-sizing: border-box;"+
"}"+
"input[type=text], select, textarea {"+
"  width: 100%;"+
"  padding: 12px;"+
"  border: 1px solid #ccc;"+
"  border-radius: 4px;"+
"  resize: vertical;"+
"}"+
"label {"+
"  padding: 12px 12px 12px 0;"+
"  display: inline-block;"+
"}"+
"input[type=submit] {"+
"  background-color: #4CAF50;"+
"  color: white;"+
"  padding: 12px 20px;"+
"  border: none;"+
"  border-radius: 4px;"+
"  cursor: pointer;"+
"  float: right;"+
"}"+
"input[type=submit]:hover {"+
"  background-color: #45a049;"+
"}"+
".container {"+
"  border-radius: 5px;"+
"  background-color: #f2f2f2;"+
"  padding: 20px;"+
"}"+
".col-25 {"+
"  float: left;"+
"  width: 25%;"+
"  margin-top: 6px;"+
"}"+
".col-75 {"+
"  float: left;"+
"  width: 75%;"+
"  margin-top: 6px;"+
"}"+
"/* Clear floats after the columns */"+
".row:after {"+
"  content: \"\";"+
"  display: table;"+
"  clear: both;"+
"}"+
"/* Responsive layout - when the screen is less than 600px wide, make the two columns stack on top of each other instead of next to each other */"+
"@media screen and (max-width: 600px) {"+
"  .col-25, .col-75, input[type=submit] {"+
"    width: 100%;"+
"    margin-top: 0;"+
"  }"+
"}"+
"</style>"+
"</head>"+
"<body>"+
"<h2>HUB5168P 網路功能設置 ...</h2>"+
"<div class=\"container\">"+
"  <form action=\"index.htm\" method=\"get\">"+
"    <div class=\"row\">"+
"      <div class=\"col-25\">"+
"        <label for=\"afname\">設備管理者確認密碼:</label>"+
"      </div>"+
"      <div class=\"col-75\">"+
"        <input type=\"text\" id=\"afname\" name=\"Supervisor\" placeholder=\"Enter the administrator password to confirm..\">"+
"      </div>"+
"    </div>"+
"    <div class=\"row\">"+
"      <div class=\"col-25\">"+
"        <label for=\"blname\">無線基地台的 SSID :</label>"+
"      </div>"+
"      <div class=\"col-75\">"+
"        <input type=\"text\" id=\"blname\" name=\"NewSSID\" placeholder=\"Your AP of SSID..\">"+
"      </div>"+
"    </div>"+
"    <div class=\"row\">"+
"      <div class=\"col-25\">"+
"        <label for=\"clname\">無線基地台的連線密碼:</label>"+
"      </div>"+
"      <div class=\"col-75\">"+
"        <input type=\"text\" id=\"clname\" name=\"NewPassword\" placeholder=\"Your AP of PSK..\">"+
"      </div>"+
"    </div>"+
"    <div class=\"row\">"+
"      <div class=\"col-25\">"+
"        <label for=\"dlname\">LINE 的授權碼 (TokenKey):</label>"+
"      </div>"+
"      <div class=\"col-75\">"+
"        <input type=\"text\" id=\"dlname\" name=\"TokenKey\" placeholder=\"Your LineNotifly of TokenKey...\">"+
"      </div>"+
"    </div>      "+
"    <br></br>      "+
"    <div class=\"row\">"+
"      <input type=\"submit\" value=\"儲存\">"+
"    </div>"+
"  </form>"+
"</div>"+
"</body>"+
"</html>";


//網頁
String myhtmlPageEnd =
    String("") +
"<!DOCTYPE html>"+
"<html>"+
"<head>"+
"<meta content=\"text/html; charset=UTF-8\" http-equiv=\"content-type\">  "+
"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"+
"</head>"+
"<body>"+
"<h2>網路資料設置完成，請五秒後關閉HUB5168P電源</h2>"+
"<h2>並將更新開關關閉後再次連接電源...</h2>"+
"</body>"+
"</html>";
