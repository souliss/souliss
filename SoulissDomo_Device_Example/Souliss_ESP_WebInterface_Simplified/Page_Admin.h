//
//  HTML PAGE
//

const char PAGE_AdminMainPage[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<h2>Souliss Node Administration </h2>
<hr>
<a href="config.html" style="width:250px" class="btn btn--m btn--blue" >Network Configuration</a><br>
<a href="main.html"   style="width:250px"  class="btn btn--m btn--blue" >Main Interface</a><br>
<hr>
<strong> <div id="mydynamicdata"> </div> </strong> <!-- added a DIV, where the dynamic data goes to -->
<strong>Clicking Main Interface Button will leaving Access Point Mode, </strong>
<strong>Please make a note of your IP Address Node in Network Information </strong>
<p><strong>You can enter this Web Interface again by your NEW IP Address Node given from Network Information. </strong></p>

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

void filldynamicdata()
{        
    //Serial.println("Dynamic Data Run");
    String values ="";
    values += "mydynamicdata|" + (String) + "Souliss Node, Millis since start: " + (String) millis() + "|div\n";   // Build a string, like this:  ID|VALUE|TYPE
    server.send ( 200, "text/plain", values);   

}
