#ifndef PAGE_EXAMPLE_H
#define PAGE_EXAMPLE_H
//
//   The EXAMPLE PAGE
//
const char PAGE_example[] PROGMEM = R"=====(
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  <h2>Welcome to SerbaAuto Node </h2>     
      <form action="" method="GET">
      <div id="mydynamicdata">Here comes the Dynamic Data in </div>   <!-- added a DIV, where the dynamic data goes to -->
      <div>-</div>
      <a href="admin.html" style="width:250px" class="btn btn--m btn--blue" >Click Here to Begin</a><br>
      <div>-</div>

      <input type="submit" style="width:273px" class="btn btn--m btn--blue" value="Reboot">
      <div>More info on http://serbaauto.com</div>
      <div>Contact us at serbaauto@gmail.com</div>
      </form>

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
#endif


void filldynamicdata()
{        
    String values ="";
    values += "mydynamicdata|" + (String) + "SerbaAuto Gateway, Millis since start: " + (String) millis() + "|div\n";   // Build a string, like this:  ID|VALUE|TYPE
    server.send ( 200, "text/plain", values);   
}

void processExample()
{        
    if (server.args() > 0 )  // Are there any POST/GET Fields ? 
    {
      LOG.println("atas");  
       for ( uint8_t i = 0; i < server.args(); i++ ) {  // Iterate through the fields
            if (server.argName(i) == "firstname") 
            {
                 // Your processing for the transmitted form-variable 
                 String fName = server.arg(i);
                 LOG.println("tengah");  
            }
        }
      //server.send ( 200, "text/html", PAGE_ESPReboot );
      LOG.println("Admin Time Out");
      AdminTimeOutCounter=0;
      LOG.println("Souliss update IP Address");
      GetIPAddress();
    }

    LOG.println("bawah");  
    server.send ( 200, "text/html", PAGE_example  ); 
}

