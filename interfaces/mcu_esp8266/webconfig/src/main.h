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
//   The MAIN PAGE
//
const char PAGE_main[] PROGMEM = R"=====(
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  <h2>Welcome to Souliss </h2>     
      <form action="" method="get">

      <hr>
      <a href="admin.html" align="center" style="width:250px" class="btn btn--m btn--blue" >Click Here to Begin</a><br>
      <hr>
      <table border="0"  cellspacing="0" cellpadding="3" style="width:310px" ></td></tr>
      <tr><td align="left">Click the check button to Reset:</td><td><input type="checkbox" id="rst" name="rst"></td></tr>
      <tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Reset">
      </table>
      </form>
      <hr>
      <p>Contact us at http://www.souliss.net/</p>
      <p>Install SoulissApp for Android from the Google Play Store to control this node</p>
      <a href="https://play.google.com/store/apps/details?id=it.angelic.soulissclient">Souliss App Download</a><br>
      <hr>



   <script>                


		window.onload = function ()
		{
			load("style.css","css", function() 
			{
				load("microajax.js","js", function() 
				{
//				    setValues("/admin/filldynamicdata");  //-- this function calls the function on the ESP      
				    setValues("/admin/rstvalues");
        });
			});
		}
		function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



   </script>

)=====";

void processMain()
{
	if (server.args() > 0 )  // Reboot Settings
	{
		config.rst = true;
		String temp = "";
		for ( uint8_t i = 0; i < server.args(); i++ ) {
			if (server.argName(i) == "rst") config.rst = false; 
			Store_Clear();
			Store_Commit();
		}

	ESP.restart();
    }

	server.send ( 200, "text/html", reinterpret_cast<const __FlashStringHelper *>(PAGE_main)); 
}

void send_reset_values_html()
{
	yield();
	String values ="";
	values += "rst|" +  (String) (config.rst ? "checked" : "") + "|chk\n";
	server.send ( 200, "text/plain", values);
}
