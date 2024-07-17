#include "html.h"

String Web_chktxt="Supervisor";       // with html.h of data same !!
String Web_chkpw="HUB5168P";          // Web check of password !!
String Web_ssidtxt="NewSSID";         // with html.h of data same !!
String Web_passwordtxt="NewPassword"; // with html.h of data same !!
String Web_tokentxt="TokenKey";       // with html.h of data same !!

typedef struct
{
 String ssid;
 String pass;
 String token;
} Webdata;

/*Webdata clearData(Webdata *st)
{
    st->ssid = "";
    st->pass = "";
    st->token = "";
    return *st;
}*/

WiFiServer server(80);

bool SendHTMLData(int status,Webdata *st)
{
 byte i=0;
 char tempdata[255];
 String Html_data; 
 if (!st || status!=WL_CONNECTED) return false;	
 delay(500);
 WiFiClient client = server.available();
 if (client)
 {
  //Serial.println("new client");
  // an http request ends with a blank line
  boolean currentLineIsBlank = true;
  while (client.connected())
  {
   if (client.available())
   {
    char c = client.read();
    //Serial.print(c);
    tempdata[i]=c;
    if (c == '\n' && currentLineIsBlank) //等待請求頭接收完成(接收到空行)
    { 
     client.print(responseHeaders); //向客戶端輸出網頁響應
     client.print(myhtmlPage);      //向客戶端輸出網頁內容
     break;    
    }
    if (c=='\n')
    {
     // you're starting a new line  
     currentLineIsBlank = true;  
     tempdata[i]='\0';
     Html_data=String(tempdata);   
     //Serial.print(Html_data);
     i=0;
     if (Html_data.startsWith("GET"))
     {
      String s1,s2,s3,s4,s5,s6,temp;
      int index;
      index = Html_data.indexOf('?');
      Html_data.substring(0, index).toInt();
      s1 = Html_data.substring(index+1 , Html_data.length());
      //Serial.println(s1);
      index = s1.indexOf('=');
      temp = s1.substring(0 , index);
      //Serial.println(temp);
      if (temp==Web_chktxt)
      {
       index = s1.indexOf('=');
       s1.substring(0, index).toInt();
       s2 = s1.substring(index +1 , Html_data.length());
       //Serial.println(s2);
       index = s2.indexOf('&');
       temp = s2.substring(0 , index);
       //Serial.println(temp);
       if (temp==Web_chkpw)
       {
        index = s2.indexOf('&');
        s2.substring(0, index).toInt();
        s3 = s2.substring(index+1 , Html_data.length());
        //Serial.println(s3);
        index = s3.indexOf('=');
        temp = s3.substring(0 , index);
        //Serial.println(temp);
        if (temp==Web_ssidtxt)
        {
         index = s3.indexOf('=');
         s3.substring(0, index).toInt();
         s4 = s3.substring(index +1 , Html_data.length());
         //Serial.println(s4);
         index = s4.indexOf('&');
         temp = s4.substring(0 , index);
         st->ssid=temp;
         //Serial.println(temp);
        }
        index = s3.indexOf('&');
        s3.substring(0, index).toInt();
        s4 = s3.substring(index+1 , Html_data.length());
        //Serial.println(s4);
        index = s4.indexOf('=');
        temp = s4.substring(0 , index);
        //Serial.println(temp);
        if (temp==Web_passwordtxt)
        {
         index = s4.indexOf('=');
         s4.substring(0, index).toInt();
         s5 = s4.substring(index +1 , Html_data.length());
         //Serial.println(s5);
         index = s5.indexOf('&');
         temp = s5.substring(0 , index);
         st->pass=temp;
         //Serial.println(temp);
        }
        index = s4.indexOf('&');
        s4.substring(0, index).toInt();
        s5 = s4.substring(index+1 , Html_data.length());
        //Serial.println(s5);
        index = s5.indexOf('=');
        temp = s5.substring(0 , index);
        //Serial.println(temp);
        if (temp==Web_tokentxt)
        {
         index = s5.indexOf('=');
         s5.substring(0, index).toInt();
         s6 = s5.substring(index +1 , Html_data.length());
         //Serial.println(s6);
         index = s6.indexOf(' ');
         temp = s6.substring(0 , index);
         st->token=temp;
         //Serial.println(temp);
        }
        Serial.println(st->ssid);
        Serial.println(st->pass);
        Serial.println(st->token);
        client.print(responseHeaders);
        client.println(myhtmlPageEnd);
        delay(1000);
        client.stop();
        delay(200);
        Serial.println("Get Data is ok");
        return true;
       }
      }
     }
     else
     {
      st->ssid=st->pass=st->token="Error";  
       //Serial.println("Set_fail");
     }     
    }
    else
    {
     if (c != '\r')
     {
      // you've gotten a character on the current line            
      currentLineIsBlank = false;
     }
    }
    i++;
   }
  }
  // give the web browser time to receive the data
  delay(10);
  // close the connection:
  client.stop(); 
  Serial.println("client disonnected");
 }
 return false;
}