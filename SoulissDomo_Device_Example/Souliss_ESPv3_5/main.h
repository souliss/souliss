#ifndef PAGE_EXAMPLE_H
#define PAGE_EXAMPLE_H
//
//   The MAIN PAGE
//
const char PAGE_main[] PROGMEM = R"=====(
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  <h2>Welcome to SerbaAuto </h2>     
      <form action="" method="GET">
      <div id="mydynamicdata">Souliss Node </div>   <!-- added a DIV, where the dynamic data goes to -->
      <hr>
      <a href="admin.html" style="width:250px" class="btn btn--m btn--blue" >Click Here to Begin</a><br>
      <hr>
      <input type="submit" style="width:273px" class="btn btn--m btn--blue" value="Reboot">
      <hr>
      <a href="http://serbaauto.com"> More info on http://serbaauto.com </a>
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
    values += "mydynamicdata|" + (String) + "Souliss Node, Millis since start: " + (String) millis() + "|div\n";   // Build a string, like this:  ID|VALUE|TYPE
    server.send ( 200, "text/plain", values);   
}

void processMain()
{

    if (server.args() > 0 )  // Reboot Settings
    {

    }

    check_ESPMode();

    server.send ( 200, "text/html", PAGE_main  ); 
}

