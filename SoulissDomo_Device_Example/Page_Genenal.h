//
//  HTML PAGE
//

const char PAGE_AdminGeneralSettings[] PROGMEM =  R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>General Settings</strong>
<hr>
<form action="" method="post">
<table border="0"  cellspacing="0" cellpadding="3" >
<tr>
	<td align="left">Node Name</td>
	<td><input type="text" id="devicename" name="devicename" value=""></td>
</tr>
<hr>
<tr><p>
	<td align="left" colspan="2">Node Mode</td>
</tr>
<hr>
<tr>
	<td align="left"> Enabled this Node as Gateway?:</td>
	<td><input type="checkbox" id="mnenabled" name="mnenabled"></td>
	
	<tr><td>Sensors Configuration:</td><td>
	<select  id="byte0" name="byte0">
		<option value="0">None</option>
		<option value="1">DHT</option>
		<option value="2">LDR</option>
		<option value="3">DALLAS</option>
		<option value="4">DHT LDR</option>
		<option value="5">DHT DALLAS</option>
		<option value="6">LDR DALLAS</option>
		<option value="7">DHT LDR DALLAS</option>
	</select>
	</td></tr>
	<tr><td>Lights Output Mode:</td><td>
	<select  id="byte1" name="byte1">
		<option value="0">None</option>
		<option value="1">PWM MODE</option>
		<option value="2">PIR_MODE</option>
		<option value="3">RGB_MODE</option>
	</select>
	</td></tr>
	
	<tr><td>Others: </td><td>
	<select  id="byte2" name="byte2">
		<option value="0">None</option>
		<option value="1">CAPACITIVEE</option>
		<option value="2">RELAY</option>
		<option value="3">PRESSURE SENSOR</option>
	</select>
	</td></tr>
	
</tr></p>
<hr>
<tr>
	<td align="left" colspan="2"><hr></td>
</tr>

<tr>
	<td align="left" colspan="2">Turn on at</td>
</tr>
<tr>
	<td align="right"> Enabled:</td>
	<td><input type="checkbox" id="tonenabled" name="tonenabled"></td>
</tr>

<tr>
	<td align="right"> Time:</td>
	<td><input type="text" id="tonhour" name="tonhour" size="2" value="00">:<input type="text" id="tonminute" name="tonminute" size="2" value="00"></td>
</tr>

<tr>
	<td align="left" colspan="2">Turn off at:</td>
<tr>
	<td align="right"> Enabled:</td>
	<td><input type="checkbox" id="toffenabled" name="toffenabled"></td>
</tr>
<tr>
	<td align="right"> Time:</td>
	<td><input type="text" id="toffhour" name="toffhour" size="2" value="00">:<input type="text" id="toffminute" name="toffminute" size="2" value="00"></td>
</tr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
</table>
</form>
<hr>
<strong>You will need only ONE Gateway Node, If this is your First Node you must enabled it as Gateway..</strong>

<script>

 

window.onload = function ()
{
	load("style.css","css", function() 
	{
		load("microajax.js","js", function() 
		{
				setValues("/admin/generalvalues");
		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



</script>
)=====";


// Functions for this Page
void send_devicename_value_html()
{
		
	String values ="";
	values += "devicename|" + (String) config.DeviceName + "|div\n";
	server.send ( 200, "text/plain", values);
	Serial.println(__FUNCTION__); 
	
}

void send_general_html()
{
	
	if (server.args() > 0 )  // Save Settings
	{
		config.AutoTurnOn = false;
		config.AutoTurnOff = false;
		config.NodeMode = false;
		config.byte0 = 0;
		config.byte1 = 0;
		config.byte2 = 0;
		String temp = "";
		for ( uint8_t i = 0; i < server.args(); i++ ) {
			if (server.argName(i) == "devicename") config.DeviceName = urldecode(server.arg(i)); 
			if (server.argName(i) == "mnenabled") config.NodeMode = true; 
			if (server.argName(i) == "byte0") config.byte0 = server.arg(i).toInt(); 
			if (server.argName(i) == "byte1") config.byte1 = server.arg(i).toInt(); 
			if (server.argName(i) == "byte2") config.byte2 = server.arg(i).toInt(); 
			if (server.argName(i) == "tonenabled") config.AutoTurnOn = true; 
			if (server.argName(i) == "toffenabled") config.AutoTurnOff = true; 
			if (server.argName(i) == "tonhour") config.TurnOnHour =  server.arg(i).toInt(); 
			if (server.argName(i) == "tonminute") config.TurnOnMinute =  server.arg(i).toInt(); 
			if (server.argName(i) == "toffhour") config.TurnOffHour =  server.arg(i).toInt(); 
			if (server.argName(i) == "toffminute") config.TurnOffMinute =  server.arg(i).toInt(); 
		}
		WriteConfig();
		firstStart = true;
		ESP.restart();
	}
	server.send ( 200, "text/html", PAGE_AdminGeneralSettings ); 
	Serial.println(__FUNCTION__); 
	
	
}

void send_general_configuration_values_html()
{
	String values ="";
	values += "devicename|" +  (String)  config.DeviceName +  "|input\n";
	values += "mnenabled|" +  (String) (config.NodeMode ? "checked" : "") + "|chk\n";
	values += "byte0|" +  (String) config.byte0 + "|input\n";
	values += "byte1|" +  (String) config.byte1 + "|input\n";
	values += "byte2|" +  (String) config.byte2 + "|input\n";
	values += "tonhour|" +  (String)  config.TurnOnHour +  "|input\n";
	values += "tonminute|" +   (String) config.TurnOnMinute +  "|input\n";
	values += "toffhour|" +  (String)  config.TurnOffHour +  "|input\n";
	values += "toffminute|" +   (String)  config.TurnOffMinute +  "|input\n";
	values += "toffenabled|" +  (String) (config.AutoTurnOff ? "checked" : "") + "|chk\n";
	values += "tonenabled|" +  (String) (config.AutoTurnOn ? "checked" : "") + "|chk\n";
	server.send ( 200, "text/plain", values);
	Serial.println(__FUNCTION__); 
}
