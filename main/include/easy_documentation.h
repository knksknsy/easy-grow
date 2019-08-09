#ifndef MAIN_INCLUDE_EASY_DOCUMENTATION_H_
#define MAIN_INCLUDE_EASY_DOCUMENTATION_H_
static  char *WEBPAGE_DOCUMENTATION = {

"<h3 id='11stromversorgung'>1.1 [Stromversorgung]</h3>"

"<p>Die Platine wird über einen Micro USB-B Anschluss mit Strom versorgt. Dabei sind der Pin 1 (VBUS) und der Pin 5 (GND) über einen Kippschalter mit dem Microkontroller verbunden. Pin 2,3,4 als Datenleitungen wurden nicht verwendet, da die RX und TX Pins des NodeMCU Boards für das Interface genutzt werden."
"Zum FLashen ist daher der Microkontroller abnehmbar.</p>"

"<p>Neben der kompletten Abschaltung des Systems über den Kippschalter, wird diese Schaltung ebenfalls für den Pumpenbetrieb benötigt."
"Mit einem Verbauch von bis zu 8 Watt könnte der Power Regulator des NodeMCU je nach Betriebsart überlastet werden und sich zu hoch erhitzen.</p>"

"<p>Mit der Betriebsart über den Vin-Pin (Vin-PIn, Micro-USB-Anschluss, sowie 3.3V-Pin werden unterstützt) lassen sich nur um die 800mA aus der V-Pins beziehen. Daher wird die Pumpe über die von uns entwickelte Platine mit Strom versorgt."
"Zur Ein- und Abschaltung wird ein Mosfet IRLZ44N genutzt.  Die 3.3V der GPIO-Pins reichen hierbei zum Durchschalten des Mosfets aus. Der direkte Betrieb über die GPIO-Pins ist nicht möglich, da Vebraucher maximal 20mA über diese beziehen dürfen. Zudem wäre die Pumpleistung bei 3.3V zu schwach. </p>"

"<p><a name='stromvebrauch'></a></p>"

"<h4 id='111stromvebrauch'>1.1.1 Stromvebrauch</h4>"

"<p>Der Stromvebrauch des Microkontrollers schwankt stark in Abhängigkeit zu dem Betriebsmodus. Funktioniert er als Accesspoint, während keine LED leuchtet, liegt der Vebrauch bei 108mA. Ist er mit einem WLAN Netzwerkverbunden benötigt die Schaltung 87mA."
"Pro eingeschaltete LED kommen 13mA (rote LED,mit 2V Flussspannung und 100 Ohm Vorwiderstand) hinzu. Die Pumpe verbraucht im Schnitt 1.2A.</p>"

"<p><a name='batteriebetrieb'></a></p>"

"<h4 id='112batteriebetrieb'>1.1.2 Batteriebetrieb</h4>"

"<p>Für Evaluation des Batteriebetriebs muss zunächst der Verbrauch in Amperestunden ermittelt werden. Als typisches Beispiel wird daher angenommen, dass:</p>"

"<ol>"
"<li>Der Kontroller mit einem Wlan verbunden ist (87mA)</li>"

"<li>Neben den zwei TankLEDs eine weitere LED eingeschalten ist (39mA)</li>"

"<li>Die Pumpe 20 Sekunden pro Tag pumpt (0.012 mA)</li>"
"</ol>"

"<p>Ein 5V Akku mit 2000mAh wäre schon nach 15 Stunden leer.</p>"

"<p>Folgende Möglichkeiten könnte für den Batteriebetrieb in den Betracht gezogen werden:</p>"

"<ul>"
		"</div></body></html>"

};
#endif /* MAIN_INCLUDE_EASY_DOCUMENTATION_H_ */
