/* 
  ESP_WebConfig - Notify Section by DarioCdj

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
<tr><br><td align="left"><strong>Pushetta: </strong></td></tr>
<tr><td><input type="checkbox" id="pushettaenabled" name="pushettaenabled">Enable</td></tr>
<br>

<tr><td align="left">API Key:</td><td><input type="text" id="pushettaapikey" name="pushettaapikey" value="" size="30"></td></tr>
<tr><td align="left">Channel:</td><td><input type="text" id="pushettachannel" name="pushettachannel" value=""></td></tr>
<br>
<tr><td colspan="2" align="left">Please register at <a href="www.pushetta.com" target="_blank">www.pushetta.com</a></td></tr>
<tr><td colspan="2" align="left">Download APP :  <a href="https://play.google.com/store/apps/details?id=com.gumino.pushetta&hl=it" target="_blank">Android</a> - <a href="https://itunes.apple.com/it/app/pushetta/id930851629?mt=8" target="_blank">Apple IOS</a></td></tr>
<tr><td colspan="2" align="left"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
</table>
<hr size=1>

<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><br><td align="left"><strong>Pushover:</strong></td></tr>
<tr><td><input type="checkbox" id="pushoverenabled" name="pushoverenabled">Enable</td></tr>
<br>
<tr><td align="left">APIToken:</td><td><input type="text" id="pushoverapitoken" name="pushoverapitoken" value="" size="30"></td></tr>
<tr><td align="left">USERKey:</td><td><input type="text" id="pushoveruserkey" name="pushoveruserkey" value="" size="30"></td></tr>
<tr><td align="left">Device:</td><td><input type="text" id="pushoverdevice" name="pushoverdevice" value=""></td></tr>
<tr><td></td>Leave Empty for all<td></td></tr>
<tr><td align="left">Sound:</td><td><input type="text" id="pushoversound" name="pushoversound" value=""></td></tr>
<tr><td></td>Use "pushover" as default<td></td></tr>
<tr><td colspan="2" align="left">Please register at <a href="https://pushover.net/" target="_blank">www.pushover.net</a></td></tr>
<tr><td colspan="2" align="left">Download APP :  <a href="https://play.google.com/store/apps/details?id=net.superblock.pushover&hl=it" target="_blank">Android</a> - <a href="https://itunes.apple.com/it/app/pushover-notifications/id506088175?mt=8" target="_blank">Apple IOS</a></td></tr>
<tr><td colspan="2" align="left"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
</table>

<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><br><td align="left"><strong>Telegram:</strong></td></tr>
<tr><td><input type="checkbox" id="telegramenabled" name="telegramenabled">Enabled</td></tr>
<br>
<tr><td align="left">BOT Token:</td><td><input type="text" id="telegrambottoken" name="telegrambottoken" value="" size="35"></td></tr>
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
		pushetta.pushettaapikey = "";
		pushetta.pushettachannel = "";
		pushetta.pushettaenabled = false;
		pushover.pushoverapitoken = "";
		pushover.pushoveruserkey = "";
		pushover.pushoverdevice = "";
		pushover.pushoversound = "";
		pushover.pushoverenabled = false;
		telegram.telegrambottoken = "";
		telegram.telegramchatgroup = "";
		telegram.telegramchatid = "";
		telegram.telegramenabled = false;
		
		
#ifndef ASYNCWEBSERVER	
		for ( uint8_t i = 0; i < server.args(); i++ ) {
			if (server.argName(i) == "pushettaapikey") pushetta.pushettaapikey  =   urldecode(server.arg(i));
			if (server.argName(i) == "pushettachannel") pushetta.pushettachannel =    urldecode(server.arg(i)); 
			if (server.argName(i) == "pushettaenabled") pushetta.pushettaenabled = true;
		}
#else
		for ( uint8_t i = 0; i < request->params(); i++ ) {
			AsyncWebParameter* p = request->getParam(i);
			if (p->name() == "pushettaapikey") pushetta.pushettaapikey =   urldecode(p->value());
			if (p->name() == "pushettachannel") pushetta.pushettachannel =    urldecode(p->value());
			if (p->name() == "pushettaenabled") pushetta.pushettaenabled = true;
		}
#endif

#ifndef ASYNCWEBSERVER	
		for ( uint8_t i = 0; i < server.args(); i++ ) {
			if (server.argName(i) == "pushoverapitoken") 	pushover.pushoverapitoken  =   urldecode(server.arg(i));
			if (server.argName(i) == "pushoveruserkey") 	pushover.pushoveruserkey =    urldecode(server.arg(i)); 
			if (server.argName(i) == "pushoverdevice") 		pushover.pushoverdevice =    urldecode(server.arg(i)); 
			if (server.argName(i) == "pushoversound") 		pushover.pushoversound =    urldecode(server.arg(i)); 
			if (server.argName(i) == "pushoverenabled") 	pushover.pushoverenabled = true;
		}
#else
		for ( uint8_t i = 0; i < request->params(); i++ ) {
			AsyncWebParameter* p = request->getParam(i);
			if (p->name() == "pushoverapitoken") 	pushover.pushoverapitoken =   urldecode(p->value());
			if (p->name() == "pushoveruserkey") 	pushover.pushoveruserkey =    urldecode(p->value());
			if (p->name() == "pushoverdevice") 		pushover.pushoverdevice =    urldecode(p->value());
			if (p->name() == "pushoversound") 		pushover.pushoversound =    urldecode(p->value());
			if (p->name() == "pushoverenabled") 	pushover.pushoverenabled = true;
		}
#endif

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
	notifyvalues += "pushettaapikey|" + (String) pushetta.pushettaapikey + "|input\n";
	notifyvalues += "pushettachannel|" +  (String) pushetta.pushettachannel + "|input\n";
	notifyvalues += "pushettaenabled|" +  (String) (pushetta.pushettaenabled ? "checked" : "") + "|chk\n";
	notifyvalues += "pushoverapitoken|" + (String) pushover.pushoverapitoken + "|input\n";
	notifyvalues += "pushoveruserkey|" +  (String) pushover.pushoveruserkey + "|input\n";
	notifyvalues += "pushoverdevice|" +  (String) pushover.pushoverdevice + "|input\n";
	notifyvalues += "pushoversound|" +  (String) pushover.pushoversound + "|input\n";
	notifyvalues += "pushoverenabled|" +  (String) (pushover.pushoverenabled ? "checked" : "") + "|chk\n";
	notifyvalues += "telegrambottoken|" + (String) telegram.telegrambottoken + "|input\n";
	notifyvalues += "telegramchatgroup|" +  (String) telegram.telegramchatgroup + "|input\n";
	notifyvalues += "telegramchatid|" +  (String) telegram.telegramchatid + "|input\n";
	notifyvalues += "telegramenabled|" +  (String) (telegram.telegramenabled ? "checked" : "") + "|chk\n";
#ifndef ASYNCWEBSERVER
	server.send ( 200, "text/plain", notifyvalues);
#else
	request->send ( 200, "text/plain", notifyvalues);
#endif	
	
}




