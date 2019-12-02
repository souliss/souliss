/* 
  ESP_WebConfig 

  Copyright (c) 2015 John Lassen. All rights reserved.
  This is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Latest version: 1.1.3  - 2015-07-20
  Changed the loading of the Javascript and CCS Files, so that they will successively loaded and that only one request goes to the ESP.

  -----------------------------------------------------------------------------------------------
  History

  Version: 1.1.2  - 2015-07-17
  Added URLDECODE for some input-fields (SSID, PASSWORD...)

  Version  1.1.1 - 2015-07-12
  First initial version to the public
*/

//
//  HTML PAGE
//


const char PAGE_AdminMainPage[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<h2>Souliss Node Administration</h2>
<hr>
<a href="config.html" style="width:250px" class="btn btn--m btn--blue" >Network Configuration</a><br>
<a href="notify.html" style="width:250px" class="btn btn--m btn--blue" >Notify Settings</a><br>
<a href="main.html"   style="width:250px"  class="btn btn--m btn--blue" >Home</a><br>
<a href="reboot.html"   style="width:250px"  class="btn btn--m btn--blue" >Reboot Node</a><br>
<hr>
<table width="500" border="1">
<strong> <div id="mydynamicdata"> </div> </strong>
<strong> <div id="mynodename"> </div> </strong>
<strong> <div id="mywifiapmac"> </div> </strong>
<strong> <div id="mywifissid"> </div> </strong>
<strong> <div id="mywifichan"> </div> </strong>
<strong> <div id="mywifimac"> </div> </strong>
<strong> <div id="mywifiip"> </div> </strong>
<strong> <div id="mywifisubnet"> </div> </strong>
<strong> <div id="mywifigw"> </div> </strong>
<strong> <div id="mywifisignal"> </div> </strong>
<strong> <div id="mywifibars"> </div> </strong>
<strong> <div id="mysketchsize"> </div> </strong>
<strong> <div id="myfreesize"> </div> </strong>
<strong> <div id="myflash"> </div> </strong>
<strong> <div id="mylastreboot"> </div> </strong>
<strong> <div id="mynodestat"> </div> </strong>

</table>
 <p><strong><a href="http://www.souliss.net" target=_blank>http://www.souliss.net</a> for Getting started information </p>
<p>


<script>
window.onload = function ()
{
	load("style.css","css", function() 
	{
		load("microajax.js","js", function() 
		{
		    setValues("/admin/filldynamicdata");  //-- this function calls the function on the ESP 
		

		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}

</script>

)=====";



#ifndef ASYNCWEBSERVER
void filldynamicdata()
#else
void filldynamicdata(AsyncWebServerRequest *request)
#endif
{        
#ifndef ASYNCWEBSERVER
	yield();
#endif	

//Cdj for wifi signal	
	int mybars;
	if (WiFi.RSSI() > -55) {
    mybars = 5;
  }
  else if (WiFi.RSSI() < -55 & WiFi.RSSI() >= -65) {
    mybars = 4;
  }
  else if (WiFi.RSSI() < -65 & WiFi.RSSI() >= -70) {
    mybars = 3;
  }
  else if (WiFi.RSSI() < -70 & WiFi.RSSI() >= -78) {
    mybars = 2;
  }
  else if (WiFi.RSSI() < -78 & WiFi.RSSI() > -82) {
    mybars = 1;
  }
  else {
    mybars = 0;
  }
// end wifisignal
	
	String values ="";
	values += "mydynamicdata|" + (String) + "<th scope='row'>Running time millis: </th><td>" +(String) millis() + "</td></tr>|div\n";   // Build a string, like this:  ID|VALUE|TYPE
	values += "mynodename|" + (String) + "<th scope='row'>Node Name: </th><td>" + String(Read_NodeName()) + "</td></tr>|div\n";
	values += "mywifiapmac|" + (String) + "AP Mac Address: " + WiFi.BSSIDstr() + "|div\n";
	values += "mywifissid|" + (String) + "WiFi SSID: " + WiFi.SSID()+ "|div\n";
	values += "mywifichan|" + (String) + "WiFi Channel: " + String(WiFi.channel())+ "|div\n";
	values += "mywifiip|" + (String) + "IP Address: " + WiFi.localIP().toString()+ "|div\n";
	values += "mywifisubnet|" + (String) + "Subnet Mask: " + WiFi.subnetMask().toString()+"|div\n";
	values += "mywifigw|" + (String) + "Ip Gateway: " + WiFi.gatewayIP().toString() + "|div\n";
	values += "mywifimac|" + (String) + "MAC Address: " + WiFi.macAddress()+ "|div\n";
	values += "mysketchsize|" + (String) + "Sketch Size: " + String(ESP.getSketchSize())+ " bytes |div\n";
	values += "myfreesize|" + (String) + "Free Sketch Space: " + String(ESP.getFreeSketchSpace())+ " bytes|div\n";
	values += "myflash|" + (String) + "Flash Size: " + String(ESP.getFlashChipSize())+ " bytes|div\n";
	values += "mywifisignal|" + (String) + "WiFI Signal: " + WiFi.RSSI() + " dB" + "|div\n";
	values += "mywifibars|" + (String) + "WiFi Bars: " + String(mybars) + " / 5"+ "|div\n";
	values += "mylastreboot|" + (String) + "Last Reboot Reason: " + ESP.getResetReason()+ "|div\n";
	

	#ifndef ASYNCWEBSERVER
	server.send ( 200, "text/plain", values);   
#else
	request->send ( 200, "text/plain", values);
#endif
}
