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

const char PAGE_Root[] PROGMEM = R"=====(
<!doctype html>
<meta name="viewport" content="width=device-width, initial-scale=1" />
<script src="microajax.js"></script> 
<html>
	<head>
	<title></title>
	<meta charset="utf-8" />
	</head>
	<body>
	<link rel="stylesheet" href="style.css" type="text/css" />
	It work's!
	</body>
</html>

)=====";

void sendRootPage()
{
	yield();
	if (server.args() > 0 )  // Are there any POST/GET Fields ? 
	{
		for ( uint8_t i = 0; i < server.args(); i++ ) {  // Iterate through the fields
		}
	}
	server.send ( 200, "text/html", reinterpret_cast<const __FlashStringHelper *>(PAGE_Root) );  
}
