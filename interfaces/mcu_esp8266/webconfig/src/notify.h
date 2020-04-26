/* 
  ESP_WebConfig - Notify and Settings Section by DarioCdj

*/


//
//  HTML PAGE
//
const char PAGE_NotifyConfiguration[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Notify Settings</strong>
<hr>
Enter your credential and API:<br><p>
Please at the moment use only one system at time.
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><br><td colspan="2" align="left"><strong>Node Name: </strong></td></tr>
<tr><td align="left">Node Name:</td><td><input type="text" id="nodename" name="nodename" value="" size="30"></td></tr>
<tr><td></td><td>This name will be used also for hostname in OTA.</td></tr>
<tr><td align="left">OTA Password:</td><td><input type="password" id="otapassword" name="otapassword" value="" size="30"></td></tr>
<tr><td></td><td>This This password will protect OTA from hackering your node.</td></tr> 
<tr><br><td colspan="2" align="left"><strong>Notify Message: </strong></td></tr>
<tr><td align="left">Messagge:</td><td><input type="text" id="notifymessage" name="notifymessage" value="" size="50"></td></tr>
<br><p>
<tr><br><td colspan="2" align="left"><strong>Souliss VNET Address: </strong></td></tr>
<tr><td></td><td>Also for Hardcoded communication (Battery Nodes)</td></tr>
<tr><td align="left">Address: </td><td>0x<input type="text" id="soulissaddress" name="soulissaddress" value="" size="4"></td></tr>
<tr><td align="left">Gateway:</td><td>0x<input type="text" id="soulissgateway" name="soulissgateway" value="" size="4"></td></tr>
<tr><td colspan="2"><input type="checkbox" id="deletesubscription" name="deletesubscription">Delete Subscription</td></tr>
<tr><td colspan="2" align="left"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
</table>
<hr size=1>
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><br><td align="left"><strong>Telegram:</strong></td></tr>
<tr><td><input type="checkbox" id="telegramenabled" name="telegramenabled">Enabled</td></tr>
<br>
<tr><td align="left">BOT Token:</td><td><input type="text" id="telegrambottoken" name="telegrambottoken" value="" size="45"></td></tr>
<tr><td align="left">Chat Group:</td><td><input type="text" id="telegramchatgroup" name="telegramchatgroup" value=""></td></tr>
<tr><td align="left">Chat ID:</td><td><input type="text" id="telegramchatid" name="telegramchatid" value=""></td></tr>
<tr><td></td><td>You can find "Chat Group" and "Chat ID" codes easily going to : https://api.telegram.org/bot<putyourtokenhere>/getUpdates and sending some message to bot or group, and then refresh web page.</td></tr>
<tr><td></td><td>ATTENTION: Chat group start with "-" </td></tr>
<tr><td></td></tr>
<tr><td colspan="2" align="left">Please register at <a href="https://www.telegram.org/" target="_blank">www.telegram.org</a></td></tr>
<tr><td colspan="2" align="left">Download APP :  <a href="https://play.google.com/store/apps/details?id=org.telegram.messenger&hl=it" target="_blank">Android</a> - <a href="https://itunes.apple.com/it/app/telegram-messenger/id686449807?mt=8" target="_blank">Apple IOS</a></td></tr>
<tr><td colspan="2" align="left"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
</table>


</form>
<hr>
</table>

<script>

window.onload = function ()
{
	load("style.css","css", function() 
	{
		load("microajax.js","js", function() 
		{
					setValues("/admin/notifyvalues");
					//setTimeout(GetState,3000);
		});
	});


}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}

</script>

)=====";

const char PAGE_WaitAndReloadHome[] PROGMEM = R"=====(
<meta http-equiv="refresh" content="0; URL=admin.html">
<tr><br><td align="left"><strong>Saving and back to Home...Remember to configure network settings!</strong></td></tr>
<br>

)=====";


//
//  SEND HTML PAGE OR IF A FORM SUMBITTED VALUES, PROCESS THESE VALUES
// 

