/*
 * easy_data.h
 *
 *  Created on: 17 Jun 2019
 *      Author: m.bilge
 */

#ifndef MAIN_INCLUDE_EASY_DATA_H_
#define MAIN_INCLUDE_EASY_DATA_H_

static  char *WEBPAGE_MOISTURE =
{
						"<h3>HTTP Server</h3>"
						"<meter max= 1.0 min= 0.0 value= 0.5 high= .75 low= .25 optimum= 0.5 ></meter>"
						"<p>URL: %s</p>"
						"<p>Current Moisture: feucht</p>"
						"<p>Watertanklevel: voll</p>"
						"<p>Sun hours: viele</p>"
						"<p>Uptime: %d seconds</p>"
						"<p>Free heap: %d bytes</p>"
						"<button onclick=\"location.href='/higher'\" type='button'>"
						"Moisture Higher</button></p>"
						"<button onclick=\"location.href='/lower'\" type='button'>"
						"Moisture Lower</button></p>"
						"</div></body></html>"
};


static  char *WEBPAGE_HEAD =
{
    "HTTP/1.1 200 OK\r\n"
    "Content-type: text/html\r\n\r\n"
    "<html><head><title>Easy Grow Server</title>"
    "<style> div.main {"
    "font-family: Arial;"
    "padding: 0.01em 16px;"
    "box-shadow: 2px 2px 1px 1px #d2d2d2;"
    "background-color: #f1f1f1;}"
    "</style></head>"
    "<body><div class='main'>"
};
static  char *WEBPAGE_REDIRECT =
{
	"HTTP/1.1 200 OK\r\n"
	"Content-type: text/html\r\n\r\n"
	"<html><head>"
	"<meta http-equiv='Refresh' content='5; url=192.168.4.1' />"
	"</head></html>"
};

static  char *WEBPAGE_CONFIG =
{
    "<h2>EasyGrow Config Page</h2>"
	"<h4>Verf�gbare WLAN:</h4>"
	"<p>%s</p>"
	"<p>%s</p>"
	"<p>%s</p>"
	"<p>%s</p>"
	"<p>%s</p>"
	"<form action='/submit'>"
	"<p>Wlan-Name eingeben: <input type='text' name='SSID' /></p>"
	"<p>Passwort eingeben: <input type='password' name='PW' /></p>"
	"<input type='submit' value='Abschicken' />"
	"<input type='ap' value='AP-Modus' />"
	"</form>"
    "</div></body></html>"
};

#endif /* MAIN_INCLUDE_EASY_DATA_H_ */
