bool sendLineMsg(int,String,String);

bool sendLineMsg(int status,String Linetoken,String message)
{
  if (status!=WL_CONNECTED) return false; 
  WiFiSSLClient xclient; 
  char host[]="notify-api.line.me";
  String response;
  String url = "/api/notify";

  //Serial.println(message);
  if (xclient.connect(host, 443))
  {
   int msg_len = message.length();
   xclient.println("POST " + url + " HTTP/1.1");
   xclient.print("Host: "); xclient.println(host);
   xclient.print("Authorization: Bearer "); xclient.println(Linetoken);
   xclient.println("Content-Type: application/x-www-form-urlencoded");
   xclient.print("Content-Length: "); xclient.println(String((msg_len+8)));
   xclient.println();
   xclient.print("message="); xclient.println(message);
   xclient.println();
   delay(2000);
   response = xclient.readString();
   //Serial.println(response);
   if (_strstr(response.c_str(),"{\"status\":200,\"message\":\"ok\"}")!=NULL)
   {
    xclient.stop();  
    return true;
   }
   xclient.stop();
 }
 return false;
}