#ifndef ASYNCWEBSERVER
void send_notify_settings_html()
#else
void send_notify_settings_html(AsyncWebServerRequest *request)
#endif
{
#ifndef ASYNCWEBSERVER
	yield();
#endif
#ifndef ASYNCWEBSERVER
	if (server.args() > 0 )  // Save Settings
#else
	if (request->params() > 0 )  // Save Settings
#endif
	{
		String temp = "";
		telegram.telegrambottoken = "";
		telegram.telegramchatgroup = "";
		telegram.telegramchatid = "";
		telegram.telegramenabled = false;
		notify.nodename= "" ;
		notify.otapassword= "" ;
		notify.notifymessage = "";
		notify.soulissaddress = 0;
		notify.soulissgateway = 0;
		notify.deletesubscription = false;
		
		
#ifndef ASYNCWEBSERVER	
		for ( uint8_t i = 0; i < server.args(); i++ ) {
			if (server.argName(i) == "telegrambottoken") 	telegram.telegrambottoken  =   urldecode(server.arg(i));
			if (server.argName(i) == "telegramchatgroup") 	telegram.telegramchatgroup =    urldecode(server.arg(i)); 
			if (server.argName(i) == "telegramchatid") 		telegram.telegramchatid =    urldecode(server.arg(i)); 
			if (server.argName(i) == "telegramenabled") 	telegram.telegramenabled = true;
		}
#else
		for ( uint8_t i = 0; i < request->params(); i++ ) {
			AsyncWebParameter* p = request->getParam(i);
			if (p->name() == "telegrambottoken") 	telegram.telegrambottoken =   urldecode(p->value());
			if (p->name() == "telegramchatgroup") 	telegram.telegramchatgroup =    urldecode(p->value());
			if (p->name() == "telegramchatid") 		telegram.telegramchatid =    urldecode(p->value());
			if (p->name() == "telegramenabled") 	telegram.telegramenabled = true;
		}
#endif

			
#ifndef ASYNCWEBSERVER	
		for ( uint8_t i = 0; i < server.args(); i++ ) {
			if (server.argName(i) == "nodename") 		notify.nodename  =   urldecode(server.arg(i));
			if (server.argName(i) == "otapassword") 		notify.otapassword  =   urldecode(server.arg(i));
			if (server.argName(i) == "notifymessage") 		notify.notifymessage  =   urldecode(server.arg(i));
			if (server.argName(i) == "soulissaddress"){
				char soulissaddresstemp[5];
				server.arg(i).toCharArray(soulissaddresstemp,sizeof(soulissaddresstemp));
				notify.soulissaddress = strtoul(soulissaddresstemp, NULL, 16);
				//sscanf(soulissaddresstemp, "%x", &notify.soulissaddress);  //converts HEX string to a DEC int
			}
			if (server.argName(i) == "soulissgateway"){
				char soulissgatewaytemp[5];
				server.arg(i).toCharArray(soulissgatewaytemp,sizeof(soulissgatewaytemp));
				notify.soulissgateway = strtoul(soulissgatewaytemp, NULL, 16);
			}
			if (server.argName(i) == "deletesubscription") notify.deletesubscription = true;
		}

#else
		for ( uint8_t i = 0; i < request->params(); i++ ) {
			AsyncWebParameter* p = request->getParam(i);
			if (p->name() == "nodename") 	notify.nodename =   urldecode(p->value());
			if (p->name() == "otapassword") 	notify.otapassword =   urldecode(p->value());
			if (p->name() == "notifymessage") 	notify.notifymessage =   urldecode(p->value());
			if (p->name() == "soulissaddress") {
				char soulissaddresstemp[5];
				p->value().toCharArray(soulissaddresstemp,sizeof(soulissaddresstemp));
				notify.soulissaddress =  trtoul(soulissaddresstemp, NULL, 16);
			}	
			
			if (p->name() == "soulissgateway") {
				char soulissgatewaytemp[5];
				p->value().toCharArray(soulissgatewaytemp,sizeof(soulissgatewaytemp));
				notify.soulissgateway = strtoul(soulissgatewaytemp, NULL, 16);
			}
			if (p->name() == "deletesubscription") 	notify.deletesubscription = true;
		}
#endif
	
		// Save the configuration
		WriteConfig();
#ifndef ASYNCWEBSERVER
		server.send (200, "text/html", reinterpret_cast<const __FlashStringHelper *>(PAGE_WaitAndReloadHome ));
#else
		request->send (200, "text/html", reinterpret_cast<const __FlashStringHelper *>(PAGE_WaitAndReloadHome ));
#endif
		delay(5000);
		
		
	}
	else
	{
#ifndef ASYNCWEBSERVER
            server.send_P ( 200, "text/html", PAGE_NotifyConfiguration ); 
#else
            request->send_P ( 200, "text/html", PAGE_NotifyConfiguration );
#endif

	}
}

//
//   FILL THE PAGE WITH VALUES
//

#ifndef ASYNCWEBSERVER
void send_notify_settings_values_html()
#else
void send_notify_settings_values_html(AsyncWebServerRequest *request)
#endif
{
#ifndef ASYNCWEBSERVER
	yield();
#endif
	String notifyvalues ="";
	notifyvalues += "telegrambottoken|" + (String) telegram.telegrambottoken + "|input\n";
	notifyvalues += "telegramchatgroup|" +  (String) telegram.telegramchatgroup + "|input\n";
	notifyvalues += "telegramchatid|" +  (String) telegram.telegramchatid + "|input\n";
	notifyvalues += "telegramenabled|" +  (String) (telegram.telegramenabled ? "checked" : "") + "|chk\n";
	notifyvalues += "nodename|" +  (String) notify.nodename + "|input\n";
	notifyvalues += "otapassword|" +  (String) notify.otapassword + "|input\n";
	notifyvalues += "notifymessage|" +  (String) notify.notifymessage + "|input\n";
	String soulissaddresshex = String (notify.soulissaddress,HEX);
	String soulissgatewayhex = String (notify.soulissgateway,HEX);
	notifyvalues += "soulissaddress|" +  soulissaddresshex + "|input\n";
	notifyvalues += "soulissgateway|" +  soulissgatewayhex + "|input\n";
	notifyvalues += "deletesubscription|" +  (String) (notify.deletesubscription ? "checked" : "") + "|chk\n";
#ifndef ASYNCWEBSERVER
	server.send ( 200, "text/plain", notifyvalues);
#else
	request->send ( 200, "text/plain", notifyvalues);
#endif	
	
}




