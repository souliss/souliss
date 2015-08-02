#ifndef PAGE_EXAMPLE_H
#define PAGE_EXAMPLE_H
//
//   The MAIN PAGE
//
const char PAGE_main[] PROGMEM = R"=====(
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  <h2>Welcome to SerbaAuto </h2>     
      <form action="" method="get">

      <div id="mydynamicdata">Souliss Node </div>   <!-- added a DIV, where the dynamic data goes to -->
      <hr>
      <a href="admin.html" align="center" style="width:250px" class="btn btn--m btn--blue" >Click Here to Begin</a><br>
      <hr>
      <table border="0"  cellspacing="0" cellpadding="3" style="width:310px" ></td></tr>
      <tr><td align="left">Click the check button to Reboot:</td><td><input type="checkbox" id="rst" name="rst"></td></tr>
      <tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Reboot">
      </table>
      </form>
      <hr>
      <p>Contact us at juanpintom@gmail.com</p>
      <p>Install SoulissApp in Google Play Store to control this node</p>
      
      <hr>
      <p>Present to You by Juan Pinto - Thanks to Dario, Lesjaw, Dom and others..</p>



   <script>                


		window.onload = function ()
		{
			load("style.css","css", function() 
			{
				load("microajax.js","js", function() 
				{
				    setValues("/admin/filldynamicdata");  //-- this function calls the function on the ESP      
				    setValues("/admin/rstvalues");
        });
			});
		}
		function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



   </script>

)=====";
#endif


void filldynamicdata()
{        
    //Serial.println("Dynamic Data Run");
    String values ="";
    values += "mydynamicdata|" + (String) + "Souliss Node, Millis since start: " + (String) millis() + "|div\n";   // Build a string, like this:  ID|VALUE|TYPE
    server.send ( 200, "text/plain", values);   

}

void processMain()
{
    
    if (server.args() > 0 )  // Reboot Settings
    {
      config.rst = true;
      String temp = "";
      for ( uint8_t i = 0; i < server.args(); i++ ) {
        if (server.argName(i) == "rst") config.rst = false; 
        LOG.println(F("EEPROM Clear, Size: "));
        LOG.println(STORE__SIZE);
        //Store_Clear();
        for(int i=0;i<STORE__SIZE;i++)
  	{	EEPROM.write(i, 255);   }
	EEPROM.commit();
      }
      //WriteConfig();
      
      ESP.restart();

    }

    server.send ( 200, "text/html", reinterpret_cast<const __FlashStringHelper *>(PAGE_main)); 
    Serial.println(__FUNCTION__);
    
}

void send_reset_values_html()
{
  String values ="";
  values += "rst|" +  (String) (config.rst ? "checked" : "") + "|chk\n";
  server.send ( 200, "text/plain", values);
  Serial.println(__FUNCTION__); 
}
