/*
 * easy_data.h
 *
 *  Created on: 17 Jun 2019
 *      Author: m.bilge
 */

#ifndef MAIN_INCLUDE_EASY_DATA_H_
#define MAIN_INCLUDE_EASY_DATA_H_

static  char *WEBPAGE_HEAD =
{
    "HTTP/1.1 200 OK\r\n"
    "Content-type: text/html\r\n\r\n"
    "<html><head><title>Easy Grow Server</title>"
    "<style>"
		"body{"
			"background: #F5F7F9;"
			"font-family: Helvetica, sans-serif;}"
		"div.main {"
			"padding: 0.01em 16px;"
			"border-radius: 5px;"
			"display: flex;"
			"align-content: center;"
			"justify-content: center;"
			"flex-direction: column;}"
		"div.panel {"
		  "display: flex;"
		  "background: white;"
		  "margin:5px;"
		  "box-shadow: 0px 2px 10px rgba(0, 0, 0, 0.05);"
		  "border-radius:5px;"
		  "flex-direction: column;}"
		"h3{margin-left:10px;}"
		"p{margin-left:15px;}"
		".wifiLink{cursor: pointer;}"
		".wifiLink:hover{opacity: 0.6;}"
		"input{"
		  "border-radius: 3px;"
		  "width: 300px;"
		  "height: 30px;"
		  "border:none;}"
		"button{"
		  "height: 40px;"
		  "width: 100px;"
		  "border:none;"
		  "color: white;"
		  "border-radius: 5px;"
		  "margin:10px;"
		  "background:black;}"
		"button:hover{"
		  "cursor: pointer;"
		  "box-shadow: 0px 2px 10px rgba(0, 0, 0, 0.3);}"
		"button:active{"
		  "opacity: 0.9;}"
		"button.b-lo{background:#ff5959;}"
		"button.b-me{background:#ffc400;}"
		"button.b-hi{background:#3fb500;}"
    "</style></head>"
    "<body><div class='main'>"
};

static  char *WEBPAGE_MOISTURE =
{
	"<meta http-equiv='Refresh' content='5; url=192.168.4.1' />"
	"<h2>Easy Grow</h2>"
	"<div class='panel'>"
		"<p>Gew&auml;hlte Feuchtigkeit: %d <meter max= 3.0 min= 0.0 value= %d ></meter><span id='moistureSelectedVal'></span></p>"
		"<p>Aktuelle Feuchtigkeit: <span id='mv'>%d</span></p>"
		"<p id='wl'>Wasserstand: %d</p>"
		"<p>Sonnenstunden: 12h</p>"
		"<p>Uptime: %d seconds</p>"
		"<p>Free heap: %d bytes</p>"
	"</div>"
		"<div class='button-container'>"
			"<button onclick=\"location.href='/reset'\" type='button'>Reset Wifi</button>"
			"<button onclick=\"location.href='/off'\" type='button'>Off</button>"
			"<button class='b-lo' onclick=\"location.href='/low'\" type='button'>Low</button>"
			"<button class='b-me' onclick=\"location.href='/medium'\" type='button'>Medium</button>"
			"<button class='b-hi' onclick=\"location.href='/high'\" type='button'>High</button>"
			"<button onclick=\"location.href='/pump'\" type='button'>Pumpen</button>"
		"</div>"
		"<script type='text/javascript'>"
		"var mVal = document.getElementById('mv');"
		"var wl = document.getElementById('wl');"
		"</script>"
	"</div></body></html>"
};


static  char *WEBPAGE_NEW_CONFIG =
{
    "<h2>EasyGrow Konfiguration</h2>"
	"<div class='panel'>"
		"<h3>Verf&uuml;gbare WLAN:</h3>"
			"<p class='wifiLink'>%s</p>"
			"<p class='wifiLink'>%s</p>"
			"<p class='wifiLink'>%s</p>"
			"<p class='wifiLink'>%s</p>"
			"<p class='wifiLink'>%s</p>"
	"</div>"
	"<form action='/submit'>"
	"<div class='button-container'>"
		"<form action='/submit'>"
		"<p>Wlan-Name: <input id='selectedWifi' type='text' name='SSID' /></p>"
		        "<p>Passwort eingeben: <input type='password' name='PW' /></p>"
		        "<button type='submit' value='Abschicken'>Abschicken</button>"
		        "<button onclick=\"location.href='/ap'\" type='button'>AP Mode</button>"
		      "</form>"
	"</div>"
		"<script type='text/javascript'>"
		"window.onload = function () {"
		"var elem = document.getElementsByClassName('wifiLink');"
		   "for(var i=0; i < elem.length; i++){"
		      "elem[i].addEventListener('click', function(){"
			  	  "document.getElementById('selectedWifi').value = event.target.innerHTML;"
				"});"
			"};};"
		"</script>"
   "</div></body></html>"
};

static  char *WEBPAGE_CONFIG =
{
    "<h2>EasyGrow Config Page</h2>"
	"<h4>Verfuegbare WLAN:</h4>"
	"<p>%s</p>"
	"<p>%s</p>"
	"<p>%s</p>"
	"<p>%s</p>"
	"<p>%s</p>"
	"<form action='/submit'>"
	"<p>Wlan-Name eingeben: <input type='text' name='SSID' /></p>"
	"<p>Passwort eingeben: <input type='password' name='PW' /></p>"
	"<input type='submit' value='Abschicken' />"
		"</form>"
	"<button onclick=\"location.href='/ap'\" type='button'>"
	"AP Mode</button></p>"
    "</div></body></html>"
};

#endif /* MAIN_INCLUDE_EASY_DATA_H_ */
