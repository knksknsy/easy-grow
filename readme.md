# Easy Grow Dokumentation

Easy Grow ist ein automatisches Bewässerungssystem für Pflanzen. Das System bietet drei verschiedene, vordefinierte Einstellungsmöglichkeiten für den Grad der Erdfeuchtigkeit. Diese Einstellungen können direkt über das System oder über ein Heimnetzwerk mittels einer Web-Applikation vorgenommen werden.
Das Bewässerungssystem hält die Erdfeuchtigkeit über den ganzen Tag feucht und unterstützt somit die Langlebigkeit der Pflanzen.

<div style="width:100%; background:red; margin:0 20px 0 20px; text-align:center;">
<img src="images/mockup.png" width="100%"></div>
</div>


## Externe Komponenten
Dieses Projekt nutzt Teile des DNS Servers aus der freien Quelle ```libesphttpd```.
In der Datei ```easy_dns.c``` ist die Lizenz und der Author vermerkt, weitere Informationen sowie das Original finden sich unter  https://github.com/Spritetm/libesphttpd/blob/master/util/captdns.c.

Für den HTTP-Server wird zusätzlich die netconn API von LwIP genutzt (https://www.nongnu.org/lwip/2_0_x/index.html)
## Inhaltsverzeichnis
- [1. Features](#features)
- [2. Erstellen der Dokumentation mit make](#make_documentation)
- [3. ESP8266 Mikrocontroller](#esp8266)
    * [3.1 NodeMCU Plattform](#nodemcu)
    * [3.2 Digitale I/O](#digital_io)
        + [3.2.1 Spannungs- und Strombegrenzungen](#restrictions)
        + [3.2.2 Verwendbare Pins](#usable_pins)
        + [3.2.3 Boot-Modi](#boot_modes)
        + [3.2.4 Interne Pull-Up/-Down-Widerstände](#pull_up_down)
    * [3.3 Pulsweitenmodulation](#pwm)
    * [3.4 Analogeingang](#analog_input)
    * [3.5 Kommunikation](#communication)
        + [3.5.1 Serial](#serial)
        + [3.5.2 I2C](#i2c)
        + [3.5.3 SPI](#spi)
    * [3.6 NodeMCU GPIO-Mapping auf ESP8266](#gpio_mapping_esp)
- [4. Aufsetzen der Softwareumgebung](#sw_env) 
    * [4.1 ESP8266 Toolchain-Setup mit Docker](#tool_docker)
        + [4.1.1 Installation von Docker](#inst_docker)
            - [4.1.1.1 MacOS](#inst_docker_mac)
            - [4.1.1.2 Windows](#inst_docker_win)
        + [4.1.2 Bauen des Docker-Images](#build_docker)
        + [4.1.3 Freigeben des seriellen Ports vom Hosts zum Docker-Container](#serial_port)
            - [4.1.3.1 MacOS](#serial_port_mac)
            - [4.1.3.2 Windows](#serial_port_windows)
        + [4.1.4 Ausführen des Docker-Containers](#cont_docker)
            - [4.1.4.1 MacOS](#cont_docker_mac)
            - [4.1.4.2 Windows](#cont_docker_win)
    * [4.2 ESP8266 Toolchain-Setup auf einer lokalen Maschine (Nur MacOS)](#local)
- [5. Konfiguration des Espressif IoT Development Frameworks](#idf_config)
    * [5.1 Eclipse IDE](#eclipse)
    * [5.2 ESP IDF](#esp_idf)
- [6. File includes mit make](#make)
    * [6.1 Flash-Argumente](#flash_argumente)
- [7. Bauen, Flashen und Monitoring](#build)
- [8. Bauen und Flashen des LED-Beispiels](#led_example)
- [9. ESP8266 RTOS SDK](#rtos_sdk)
    * [9.1 Überblick](#rtos_overview)
    * [9.2 Konzepte](#rtos_concept)
    * [9.3 Beispiel Projekt](#example_project)
    * [9.4 Tasks](#rtos_xtasks)
        + [9.4.1 Implementierung](#rtos_xtasks_impl)
        + [9.4.2 Verwendung im Projekt](#rtos_xtasks_usage)
    * [9.5 GPIO](#rtos_gpio)
        + [9.5.1 Konfiguration](#rtos_gpio_conf)
        + [9.5.2 Interrupt Service Routine](#rtos_gpio_isr)
        + [9.5.3 Analogeingang](#rtos_gpio_analog)
    * [9.6 Timer](#rtos_timer)
    * [9.7 WiFi](#rtos_wifi)
        + [9.7.1 Event Handler](#rtos_wifi_event)
        + [9.7.2 Initialisierung und Einstellung des Modus](#rtos_wifi_init)
        + [9.7.3 Konfiguration und Verbindung](#rtos_wifi_connect)
        + [9.7.4 Nach verfügbaren Access Points scannen](#rtos_wifi_scan)
    * [9.8 HTTP Server](#rtos_http_server)
        + [9.8.1 Eine (TCP-) Verbindung eröffnen](#rtos_http_server_connect)
        + [9.8.2 Eingehende Verbindungsanfragen abfangen](#rtos_http_server_recv)
        + [9.8.3 Daten über eine TCP-Verbindung senden](#rtos_http_server_send)
        + [9.8.4 Verbindungen beenden](#rtos_http_server_close)
    * [9.9 Schreiben und Lesen des Flash-Speichers](#rtos_flash)
- [10. Easy Grow Projekt](#easy_grow)
    * [10.1 Hardware-Komponenten](#eg_hardware)
    * [10.2 Stromversorgung](#stromversorgung)
        + [10.2.1 Stromverbrauch](#stromvebrauch)
        + [10.2.2 Batteriebetrieb](#batteriebetrieb)
    * [10.3 GPIO-Mapping](#eg_gpio)
    * [10.4 Schaltbild](#eg_circuit)
    * [10.5 Funktionsweise](#eg_functionality)
        + [10.5.1 Hardware-Logik](#eg_func_hw_logic)
            - [10.5.1.1 Einstellung der Erdfeuchtigkeit](#eg_func_hw_logic_set_moisture)
            - [10.5.1.2 Messung der Erdfeuchtigkeit](#eg_func_hw_logic_read_moisture)
            - [10.5.1.3 Bewässerung der Pflanze](#eg_func_hw_logic_watering)
            - [10.5.1.4 Aufzeichnung der Sonnenstunden](#eg_func_hw_logic_sun_hours)
            - [10.5.1.5 Programmablaufplan](#eg_func_hw_logic_pap)
        + [10.5.2 Access-Point / WiFi-Manager](#eg_func_ap)
            - [10.5.2.1 Start mit gespeicherten Credentials](#eg_func_ap_with_creds)
            - [10.5.2.2 Start ohne gespeicherte Credentials](#eg_func_ap_without_creds)
        + [10.5.3 Webserver](#eg_func_server)
            - [10.5.3.1 Starten des Webserver-Tasks](#eg_func_server_start)
            - [10.5.3.2 Webpage vorbereiten und senden](#eg_func_server_page)
            - [10.5.3.3 Interaktionen auf der Webpage](#eg_func_server_interact)
        + [10.5.4 Easy_DNS](#eg_func_dns)   
        + [10.5.5 Bedienung der Weboberflächen](#eg_func_server_gui)    
            - [10.5.5.1 Setupwebseite](#eg_func_server_gui-initial)
            - [10.5.5.2 Access-Point Webseite](#eg_func_server_gui-ap)
    * [10.6 Git / CICD](#git)
        - [10.6.1 Continuous Integration](#git-cicd)
    * [10.7 Produktdesign](#eg_design)
- [11. Changelog](#changelog)
<!-- toc -->


<a name="features"></a>
## 1. Features

- Gießen der Pflanze (Automatisiert und Manuell)

- Einstellen der gewünschten Feuchtigkeit

<img src="images/crop_states.gif"  width="500">   

- Verbinden zum heimischen WLAN oder Steuerung über eigenes WiFi-Netzwerk

<img src="images/wifi_all.gif" width="500">
  
- Darstellung einer Übersichtswebseite mit gewünschter und derzeitiger Feuchtigkeit</p>

<img src="images/wifi_setup.gif" width="500">


<a name="make_documentation"></a>
## 2. Erstellen der Dokumentation mit 'make'

```make documentation``` bietet die Möglichkeit die aktuelle Dokumentation aus der Datei ```readme.md``` 
in ein HTML-File umzuwandeln und diese im Anschluss auf einer Webseite des ESP anzuzeigen. 
Für die Erstellung des HTMLs wird [Pandoc](https://pandoc.org/) benötigt. Der Benutzer kann sich somit 
die aktuelle Dokumentation des Projekts in dem produktiven System anzeigen lassen. 
Jedoch werden Bilder dabei nicht abgebildet. Leider war das erstellte Dokument im Fall unserer Dokumentation viel zu groß
für die geringe Speichergröße des ESP was zu einem Overflow und Absturz führte. Daher wird über die Seite 'Dokumentation' 
nun auf dieses GitLab Repository verwiesen.

<a name="esp8266"></a>
## 3. ESP8266 Mikrocontroller

Der ESP8266 ist ein System on a Chip (SoC), hergestellt von der chinesischen Firma Espressif. 
Es besteht aus einer Tensilica L106 32-Bit Mikrocontroller-Einheit (MCU) und einem Wi-Fi-Transceiver. Es verfügt über 11 GPIO-Pins (General Purpose Input/Output Pins) und einen Analogeingang. Es kann wie jeder andere Mikrocontroller programmiert werden.

Die Wi-Fi-Kommunikation kann genutzt werden, um sich mit einem Heimnetzwerk zu verbinden, einen Webserver mit echten Webseiten zu hosten und Smartphones mit ihm verbinden zu lassen.

<a name="nodemcu"></a>
### 3.1 NodeMCU Plattform

ESP8266 ist nur der Name des Chips, viele Unternehmen haben ihre eigenen Boards entwickelt, die diesen Chip verwenden, so dass es viele verschiedene EESP8266 Boards auf dem Markt gibt. In diesem Projekt wurde die NodeMCU Plattform verwendet und im Weiteren die Eigenschaften und Funktionalitäten näher beschrieben.

Die Features des NodeMCU Development Boards sind folgende:
-	“USB-to-Serial Converter” zur Programmierung
-	3,3 V-Regler für die Stromversorgung
-	On-Board-LEDs zum Debuggen
-	Spannungsteiler zur Skalierung des Analogeingangs
-	Einen Flash-Speicher mit 4 MB Kapazität

| __GPIO__ | __3,3V Vreg__ | __USB-to-Serial__ | __Auto-Reset__ | __Auto-Program__ | __Flash__      | __ADC Range__ |
| :---: |  :---:     | :---:          | :---:       | :---:         | :---:       | :---:      |
| 11   | ✓         | ✓             | ✓          | ✓            | 4MB (32Mb) | 0-3,3 V   |

Dieses Board wird im Gerätemanager (Windows) oder in ```lsusb``` (Linux) angezeigt, sobald es angeschlossen wird. Es verfügt über einen integrierten 3,3 V-Regler und kann direkt über USB programmiert werden, so dass keine externen Komponenten benötigt werden, um es zu betreiben.

Der ESP8266 sollte mit maximal 3,3 V betrieben werden, da es bei einem 5V-Netzteil zerstört wird. Die I/O-Pins des ESP8266 sind nicht 5 V tolerant und werden ebenfalls bei einer 5 V Spannung zerstört. Der ESP8266 kann zudem nur 12 mA pro Ausgangspin erzeugen oder absenken.

Des Weiteren teilt der ESP8266 die Systemressourcen und die CPU-Zeit zwischen ihren „Sketches“ und dem Wi-Fi-Treiber. Auch Features wie Pulsweitenmodulation (PWM), Interrupts oder I2C werden in der Software emuliert.

<a name="digital_io"></a>
### 3.2 Digitale I/O

Der ESP8266 verfügt über digitale Ein-/Ausgangspins (I/O oder GPIO, General Purpose Input/Output Pins). Sie können als digitale Eingänge zum Lesen einer digitalen Spannung oder als digitale Ausgänge, entweder 0 V (Senkstrom) oder 3,3 V (Quellenstrom), verwendet werden.

<a name="restrictions"></a>
#### 3.2.1 Spannungs- und Strombegrenzungen

Der ESP8266 ist ein 3,3 V Mikrocontrollern, so dass seine GPIOs auch mit 3,3 V arbeiten. Die Pins sind nicht 5 V tolerant, wenn mehr als 3,6 V auf einen Pin angewendet werden, wird der Chip zerstört. Der maximale Strom, der von einem einzelnen GPIO-Pin bezogen werden kann, beträgt 12 mA.

<a name="usable_pins"></a>
#### 3.2.2 Verwendbare Pins

Der ESP8266 hat 17 GPIO-Pins (0-16), von denen jedoch nur 11 verwendet werden können, da 6 Pins (GPIO 6-11) zum Anschluss des Flash-Speicherchips verwendet werden. Dies ist der kleine 8-beinige Chip direkt neben dem ESP8266. Wenn diese Pins verwendet werden, kann dieser Zugriff das Programm zum Absturz bringen.

GPIO 1 und 3 werden als TX und RX der seriellen Hardware-Schnittstelle (UART) verwendet, so dass sie in den meisten Fällen nicht als normale I/O beim Senden/Empfangen serieller Daten verwendet werden können.

<a name="boot_modes"></a>
#### 3.2.3 Boot-Modi

Einige I/O-Pins haben eine spezielle Funktion beim Booten. Sie wählen einen von 3 Boot-Modi:

| __GPIO15__ | __GPIO0__ | __GPIO2__ | __Modus__                      |
| :---   | :---  | :---  | :---                       |
| 0 V    | 0 V   | 3,3 V | UART Bootloader            |
| 0 V    | 3,3 V | 3,3 V | Boot Sketch<br>(SPI Flash) |
| 3,3 V  | X     | X     | SDIO Modus                 |

Diese Bedingungen werden erfüllt, in dem zusätzliche Widerstände durch den Leiterplattenhersteller des NodeMCUs eingebaut wurden. Dies hat jedoch einige Auswirkungen:

- GPIO15 ist immer low gezogen, so dass der interne Pull-Up-Widerstand nicht verwendet werden kann. Dies muss beachtet werden, sobald der GPIO 15 als Eingang verwendet wird um einen Schalter zu lesen oder an ein Gerät mit einen Open-Collector-Ausgang oder einen Open-Drain-Ausgang angeschlossen wird, wie z.B. I2C.
- GPIO0 wird im Normalbetrieb auf high gezogen, so dass es nicht als Hi-Z-Eingang verwendet werden kann.
- GPIO2 kann beim Booten nicht auf low sein, so dass kein Schalter daran angeschlossen werden kann

<a name="pull_up_down"></a>
#### 3.2.4 Interne Pull-Up/-Down-Widerstände

GPIO 0-15 haben alle einen eingebauten Pull-Up-Widerstand. GIPO 16 hat einen eingebauten Pull-Down-Widerstand.

<a name="pwm"></a>
### 3.3 Pulsweitenmodulation

Der ESP8266 unterstützt keine Hardware-PWM, jedoch wird Software-PWM auf allen digitalen Pins unterstützt. Der Standard-PWM-Bereich beträgt 10 Bit bei 1 kHz. Der Bereich kann aber bis zu 14 Bit bei 1 kHz verändert werden.

<a name="analog_input"></a>
### 3.4 Analogeingang

Der ESP8266 verfügt über einen einzigen Analogeingang mit einem Eingangsbereich von 0-1 V. Wenn eine Spannung von 3,3 V geliefert wird, kann der Chip beschädigt werden. Der NodeMCU hat einen integrierten resistiven Spannungsteile, um einen Bereich von 0-3,3 V zu erhalten. Es kann zudem ein Trimpot als Spannungsteiler verwendet werden. Der ADC (Analog-Digital-Wandler) hat eine Auflösung von 10 Bit.

<a name="communication"></a>
### 3.5 Kommunikation

<a name="serial"></a>
#### 3.5.1 Serial

Der ESP8266 verfügt über zwei Hardware-UARTS (Serielle Schnittstellen):

UART0 an den Pins 1 und 3 (TX0 bzw. RX0) und UART1 an den Pins 2 und 8 (TX1 bzw. RX1), jedoch wird GPIO 8 zum Anschluss des Flash-Speicherchips verwendet. Das bedeutet, dass UART1 nur Daten übertragen kann.

UART0 hat auch eine Hardware-Flusskontrolle an den Pins 15 und 13 (RTS0 bzw. CTS0). Diese beiden Pins können auch als alternative TX0- und RX0-Pins verwendet werden.

<a name="i2c"></a>
#### 3.5.2 I2C

Der ESP hat keine Hardware TWI (Two Wire Interface), ist aber in Software implementiert. Das bedeutet, dass so ziemlich alle zwei digitalen Pins verwendet werden können. Standardmäßig verwendet die I2C-Bibliothek Pin 4 als SDA und Pin 5 als SCL. Die maximale Geschwindigkeit beträgt ca. 450 kHz.

<a name="spi"></a>
#### 3.5.3 SPI

Der ESP8266 verfügt über einen SPI-Anschluss, der dem Benutzer zur Verfügung steht, der als HSPI bezeichnet wird. Es verwendet GPIO14 als CLK, 12 als MISO, 13 als MOSI und 15 als Slave Select (SS). Es kann sowohl im Slave- als auch im Master-Modus (in der Software) verwendet werden.

<a name="gpio_mapping_esp"></a>
### 3.6 NodeMCU GPIO-Mapping auf ESP8266

<img src="images/nodemcu.png" alt="NodeMCU GPIO-Mapping auf ESP8266">

![](https://via.placeholder.com/15/00FF00/808080?text=+) = Zur Verwendung empfohlen

![](https://via.placeholder.com/15/FFCC00/808080?text=+) = Kann zu unerwartetes Verhalten führen (vor allem beim Booten)

![](https://via.placeholder.com/15/FF0000/808080?text=+) = Nicht zur Verwendung empfohlen

| __Label__ | __GPIO__ | __Input__          | __Output__                    | __Bemerkung__                                                        |
| :---  | :--- | :---           | :---                      | :---                                                             |
| D0    | 16   | Kein Interrupt | Kein PWM oder I2C Support | HIGH beim Boot.<br>Wird verwendet um vom Deep-Sleep aufzuwecken. |
| D1    | 5    | ![](https://via.placeholder.com/15/00FF00/808080?text=+) | ![](https://via.placeholder.com/15/00FF00/808080?text=+) | Oft als SCL (I2C) verwendet.  |
| D2    | 4    | ![](https://via.placeholder.com/15/00FF00/808080?text=+) | ![](https://via.placeholder.com/15/00FF00/808080?text=+) | Oft als SCL (I2C) verwendet.  |
| D3    | 0    | Pulled UP | ![](https://via.placeholder.com/15/00FF00/808080?text=+) | Verbunden mit dem FLASH-Taster.<br>Boot schlägt fehl wenn auf LOW gezogen. |
| D4    | 2    | Pulled UP | ![](https://via.placeholder.com/15/00FF00/808080?text=+) | HIGH beim Boot.<br>Mit dem On-Board-LED verbunden.<br>Boot schlägt fehl wenn auf LOW gezogen. |
| D5    | 14    | ![](https://via.placeholder.com/15/00FF00/808080?text=+) | ![](https://via.placeholder.com/15/00FF00/808080?text=+) | SPI (SCLK) |
| D6    | 12    | ![](https://via.placeholder.com/15/00FF00/808080?text=+) | ![](https://via.placeholder.com/15/00FF00/808080?text=+) | SPI (MISO) |
| D7    | 13    | ![](https://via.placeholder.com/15/00FF00/808080?text=+) | ![](https://via.placeholder.com/15/00FF00/808080?text=+) | SPI (MOSI) |
| D8    | 15    | Pulled to GND | ![](https://via.placeholder.com/15/FFCC00/808080?text=+) | SPI (CS)<br>Boot schlägt fehl wenn auf HIGH gezogen. |
| RX    | 3     | ![](https://via.placeholder.com/15/FFCC00/808080?text=+) | ![](https://via.placeholder.com/15/FF0000/808080?text=+)<br>RX pin | HIGH beim Boot. |
| TX    | 1     | ![](https://via.placeholder.com/15/FF0000/808080?text=+)<br>TX pin | ![](https://via.placeholder.com/15/FFCC00/808080?text=+) | HIGH beim Boot.<br>Debug-Ausgabe beim Boot.<br>Boot schlägt fehl wenn auf LOW gezogen. |
| A0    | ADC0     | ![](https://via.placeholder.com/15/00FF00/808080?text=+)<br> | ![](https://via.placeholder.com/15/FF0000/808080?text=+) | Analog Input |

<a name="sw_env"></a>
## 4. Aufsetzen der Softwareumgebung

Das Projekt "Easy Grow" wurde mittels des ESP8266 RTOS Software Development Kits (ESP-IDF Style) entwickelt.
In diesem Kapitel wird der Setup der Software-Umgebung beschrieben.
Der Setup bezieht sich auf die Installation der Toolchain, um Applikationen für den ESP8266 Chip zu bauen, sowie die Installation des ESP8266 RTOS SDKs, welche die API für den ESP8266 und Scripte für den Betrieb der Toolchain beinhaltet. Das ESP8266 RTOS SDK basiert auf das Real-Time-Betriebssystem FreeRTOS für Embedded-Devices. Die Grundlagen hierfür werden im Kapitel [9. ESP8266 RTOS SDK](#rtos_sdk) behandelt.

Um Anwendungen für ESP8266 zu entwickeln, wird folgendes benötigt:

- PC, der mit einem Windows-, Linux- oder Mac-Betriebssystem ausgestattet ist.
- Toolchain zur Entwicklung der Anwendung für ESP8266.
- ESP8266 RTOS SDK, das die API für ESP8266 und die Toolchain enthält.
- Das ESP8266-Board (NodeMCU) selbst und ein USB-Kabel zum Anschluss an den PC.

<img src="images/what-you-need.png" alt="" width="500">

Es stehen zwei Setup-Möglichkeiten zur Verfügung um die ESP8266 Software-Umgebung aufzusetzen:

1. Mittels eines Docker-Images (Linux Ubuntu 16.04 64bit)
2. Installation auf einer lokalen Maschine mittels eines Scripts (Nur MacOS)

Für die Installation wird Docker empfohlen.

<a name="tool_docker"></a>
### 4.1 ESP8266 Toolchain-Setup mit Docker

<a name="inst_docker"></a>
#### 4.1.1 Installation von Docker

<a name="inst_docker_mac"></a>
##### 4.1.1.1 MacOs

1. Brew installieren: https://brew.sh
2. Docker Engine und Docker-Machine installieren:<br/>```$ brew install docker```
3. Cask zum Brew Tap hinzufügen:<br/>```$ brew tap caskroom/cask```
3. Docker Client installieren:<br/>```$ brew cask install docker```
4. Optional: Falls Virtualbox nach dem 2. Schritt fehlt. Virtualbox installieren mit:<br/>```$ brew cask install virtualbox```

<a name="inst_docker_win"></a>
##### 4.1.1.2 Windows

Folge den Anweisungen auf https://docs.docker.com/docker-for-windows.

<a name="build_docker"></a>
#### 4.1.2 Bauen des Docker-Images

Das Docker-Image muss nur einmal gebaut werden. Mit ```$ docker images``` kann überprüft werden, ob das Image bereits installiert wurde.

Das Image muss nicht installiert werden, falls die Zeile ```docker-esp8266``` im Terminal ausgegeben wird.

Falls ```docker-esp8266``` im Terminal nicht erscheint, müssen die folgenden Schritte ausgeführt werden:

1. Wechsle das Verzeichnis indem sich die ```Dockerfile``` befindet:<br/>```$ cd <project_path>/docker```
2. Baue das Docker-Image: ```$ docker build -t docker-esp8266 .```

Im 2. Schritt wird ein Ubuntu-Image gebaut und die vorausgesetzten Packages für den ESP8266 Toolchain installiert. Der Befehl führt nach der Ausführung einen Script aus, der die Toolchain installiert.

<a name="serial_port"></a>
#### 4.1.3 Freigeben des seriellen Ports vom Hosts zum Docker-Container

<a name="serial_port_mac"></a>
##### 4.1.3.1 MacOS

Für MacOS muss der "CP210x USB to UART Bridge VCP" Treiber installiert werden.

Lade den Treiber herunter und folge den Anweisungen auf der Silicon Labs Website (https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers).

Überprüfe ob der Treiber erfolgreich installiert wurde. Die folgende Zeile ```$ ls /dev/tty.*``` sollte ```/dev/tty.SLAB_USBtoUART``` ausgeben.

Die nächsten Schritte zeigen, wie ein serieller Port vom Host an den Docker-Container freigegeben werden kann.
Hierfür muss zunächst ein Virtualbox Treiber für die Docker-Maschine erzeugt werden.

1. ```$ docker-machine create --driver virtualbox default```
2. Überprüfe ob der Treiber erzeugt wurde: ```$ docker-machine ls```
3. Stoppe die Docker-Maschine: ```$ docker-machine stop```
4. Jetzt kann die VM konfiguriert und der serielle Port exportiert werden. Öffne die Virtualbox Anwendung. Wähle die 'default' VM aus und klicke auf den 'Settings' Button, um die USB Einstellungen vorzunehmen.
<img src="images/virtualbox_default_settings.png" width="500" alt="Virtualbox 'default' VM Einstellungen">

5. Klicke auf 'Ports' und anschließend auf den 'USB' Tab. Aktiviere die 'Enable USB Controller' Checkbox. Wähle 'USB 2.0 (EHCI) Controller' aus. Füge einen USB Filter hinzu (USB Icon mit grünem Plus Symbol). Wähle den 'Silicon Labs CP2102 USB to UART Bridge Controller [0100]' Treiber aus.
<img src="images/virtualbox_port_usb.png" width="500" alt="Virtualbox serieller Port">

6. Falls USB 2.0 nicht ausgewählt werden kann, muss der 'Oracle VM VirtualBox Extension Pack' installiert werden. Die Anweisungen für die Installation befindet sich hier: https://www.virtualbox.org/wiki/Downloads
7. Starte die Docker-Maschine mit: ```$ docker-machine start```
8. Es müssen einige Umgebungsvariablen gesetzt werden, damit Docker die VM verwendet anstelle des nativen Modus. Das ```$ docker-machine env``` Kommando gibt die notwendigen Schritte für das Setzen der Variablen an. Führe folgenden Befehl aus um diese zu setzen: ```$ eval "$(docker-machine env default)"```

Der serielle Port des Hosts ist nun vom Docker-Container aus ansprechbar.

<a name="serial_port_win"></a>
##### 4.1.3.2 Windows

Leider gibt es bis auf Weiteres keine Unterstützung der 'Device Assignment' und 'Sharing Workloads' in Hyper-V-isolierte Windows Containern. 

<a name="cont_docker"></a>
#### 4.1.4 Ausführen des Docker-Containers

Stelle sicher, dass Docker installiert, das ```docker-esp8266``` Image gebaut und der serielle Port des Hosts an den Docker-Container freigegeben wurde, bevor der Docker container gestartet werden soll.

Es gibt zwei Möglichkeiten einen Container vom ```docker-esp8266``` Image zu instanziieren:

1. Über den ```docker.sh``` Script.<br/>(Nur MacOS)
2. Über den ```docker run``` Befehl.<br/>(Empfohlen für Windows)

<a name="cont_docker_mac"></a>
##### 4.1.4.1 MacOS

1. Wechsle in den ```docker``` Ordner im Projektverzeichnis: ```$ cd <project_path>/docker```
2. Starte den Container mithilfe des Scripts: ```$ ./docker.sh```. Dieses Script startet einen Container für das ```docker-esp8266``` Image. Das Script mountet zudem das Projektverzeichnis in das ```/easy-grow``` Verzeichnis des Containers. Zudem exportiert es den Hosts ```/dev/ttyUSB0``` Port an den Port ```/dev/ttyUSB0``` des Containers.

<a name="cont_docker_win"></a>
##### 4.1.4.2 Windows

Führe folgendes Befehl im Projektverzeichnis ```<project_path>``` aus:<br/>
```docker run -ti --rm --name esp8266 -v <project_path>:/easy-grow docker-esp8266 /bin/bash```

<a name="local"></a>
### 4.2 ESP8266 Toolchain-Setup auf einer lokalen Maschine (Nur MacOS)

Wechsle in das ```setup``` Verzeichnis im Projektverzeichnis um die Softwareumgebung für den ESP8266 aufzusetzen.
Dort befindet sich der ```initial_setup.sh``` Script, der folgendes automatisch aufsetzt:

- Die Toolchain um Applikationen für den ESP8266 zu bauen.
- Die ESP8266 RTOS SDK, die die API und Scripte beinhaltet um die Toolchain zu betreiben.

Übergebe das ```--dir <path>``` Argument, um die Softwareumgebung in einem beliebigen Verzeichnis zu installieren. 
Wird dieses Argument nicht gesetzt, wird standardmäßig die Softwareumgebung im ```ESP``` Ordner des Projektverzeichnis installiert.

Die Toolchain befindet sich im Pfad ```<path>/ESP/xtensa-lx106-elf``` wohingegen die SDK sich im Pfad ```<path>/ESP/ESP8266_RTOS_SDK``` befindet.

Öffne ein neues Terminalfenster und füge den folgende Befehl ein um das Setup zu starten:
- Wechsle in das ```setup``` Verzeichnis im Projektpfad:<br>```$ cd setup```
- Führe den Script als ein User aus (nicht als Root):<br>```$ ./initial-setup.sh```
- Warte bis das Setup beendet ist.
- Vergiss nicht dein ```bash_profile``` nach dem Setup zu sourcen:<br> ```$ source ~/.bash_profile```

<a name="idf_config"></a>
## 5. Konfiguration des Espressif IoT Development Frameworks

<a name="eclipse"></a>
### 5.1 Eclipse IDE

Um eine gemeinsame Enwicklungsumgebung innerhalb des Teams zu schaffen, wurde die Eclipse IDE gewählt. Diese bietet
 eine grafische Oberfläche zum Schreiben, Kompillieren und Debuggen von ESP8266_RTOS_SDK Projekten in C.
Nach dem Download des plattformabhängig richtigen Installers von https://www.eclipse.org/downloads/ sollte beim ersten Start "Eclipse for C/C++ Development" ausgewählt werden.
Um die Entwicklunsumgebung initial zu konfigurieren sind die folgenden Schritte notwendig, welche einer offiziellen ausführlichen Anleitung von Espressif unter 
https://docs.espressif.com/projects/esp8266-rtos-sdk/en/latest/get-started/eclipse-setup.html entnommen wurden.

***Die folgenden Schritte beschreiben die in diesem Projekt angewandte Nutzung unter Mac/OSx. 
Eine ausführliche Anleitung zur Konfiguration unter Windows kann unter diesem Link gefunden werden: https://docs.espressif.com/projects/esp8266-rtos-sdk/en/latest/get-started/eclipse-setup-windows.html#eclipse-windows-setup***

#### **Projekt importieren/anlegen**

1. In Eclipse über File -> Import entweder das idf-Template von github oder eines der Beispiele im Unterverzeichnis ESP8266_RTOS_SDK/examples auswählen.
2. Im aufpoppenden Dialog "C/C++" und -> "Existing code as Makefile Projekt" auswählen und "Next" klicken
3. Auf der nächsten Seite "Existing Code Location" als Verzeichnis auswählen. Hier sollte nicht der Pfad zum ESP8266_RTOS_SDK Verzeichnis selbst stehen, welcher erst später gebraucht wird. Das angegebene Verzeichnis sollte eine Datei namens "Makefile" enthalten.
4. Auf der selben Seite unter "Toolchain for Indexer Settings" "Cross GCC auswählen", dann "Finish" klicken.


#### **Projekt in Eclipse konfigurieren**
Das neue Projekt sollte nun im Projekt-Explorer von Eclipse angezeigt werden. 

1. Auf das neu importierte Projekt im Explorer rechts klicken und "Properties" auswählen.
2. Unter "Environment" auf "C/C++ Build" klicken und "Add.." auswählen. 
3. ``BATCH_BUILD`` mit dem Wert 1 eintragen`
4. Erneut "Add.." klicken und ``IDF_PATH`` hinzufügen. Der Wert sollte der vollständige Pfad sein, in dem das ESP8266_RTOS_SDK installiert ist.
5. Die Umgebungsvariable ``PATH`` bearbeiten und hier den Pfad zur Xtensa-Toolchain angeben (Welche im Rahmen des ESP8266_RTOS_SDK-Setups installiert wurde),
falls dieser noch nicht bereits im ``PATH`` aufgeführt wird. Ein typischer Pfad zur Toolchain könnte so aussehen: ```/home/user-name/esp/xtensa-lx106-elf/bin```.
Vor dem angehängten Pfad muss ein Doppelpunkt angehängt werden.
6. Erneut "Add.." betätigen und die Umgebungsvariable ```PYTHONPATH``` mit dem Wert ```/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/site-packages``` hinzufügen.
Somit überschreibt das systemseitig installierte Python alle in Eclipse vorkommenden Instanzen von Python.


<img src="images/paths.png" alt="Setting Paths" width="100%">
So könnte die vollständig konfigurierte "Environment" Umgebung aussehen.

Die hier verwendeten Umgebungsvariablen lassen sich unter MacOS außerdem in der .bash_profile Datei einsehen und bearbeiten. 
Diese kann  beispielsweise über ```nano .bash_profile``` aufgerufen werden.

Anschließend zum Reiter "C/C++ General" - "Preprocessor Include Paths" navigieren. 

1. In den "Providers" Tab wechseln.
2. Aus der Liste der Provider “CDT Cross GCC Built-in Compiler Settings” anklicken
3. “Command to get compiler specs” zu ```xtensa-lx106-elf-gcc ${FLAGS} -E -P -v -dD "${INPUTS}"``` ändern.
5. "Compiler Command Pattern" ändern in ```xtensa-lx106-elf-(gcc|g\+\+\c\+\+\+|cc|cpp|clang)```.

Nun zur "C/C++ General" -> "Indexer" Einstellungs-Seite wechseln.

1. "Enable project specific settings" aktivieren, um die restlichen Einstellungen auf dieser Seite zu aktivieren.
2. Die Option "Allow heuristic resolution of includes" deaktivieren. Wenn diese Option aktiviert ist 
kann es vorkommen, dass Eclipse die benötigten Header Verzeichnisse nicht findet.
3. Zu "C/C++ Build" -> "Behavior" navigieren
4. "Enable parallel build" aktivieren, um mehrere Build-Tasks parallel laufen lassen zu können.

Die genannte, offizielle Anleitung beschreibt zwar auch das Bauen und Flashen in Eclipse, jedoch wurde dies im Rahmen dieses Projektes
aufgrund der höheren Flexibilität und Geschwindigkeit ausschließlich über die Kommandozeile vorgenommen, wie im Folgenden beschrieben. 

<a name="esp_idf"></a>
### 5.2 ESP IDF

1. Führe ```$ make menuconfig``` im Projektverzeichnis aus um das Framework zu konfigurieren.
2. Wähle 'Serial flasher config' aus. Ändere den 'Default serial port' zu ```/dev/ttyUSB0```. Das wird nur vorausgesetzt, wenn MacOS or Linux verwendet wird. Unter Windows Hosts wird es nicht benötigt, da es keine Unterstützung für 'Device Assignments' gibt.
3. Wähle 'Flash SPI mode' aus und wähle anschließend 'DIO' aus.
4. Speichere die Einstellungen und verlasse das Konfigurationsmenü.

<a name="make"></a>
## 6. File includes mit make

Das vordefinierte Makefile des SDKs bietet verschiedene Möglichkeiten Dateien einzubinden. Dafür benötigt jedes Projekt ein eigenes Makefile, das auf das SDK-Makefile verweist:
```
PROJECT_NAME := easy_grow

include $(IDF_PATH)/make/project.mk
```
In diesem können weitere Include-Pfade spezifiziert werden, dies funktioniert in Abhängigkeit der Commit-Version sowie des Pfades dennoch nicht zuverlässig. 
```COMPONENT_ADD_INCLUDEDIRS```
```COMPONENT_SRCDIRS```

Daher wurden in dem Projekt EasyGrow Pseudo-Makefiles in die relevanten Ordner eingefügt. Diese leeren ```component.mk``` Dateien werden im Buildprozess erkannt und Dateien auf gleiche Dateiebene hinzugefügt.

<a name="flash_argumente"></a>
### 6.1 Flash-Argumente

```make build``` ruft den Buildprozess auf und die erzeugte Firmware kann mit  ```make flash``` auf den Mikrocontroller übertragen werden. 
```make flash``` nutzt hierbei die Einstellungen aus der ```sdkconfig```. Diese Datei kann manuell erzeugt oder mit ```make menuconfig``` generiert werden. In ihr sind Daten wie die Baudrate und der USB-Port enthalten.

Die Anweisungen, um die ```sdkconfig``` mit den minimalen Einstellungen für das Projekt zu generieren, befinden sich im Kapitel [5.2 Konfiguration des Espressif IoT Development Frameworks](#esp_idf).

Diese Einstellungen können mit ```make print_flash_cmd``` ausgegeben werden und bei direktem flashen über das Phyton-Programm ```esptool.py``` direkt gesetzt werden:

```python esptool.py --chip esp8266 --port /dev/ttyUSB0 --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode qio --flash_freq 40m --flash_size detect 0 bootloader/bootloader.bin 0x10000 example_app.bin 0x8000 partitions_singleapp.bin```

Der Aufruf über das ```esptool``` erfolgt innerhalb des Makefiles und bildet somit keinen differenten Flash-Prozess ab.

<a name="build"></a>
## 7. Bauen, Flashen und Monitoring

1. Baue das Projekt mit ```$ make``` aus dem Projektverzeichnis.
2. Führe ```$ make flash``` aus, um den nodeMCU zu flashen. Dieser Schritt funktioniert nur unter MacOS und Linux, jedoch nicht unter Windows Hosts.
3. Verbinde den nodeMCU nach dem Flashen. Um das Monitoring auszuführen führe ```$ make monitor``` aus. Dieser Schritt funktioniert nur unter MacOS und Linux, jedoch nicht unter Windows Hosts.

<a name="led_example"></a>
## 8. Bauen und Flashen des LED-Beispiels

1. Klone das Git Repository:<br>```$ git clone git@gitlab.mi.hdm-stuttgart.de:embedded/ss19/easy-grow.git```
2. Wechsle in das Projektverzeichnis ```easy-grow```:<br>```$ cd easy-grow```
3. Checke den ```example``` Branch aus:<br>```$ git checkout example```
4. Wechsle in das ```docker``` Verzeichnis:<br>```$ cd docker```
5. Verbinde den ESP8266 nodeMCU mittels USB.
6. Führe den ```docker.sh``` Script aus. Dieser Script erzeugt das Docker Image ```docker-esp8266``` und initialisier einen Container mit der ESP IDF Umgebung.<br>```$ ./docker.sh```
7. Wechsle innerhalb des Containers zum ```easy-grow``` Projektverzeichnis:<br>```$ cd easy-grow```
8. Konfiguriere den 'serial flasher' der ESP IDF. Weitere Informationen befinden sich hier: [5. Konfiguration des Espressif IoT Development Frameworks](#idf_config).
9. Baue das Projekt, flashe den nodeMCU, und aktiviere das Monitoring mit:<br>```$ make && make flash && make monitor```.<br> Falls eine Fehlermeldung erscheint, führe den folgenden Befehl erneut aus: ```$ make flash && make monitor```.

<a name="rtos_sdk"></a>
## 9. ESP8266 RTOS SDK

Das ESP8266 SDK ist eine Opensource IoT-Plattform für die Anwendungsentwicklung. Es wurde von Espressif entwickelt. Die SDK basiert auf dem Echtzeit-Betriebssystem RTOS, welches auf FreeRTOS basiert.

Das RTOS ist ein Multi-Tasking-Betriebssstem. Es können Standard Schnittstellen verwendet werden, um die Realisierung von Ressourcenmanagement, Recyclingoperationen, Ausführungsverzögerungen, Inter-Task-Messaging und -Synchronisation und andere aufgabenorientierte Prozessgestaltung Ansätze zu ermöglichen.

<a name="rtos_overview"></a>
### 9.1 Überblick

Ein ESP8266_RTOS_SDK-Projekt kann als Zusammenfassung mehrerer Komponenten betrachtet werden. Beispielsweise könnte es für ein HTTP-Request, das die aktuelle Erdfeuchtigkeit anzeigt, Folgendes geben:

- Die SoC-Basisbibliotheken (```libc```, ROM-Bindings etc.)
- Die WiFi-Treiber
- Ein TCP/IP-Stack
- Das FreeRTOS-Betriebssystem
- Das Hauptcode, der das Ganze zusammenfügt

ESP8266_RTOS_SDK macht diese Komponenten explizit und konfigurierbar. Dazu sucht die Build-Umgebung beim Kompilieren eines Projektes alle Komponenten in den SDK-Verzeichnissen, den Projektverzeichnissen und (optional) in zusätzlichen benutzerdefinierten Komponentenverzeichnissen nach (siehe Kapitel [6. File includes mit make](#make) für Letzeres).

Anschließend kann der Benutzer das ESP8266_RTOS_SDK-Projekt mithilfe eines textbasierten Menüsystems konfigurieren, um jede Komponente anzupassen (siehe Kapitel [5. Konfiguration des Espressif IoT Development Frameworks](#idf_config)). Nachdem die Komponenten im Projekt konfiguriert sind, kompiliert der Build-Prozess das Projekt.

<a name="rtos_concepts"></a>
### 9.2 Konzepte

Ein "Projekt" ist ein Verzeichnis, das alle Dateien und Konfigurationen enthält, um eine einzige ausführbare "App" zu erstellen, sowie zusätzliche unterstützende Outputs wie eine Partitionstabelle, Daten-/Dateisystempartitionen und einen Bootloader.

"Projektkonfiguration" wird in einer einzigen Datei namens ```sdkconfig``` im Stammverzeichnis des Projekts gespeichert. Diese Konfigurationsdatei wird über ```make menuconfig``` geändert, um die Konfiguration des Projekts anzupassen. Ein einzelnes Projekt enthält genau eine Projektkonfiguration.

Eine "App" ist eine ausführbare Datei, die von ESP8266_RTOS_SDK erstellt wird. Ein einzelnes Projekt erstellt in der Regel zwei Apps - eine "Project App" (die Hauptausführungsdatei, d.h. Ihre benutzerdefinierte Firmware) und eine "Bootloader-App", das die "Project App" startet.

"Komponenten" sind modulare eigenständige Codeteile, die in statische Bibliotheken (```.a```-Dateien) kompiliert und in eine App eingebunden werden. Einige werden von ESP8266_RTOS_SDK selbst bereitgestellt, andere können von anderen Stellen bezogen werden.

Einige Dinge sind nicht Teil des Projekts:

ESP8266_RTOS_SDK ist nicht Teil des Projekts. Stattdessen ist es eigenständig und über die Umgebungsvariable ```IDF_PATH``` mit dem Projekt verknüpft, die den Pfad des ESP8266_RTOS_SDK-Verzeichnisses enthält. Auf diese Weise kann das IDF-Framework vom Projekt entkoppelt werden.

Die Toolchain für die Kompilierung ist nicht Teil des Projekts. Die Toolchain sollte in der System-Befehlszeile ```PATH``` installiert werden, oder der Pfad zur Toolchain kann als Teil des Compiler-Präfix in der Projektkonfiguration festgelegt werden

<a name="example_project"></a>
### 9.3 Beispiel Projekt

Ein Beispiel-Projekt-Verzeichnis könnte folgendermaßen aussehen:

```
- project/
            - Makefile
            - sdkconfig
            - components/   - component1/   - component.mk
                                            - Kconfig
                                            - src1.c
                            - component2/   - component.mk
                                            - Kconfig
                                            - src1.c
                                            - include/      - component2.h
            - main/         - src1.c
                            - src2.c
                            - component.mk
            - build/
```

Folgende Elemente sind im Beispiel ```project``` enthalten:

- Ein Top-Level-Projekt Makefile. Dieses Makefile setzt die Variable ```PROJECT_NAME```und definiert (optional) projektweite Make-Variablen. Es enthält die Haupt-Makefile ```$(IDF_PATH)/make/project.mk```, die den Rest des EPS8266_RTOS_SDK Build System implementiert.
- Die Projekt-Konfigurationsdatei ```sdkconfig```. Diese Datei wird erstellt, bzw. aktualisiert, wenn ```make menuconfig``` ausgeführt wird und enthält die Konfiguration für alle Komponenten des Projekts (einschließlich ESP8266_RTOS_SDK selbst).
- Das optionale Verzeichnis ```components``` enthält Komponenten, die Teil des Projekts sind. Ein Projekt muss keine solchen benutzerdefinierten Komponenten enthalten, aber es kann nützlich sein, um wiederverwendbaren Code zu strukturieren oder Komponenten von Drittanbietern aufzunehmen, die nicht Teil von ESP8266_RTOS_SDK sind.
- Das Verzeichnis ```main``` ist eine spezielle Pseudokomponente, die Quellcode für das Projekt selbst enthält. ```main``` ist ein Standardname, die Makefile-Variablen ```COMPONENT_DIRS``` enthält diese Komponente. Diese Variable kann aber verändert werden (oder ```EXTRA_COMPONENT_DIRS``` setzen), um nach Komponenten an anderen Stellen zu suchen.
- Das Verzeichnis ```build``` ist der Ort, an dem der Build-Output erstellt wird. Nachdem der Make-Prozess ausgeführt wurde, enthält dieses Verzeichnis temporäre Objektdateien und Bibliotheken sowie endgültige binäre Output-Dateien. Dieses Verzeichnis wird in der Regel nicht der Source Control hinzugefügt oder mit dem Projekt-Quellcode verteilt.

Komponentenverzeichnisse enthalten ein Komponenten-Makefile ```component.mk```. Dies kann Variablendefinitionen zur Steuerung des Build-Prozesses der Komponente un deren Integration in das Gesamtprojekt enthalten.

Jede Komponente kann auch eine ```Kconfig```-Datei enthalten, die die Komponentenkonfigurationsoperationen definiert, die über die Projektkonfiguration eingestellt werden können. Einige Komponenten können auch ```Kconfig.projbuild``` und ```Makefile.projbuild``` Dateien beinhalten, die spezielle Dateien für übergeordnete Teile des Projekts sind.

<a name="rtos_xtasks"></a>
### 9.4 Tasks

Tasks sind ein wichtiges Feature von FreeRTOS. Mithilfe von Tasks können verschiedene Code-Abschnitte (scheinbar) parallel ausgeführt werden. 
Scheinbar deshalb, da ein Prozessor in der Realität zu einem bestimmten Zeitpunkt nur eine Aufgabe erledigen kann. 
Die vorgetäuschte Gleichzeitigkeit wird durch das schnelle Wechseln zwischen implementierten Tasks erreicht.

<a name="rtos_xtasks_impl"></a>
#### 9.4.1 Implementierung 
Für einen Task muss zunächst eine Funktion implementiert werden, die den auszuführenden Code beinhaltet. 
Die Funktion hat keinen Rückgabewert und nimmt als Eingangswert einen Zeiger vom Typ void, welcher genutzt werden kann, um Informationen jeglicher Art in die Funktion hineinzugeben. 
Da Task-Funktionen nichts zurückgeben, beinhalten sie meist eine kontinuierliche Schleife. 
Falls ein Task dennoch innerhalb seiner Funktion endet, sollte er sich mithilfe von ```vTaskDelete(null)``` beenden, um einen sauberen Abbruch zu garantieren. 

##### Beispiel

```
void vATaskFunction( void *pvParameters )
{
    for( ;; )
    {
        -- Auszuführender Code --
    }
    
    vTaskDelete( NULL );
}
```

Ein Task wird mit der Funktion ```xTaskCreate()``` kreiert, welche folgende Eingabeparameter erwartet:

| __Typ__ | __Name__ | __Zweck__ |
| :---    | :---     | :---      |
| ```TaskFunction_t``` | ```pvTaskCode``` | Unsere zuvor erstellte Task-Funktion |
| ```const char * const``` | ```pcName``` | Ein beschreibender Name für den Task, hauptsächlich für Debugging hilfreich |
| ```configSTACK_DEPTH_TYPE``` | ```usStackDepth``` | Die Anzahl der Wörter (nicht Bytes!), die für den Stack dieses Tasks allokiert werden sollen |
| ```void``` | ```*pvParameters``` | Ein Wert, der als Parameter in die Task-Funktion hineingegeben wird |
| ```UBaseType_t``` | ```uxPriority``` | Die Priorität, mit der der erstellte Task ausgeführt wird |
| ```TaskHandle_t``` | ```*pxCreatedTask``` | (optional) Weist dem Task einem zuvor instantiierten Handler zu. Dieser kann genutzt werden, um den Task beispielsweise zu pausieren oder zu löschen   |

Bei einem Erfolg gibt ```xTaskCreate``` ```pdPASS``` zurück, ansonsten ```errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY```. 
Ein laufender Task kann mit der Funktion ```xTaskDelete(TaskHandle_t xTask)``` gelöscht werden, indem der dem Task zugewiesene Handler als Eingabeparameter angegeben wird.

##### Komplettes Beispiel

```
void vATaskFunction( void *pvParameters )
{
    for( ;; )
    {
        -- Auszuführender Code --
    }
}

void anotherFunction()
{
    BaseType_t xReturned;
    TaskHandle_t xHandle = NULL;
    
    /* Task erstellen */
    xReturned = xTaskCreate(vATaskFunction, "NAME", 512, ( void * ) 1, 2, &xHandle );
    
    if( xReturned == pdPASS )
    {
        /* Der Task wurde erstellt.  Der Handler kann nun zum Löschen des Tasks genutzt werden. */
        vTaskDelete( xHandle );
    }
}


```
<a name="rtos_xtasks_usage"></a>
#### 9.4.2 Verwendung im Projekt

Im Easy Grow Projekt werden Tasks an 4 verschiedenen Stellen verwendet:
1. Zur kontinuierlichen Abfrage der GPIO Pins
2. Für den DNS Server
3. Für den Webserver
4. Zur Überprüfung, ob tatsächlich eine WiFi-Verbindung existiert

Bei Letzterem handelt es sich um einen Bug Fix, der mit dem Löschen des Flash-Speichers des ESP zusammenhängt. 
Das Löschen des Speichers sorgt dafür, dass zuvor gespeicherte WiFi-Credentials verschwinden, der ESP jedoch zunächst fehlerhaft zurückgibt, dass eine Verbindung exisiert.
Hier kommt der Task ins Spiel, der nach einer gewissen Zeit überprüft, ob tatsächlich eine WiFi-Verbindung vorhanden ist. 

Die Tasks werden in Kapitel 10 nochmal genauer beschrieben.

<a name="rtos_gpio"></a>
### 9.5 GPIO

Das ESP8266_RTOS_SDK bietet die ```esp8266/include/driver/gpio.h``` Datei an, um die 11 verwendbaren Pins des ESP8266 Chips zu konfigurieren und zu verwenden.

<a name="rtos_gpio_conf"></a>
#### 9.5.1 GPIO Konfiguration

Die Konfiguration eines GPIOs erfolgt über die Struct ```gpio_config_t``` und beinhaltet folgende Members:

| __Typ__ | __Name__ | __Zweck__ |
| :---    | :---     | :---      |
| ```uint32_t``` | ```pin_bit_mask``` | GPIO Pin:<br>Gesetzt durch Bit-Maske<br>Jedes Bit wird auf einen GPIO abgebildet. |
| ```gpio_mode_t``` | ```mode``` | GPIO Modus:<br>Input oder Output |
| ```gpio_pullup_t``` | ```pull_up_en``` | GPIO Pull-Up-Widerstand |
| ```gpio_pulldown_t``` | ```pull_down_en``` | GPIO Pull-Down-Widerstand |
| ```gpio_int_type_t``` | ```intr_type``` | GPIO Interrupt-Typ |

Durch die Enum ```gpio_mode_t``` wird der GPIO Modus definiert. Sie besitzt folgende Werte:

| __Wert__ | __Zweck__ |
| :---     | :---      |
| ```GPIO_MODE_DISABLE = GPIO_MODE_DEF_DISABLE``` | Deaktiviere GPIO als Input und Output |
| ```GPIO_MODE_INPUT = GPIO_MODE_DEF_INPUT``` | GPIO Input Modus |
| ```GPIO_MODE_OUTPUT = GPIO_MODE_DEF_OUTPUT``` | GPIO Output Modus |
| ```GPIO_MODE_OUTPUT_OD = ((GPIO_MODE_DEF_OUTPUT)\|(GPIO_MODE_DEF_OD))``` | Nur Output mit Open-Drain Modus |

Mittels der Enums ```gpio_pullup_t``` und ```gpio_pulldown_t``` werden die internen Pull-Up/-Down-Widerstände aktiviert bzw. deaktiviert. Sie besitzen folgende Werte:

| __Wert__ | __Zweck__ |
| :---     | :---      |
| ```GPIO_PULLUP_DISABLE = 0x0``` | Deaktiviere den GPIO Pull-Up-Widerstand |
| ```GPIO_PULLUP_ENABLE = 0x1``` | Aktiviere den GPIO Pull-Up-Widerstand |

| __Wert__ | __Zweck__ |
| :---     | :---      |
| ```GPIO_PULLDOWN_DISABLE = 0x0``` | Deaktiviere den GPIO Pull-Down-Widerstand |
| ```GPIO_PULLDOWN_ENABLE = 0x1``` | Aktiviere den GPIO Pull-Down-Widerstand |

Durch die Enum ```gpio_int_type_t``` wird definiert, bei welcher Flanke ein Interrupt für ein GPIO ausgelöst werden soll. Sie beinhaltet folgende Werte:

| __Wert__ | __Zweck__ |
| :---     | :---      |
| ```GPIO_INTR_DISABLE = 0``` | Deaktiviere GPIO Interrupt |
| ```GPIO_INTR_POSEDGE = 1``` | GPIO Interrupt bei steigender Flanke |
| ```GPIO_INTR_NEGEDGE = 2``` | GPIO Interrupt bei fallender Flanke |
| ```GPIO_INTR_ANYEDGE = 3``` | GPIO Interrupt bei steigender oder fallender Flanke |
| ```GPIO_INTR_LOW_LEVEL = 4``` | GPIO Interrupt bei Low-Level-Trigger |
| ```GPIO_INTR_HIGH_LEVEL = 5``` | GPIO Interrupt bei High-Level-Trigger |
| ```GPIO_INTR_MAX``` | - |

Nachdem ```gpio_config_t``` initialisiert wurde, kann die GPIO mittels der folgenden Methode konfiguriert werden:

```c
esp_err_t gpio_config(const gpio_config_t *gpio_cfg)
```

##### Beispiel
```c
#include <driver/gpio.h>
#define GPIO_PIN    16

gpio_config_t gpio_cfg;
gpio_cfg.mode = GPIO_MODE_INPUT;
gpio_cfg.pin_bit_mask = (1ULL << GPIO_PIN);
gpio_cfg.intr_type = GPIO_INTR_ANYEDGE;

gpio_config(&gpio_cfg);
```

Nach der Konfiguration kann der Wert eines GPIOs mit den Methoden gesetzt bzw. gelesen werden:

```c
esp_err_t gpio_set_level(gpio_num_t gpio_num, uint32_t level)
```

```c
int gpio_get_level(gpio_num_t gpio_num)
```

#### Beispiel

```c
#include <driver/gpio.h>
#define GPIO_PIN    16

gpio_config_t gpio_cfg;
gpio_cfg.mode = GPIO_MODE_OUTPUT;
gpio_cfg.pin_bit_mask = (1ULL << GPIO_PIN);
gpio_cfg.intr_type = GPIO_INTR_POSEDGE;

gpio_config(&gpio_cfg);

gpio_get_level(GPIO_PIN);       // level: 0
gpio_set_level(GPIO_PIN, 1);
gpio_get_level(GPIO_PIN);       // level: 1
```

<a name="rtos_gpio_isr"></a>
#### 9.5.2 Interrupt Service Routine

Wurde ```gpio_config_t.intr_type``` gesetzt, so besteht die Möglichkeit, bei verändernden Flanken eines GPIOs, auf diese zu reagieren mittels der ISR.

Hierfür ist es notwendig die folgende Methode, noch vor der Konfiguration der GPIO, aufzurufen. Es installiert den GPIO ISR-Handler-Dienst des Treibers, der GPIO-Interrupt-Handler pro Pin erlaubt.

```c
esp_err_t gpio_install_isr_service(int no_use)
```

Der Parameter ```no_use``` hat keine Bedeutung und muss lediglich mit 0 übergeben werden.

Bei der Konfiguration eines GPIOs muss der ISR-Handler hinzugefügt werden. Hierfür verwendet man folgende Methode:

```c
esp_err_t gpio_isr_handler_add(gpio_num_t gpio_num, gpio_isr_t isr_handler, void *args)
```

```isr_handler``` ist die ISR-Handler-Funktion, welche auf das entsprechende GPIO reagiert.
Über ```args``` können Parameter dem ISR-Handler übergeben werden.

Innerhalb der Funktion ```isr_handler``` sollte folgende Methode aufgerufen werden:

```c
BaseType_t xQueueSendFromISR(QueueHandle_t xQueue, const void *pvItemToQueue, BaseType_t *pxHigherPriorityTaskWoken)
```

Diese Methode setzt ein Element auf die Rückseite der ```xQueue```. Es ist sicher, diese Funktion innerhalb einer ISR zu verwenden. Elemente (```pvItemToQueue```) werden als Copy und nicht als Referenz in die Queue gestellt. Daher ist es besser, einen Zeiger auf das Element zu speichern, das in die Queue gestellt wird.

Eine Queue wird durch folgende Methode erzeugt:

```c
QueueHandle_t xQueueCreate(UBaseType_t uxQueueLength, UBaseType_t uxItemSize)
```

```uxQueueLength``` ist die maximale Zahl der Elemente einer Queue, die sie zur jederzeit beinhalten kann.
```uxItemSize```gibt die Größe (in Bytes) eines Queue-Elements an.

```xQueue```-Elemente werden in einem ```xTask``` verarbeitet. Um ein Element einer Queue zu erhalten, muss folgende Methode, innerhalb des ```xTask```s aufgerufen werden:

```c
BaseType_t xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait)
```

Elemente einer Queue werden als Copy übergeben, sodass ein Buffer (```pvBuffer```) einer entsprechenden Größe (in Bytes) übergeben werden muss.

##### Beispiel

```c
#include "freertos/task.h"
#include "freertos/queue.h"
#include <driver/gpio.h>
#define GPIO_PIN    16

// Queue-Handler für ISR
static xQueueHandle queue = NULL;

// Installiert GPIO ISR Dienst
gpio_install_isr_service(0);
// Erzeugen einer Queue, um GPIO Events einer ISR zu behandlen
queue = xQueueCreate(10, sizeof(uint32_t));
// Erzeuge und starte einen xTask
xTaskCreate(task, "task", 4096, NULL, 10, NULL);

gpio_config_t gpio_cfg;
gpio_cfg.mode = GPIO_MODE_INPUT;
gpio_cfg.pin_bit_mask = (1ULL << GPIO_PIN);
gpio_cfg.intr_type = GPIO_INTR_POSEDGE;

// ISR-Handler an eine bestimmte GPIO hängen
gpio_isr_handler_add(GPIO_PIN, isr_handler, (void *) GPIO_PIN);

gpio_config(&gpio_cfg);

static void isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(queue, &gpio_num, NULL);
}

static void task(void *arg)
{
    uint32_t gpio_num;
    while (1)
    {
        // Erhalte ein Element von der Queue
        if (xQueueReceive(queue, &gpio_num, portMAX_DELAY))
        {
            ...
        }
    }
}
```

<a name="rtos_gpio_analog"></a>
#### 9.5.3 Analogeingang

Das ESP8266_RTOS_SDK bietet die ```esp8266/include/driver/adc.h``` Datei an. Im Folgenden wird beschrieben, wie der Analogeingang des ESP8266 verwendet werden kann.

##### Initialisierung des ADC

Zunächst muss unter ```menuconfig->Component config->PHY->vdd33_const``` verändert werden.
```vdd33_const``` bietet ADC-Modus-Einstellungen, d.h. die Auswahl zwischen der Systemspannung oder externer Spannungsmessungen.
Beim Messen der Systemspannung muss der ```vdd33_const``` Wert auf 255 eingestellt werden.
Um die externe Spannung am ```TOUT(ADC)``` Pin zu lesen, benötigt ```vdd33_const``` einen Wert von < 255.
Wenn die ADC-Referenzspannung auf die aktuelle VDD33-Netzspannung eingestellt ist, beträgt der Wertebereich von ```vdd33_const[18,36]``` (Einheit 0,1 V).
Wenn die ADC-Referenzspannung auf den Standardwert von 3,3 V als Versorgungsspannung eingestellt ist, ist der Bereich von ```vdd33_const[0,18]``` oder (36, 255).

Das ESP8266_RTOS_SDK bieten die Struct ```adc_config_t``` zum Initialisieren des ADC Parameters an.

| __Typ__ | __Name__ | __Zweck__ |
| :---    | :---     | :---      |
| ```adc_mode_t``` | ```mode``` | ADC Modus |
| ```uint8_t``` | ```clk_div``` | ADC Sample Collection Clock<br>= 80M/```clk_div```<br>Bereich [8, 32] |

Die ADC-Arbeitsmodus Enum ```adc_mode_t``` hat folgende Werte:

| __Wert__ |
| :---     |
| ```ADC_READ_TOUT_MODE = 0``` |
| ```ADC_READ_VDD_MODE``` |

Über folgende Methode wird der ADC initialisiert:

```c
esp_err_t adc_init(adc_config_t *config)
```

###### Beispiel
```c
#include <driver/adc.h>
#define GPIO_PIN    16

adc_config_t adc_cfg;
adc_cfg.mode = ADC_READ_TOUT_MODE;
adc_cfg.clk_div = 8;

ESP_ERROR_CHECK(adc_init(&adc_cfg));
```

##### Lesen des Analogeingangs

Einzelne Messungen des ```TOUT(ADC)``` Pins (Einheit 1/1023 V) oder des VDD Pins (Einheit 1 mV) erfolgt über folgende Methode:

```c
esp_err_t adc_read(uint16_t *data)
```

Der Zeiger ```data``` nimmt den ADC-Wert entgegen.

Mehrere Messungen des ```TOUT(ADC)``` Pins (Einheit 1/1023 V) erfolgen über die folgende Methode:

```c
esp_err_t adc_read_fast(uint16_t *data, uint16_t len)
```

Der Zeiger ```data``` nimmt den ADC-Wert entgegen.
```len``` bestimmt die Länge der ADC Werte, die ausgelesen werden sollen.

###### Beispiel

```c
#include "driver/adc.h"
#define DEPTH   100

uint16_t adc_data[DEPTH];

if (ESP_OK == adc_read_fast(adc_data, DEPTH))
{
    for (int x = 0; x < DEPTH; x++)
    {
        printf("adc_data[%d] = %d", x, adc_data[x]);
    }
}
```

<a name="rtos_timer"></a>
### 9.6 Timer

Das ESP8266_RTOS_SDK bietet Software-Timer an, die jedoch einige Einschränkungen haben:

- Die maximale Auflösung ist gleich der RTOS-Tick-Rate
- Timer-Callbacks werden von einem Task mit niedriger Priorität ausgelöst

Hardware-Timer sind frei von beiden Einschränkungen, aber oft sind sie weniger komfortabel zu bedienen. Beispielsweise können Anwendungskomponenten Timer-Events benötigen, um bestimmten Zeiten in der Zukunft auszulösen, aber der Hardware-Timer enthält nur einen "Vergleichswert", der für die Interrupt-Erzeugung verwendet wird. Dies bedeutet, dass einige Hilfsmittel auf dem Hardware-Timer aufgebaut werden müssen, um die Liste der anstehenden Ereignisse zu verwalten und die Callbacks für diese Events versenden zu können, wenn entsprechende Hardware-Interrupts auftreten.

Die ```esp_timer``` API bietet eine solche Möglichkeit. Intern verwendet ```esp_timer``` einen 32-Bit-Hardware-Timer. ```esp_timer``` bietet einmalige und periodische Timer in Mikrosekunden-Zeitauflösung und 64-Bit-Bereich.

Timer-Callbacks werden von einer ```esp_timer```-Task mit hoher Priorität ausgelöst. Da alle Callbacks von derselben Task aus versendet werden, wird empfohlen, nur den geringstmöglichen Arbeitsaufwand von Callback selbst aus zu erledigen und stattdessen ein Event über eine Queue in einem Task mit niedriger Priorität zu senden.

Wenn andere Tasks mit einer höheren Priorität als ```esp_timer``` ausgeführt werden, wird das Callback-Dispatching verzögert, bis der ```esp_timer```-Task eine Chance zur Ausführung hat. Dies ist beispielsweise der Fall, wenn ein SPI-Flash-Vorgang durchgeführt wird.

Das Erstellen und Starten eines Timers sowie das Versenden des Callbacks dauert einige Zeit. Daher gibt es eine untere Grenze für den Timeout-Wert von einmaligen ```esp_timer```. Wenn ```esp_timer_start_once()``` mit einem Timeout-Wert von weniger als 20 µs aufgerufen wird, wird der Callback erst nach ca. 20 µs ausgelöst.

Der periodische ```esp_timer``` schränkt die minimale Timerzeit ebenfalls um 50 µs ein. Periodische Software-Timer mit einem Zeitraum von weniger als 50 µs sind nicht sinnvoll, da sie den größten Teil der CPU-Zeit verbrauchen würden. Die Verwendung von dedizierten Hardware-Peripheriegeräten oder DMA-Funktionen werden empfohlen, wenn ein Timer mit einer kurzen Zeitspanne erforderlich ist.

#### Verwendung der ```esp_timer``` API

Ein Single-Timer wird durch den Typ ```esp_timer_handle_t``` repräsentiert. Dem Timer ist eine Callback-Funktion zugeordnet. Diese Callback-Funktion wird nach jedem Ablauf des Timers von der ```esp_timer```-Task aufgerufen.

- Um einen Timer zu erstellen, muss ```esp_timer_create()``` aufgerufen werden
- Um den Timer zu löschen, muss ```esp_timer_delete()``` aufgerufen werden

Der Timer kann im One-Shot-Modus oder im periodischen Modus gestartet werden.

- Um den Timer im One-Shot-Modus zu starten, muss ```esp_timer_start_once()``` aufgerufen und der Zeitintervall angegeben werden, nach dem der Callback erfolgen soll. Wenn der Callback erfolgt, gilt der Timer als gestoppt.
- Um den Timer im periodischen Modus zu starten, muss ```esp_timer_start_periodic()``` aufgerufen und der Zeitraum, an dem der Callback erfolgt, übergeben werden. Der Timer läuft weiter, bis ```esp_timer_stop()``` aufgerufen wird.

Der Timer darf nicht laufen, wenn ```esp_timer_start_once()``` oder ```esp_timer_start_periodic()``` aufgerufen wird. Um einen laufenden Timer neu zu starten, muss zuerst ```esp_timer_stop()``` und dann eine der Startfunktionen aufgerufen werden.

Um die ```esp_timer``` API zu verwenden, muss die ```esp_common/include/esp_timer.h``` Datei eingebunden werden.

Die Konfiguration des Timers erfolgt durch den Sturct ```esp_timer_create_args_t```, der folgendes beinhaltet:

| __Typ__ | __Name__ | __Zweck__ |
| :---    | :---     | :---      |
| ```esp_timer_cb_t``` | ```callback``` | Callback-Funktion, die aufgerufen werden soll, wenn der Timer abläuft |
| ```void*``` | ```arg``` | Argument, das an den Callback übergeben werden kann |
| ```esp_timer_dispatch_t``` | ```dispatch_method``` | Aufruf des Callbacks von einem Task oder einer ISR |
| ```const char*``` | ```name``` | Timer Name, welcher in der ```esp_timer_dump``` Funktion verwendet wird |

Die Enum ```esp_timer_dispatch_t``` beinhaltet folgende Werte:

| __Wert__ | __Zweck__ |
| :---     | :---      |
| ESP_TIMER_TASK | Callback wird von einem Task aufgerufen |

##### Beispiel

```c
#include "esp_timer.h"
#define TIMER_TIME  100000

// Erzeugung des Timers
const esp_timer_create_args_t adc_timer_args = { .callback = &timer_callback, .name = "timer_callback" }

esp_timer_handle_t adc_timer;
ESP_ERROR_CHECK(esp_timer_create(&adc_timer_args, &adc_timer));

// Starten des Timers
ESP_ERROR_CHECK(esp_timer_start_periodic(adc_timer, TIMER_TIME));

void timer_callback(void* arg)
{
    int64_t time_since_boot = esp_timer_get_time();
    printf("Timer called, time since boot: (%d)", (int32_t)time_since_boot);
}
```

#### Abrufen der aktuellen Zeit

Die ```esp_timer``` API bietet auch eine Funktion, um die seit dem Start vergangene Zeit in Mikrosekunden zu erhalten: ```esp_timer_get_time()```. Diese Funktion gibt die Anzahl der Mikrosekunden seit der Initialisierung von ```esp_timer``` zurück, was normalerweise kurz vor dem Aufrug der ```app_main```-Funktion geschieht.

<a name="rtos_wifi"></a>
### 9.7 WiFi

Das ESP8266_RTOS_SDK bietet die ```esp8266/include/esp_wifi.h``` Datei an, die Unterstützung für WiFi-spezifische Funktionen bereit stellt. 
Dies umfasst unter Anderem:

1. Einen Station Mode, der genutzt werden kann, um den ESP mit einem Access Point (z.B. das heimische WLAN) zu verbinden.
2. Einen AP Mode, der es ermöglicht, einen Access Point zu eröffnen, mit dem sich andere Geräte verbinden können.
3. Einen kombinierten Modus (Station & AP), wodurch der ESP sowohl als Access Point fungieren als auch mit einem anderen Access Point verbunden sein kann. 
4. Verschiedene Sicherheitseinstellungen für die oben genannten Modi (WPA, WPA2, WEP, etc).
5. Das Scannen nach verfügbaren Access Points.

Bei vielen der folgenden Codebeispielen wird die Funktion ```ESP_ERROR_CHECK()``` verwendet. 
Dies ist eine Helfer-Funktion, die erspart, dass jede Funktion, die ```esp_err_t``` zurück liefert, auf einen potentiellen Fehler untersuchen werden muss. 

<a name="rtos_wifi_event"></a>
#### 9.7.1 Event Handler
Das ESP8266_RTOS_SDK liefert mit dem WiFi auch einen Event Handler. 
Dieser kann mithilfe von ```esp_err_t esp_event_loop_init(system_event_cb_t cb, void *ctx)``` initialisiert werden, wobei ```cb``` der eigentliche event_handler ist. 
Innerhalb von ```cb ``` können anschließend alle WiFi-bezogenen Events abgefangen werden.

##### Beispiel
```
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id)
    {
        case SYSTEM_EVENT_SCAN_DONE:
        {
            ...
            break;
        }
        case SYSTEM_EVENT_STA_START:
        {    
            ...
            break;
        }
    }
}
...
ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
```

<a name="rtos_wifi_init"></a>
#### 9.7.2 Initialisierung und Einstellung des Modus
WiFi muss immer zunächst mit der Funktion ```esp_err_t esp_wifi_init(const wifi_init_config_t *config)``` initialisiert werden.
Für den Eingangsparameter ```*config``` gibt es eine Standardkonfiguration, die meistens den Anforderungen genügt.

##### Beispiel
``` 	
wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();                                        	
ESP_ERROR_CHECK(esp_wifi_init(&cfg));
```
Mithilfe der Methode ```esp_err_t esp_wifi_set_mode(wifi_mode_t mode)``` kann der gewünschte Modus (Station, AP, kombiniert) eingestellt werden.

##### Beispiel
```
// Wahlweise WIFI_MODE_AP, WIFI_MODE_STA oder WIFI_MODE_APSTA
ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
```

<a name="rtos_wifi_config_connect"></a>
#### 9.7.3 Konfiguration und Verbindung
Für die Konfiguration des WiFi wird die Methode ```esp_err_t esp_wifi_set_config(wifi_interface_t interface, wifi_config_t *conf)``` bereit gestellt. 
Der erste Parameter bestimmt das Template (Wahl aus ESP_IF_WIFI_STA, ESP_IF_WIFI_AP und ESP_IF_MAX), der zweite enthält die eigentliche Konfiguration.

##### Beispiel
```
wifi_config_t wifi_config = {
    .ap = {
        .ssid = "MyWifiSSID",
        .password = "MyPassword",
        .authmode = WIFI_AUTH_WPA_WPA2_PSK
    },
};
ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
```
Mit den Funktionen ```esp_err_t esp_wifi_start(void)``` und ```esp_err_t esp_wifi_stop(void)``` wird anschließend der gewählte Modus mit den festgelegten Konfigurationen gestartet beziehungsweise beendet.
Für den Station- bzw. den kombinierten Modus muss zusätzlich ```esp_err_t esp_wifi_connect(void)``` aufgerufen werden, damit sich der ESP mit dem Access Point verbindet.
Gleichermaßen lässt sich die Verbindung mit ```esp_err_t esp_wifi_disconnect(void)``` auch wieder trennen. 

<a name="rtos_wifi_scan"></a>
#### 9.7.4 Nach verfügbaren Access Points scannen
Im Station- und kombinierten Modus besteht die Möglichkeit, nach verfügbaren Access Points zu scannen.
Dafür lässt sich die Methode ```esp_err_t esp_wifi_scan_start(const wifi_scan_config_t *config, bool block)``` verwenden. 
Mit dem ersten Eingabeparameter wird die Konfiguration übergeben, der zweite Parameter bestimmt, ob der Scan blockieren oder direkt zurückgeben soll.
Mit ```esp_err_t esp_wifi_scan_stop(void)``` kann der Scan zudem gestoppt werden.

##### Beispiel

```
wifi_scan_config_t scan_conf = {
    .ssid = NULL,
    .bssid = NULL,
    .channel = 0,
    .show_hidden = false
};
ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_conf, false));
...
ESP_ERROR_CHECK(esp_wifi_scan_stop());
```

Nach erfolgreichem Scan können die Ergebnisse mit der Methode ```esp_err_t esp_wifi_scan_get_ap_records(uint16_t *number, wifi_ap_record_t *ap_records)``` abgerufen werden.
Dabei muss der erste Parameter die Anzahl der Ergebnisse angegeben. 
Diese bekommt man mit der Methode ```esp_err_t esp_wifi_scan_get_ap_num(uint16_t *number)```.

##### Beispiel

```
uint16_t ap_count = 0;
esp_wifi_scan_get_ap_num(&ap_count);
if (ap_count == 0)
{
    // No APs found
}
wifi_ap_record_t *ap_list = (wifi_ap_record_t *)malloc(sizeof(wifi_ap_record_t) * ap_count);
if (!ap_list)
{
    // malloc error, ap_list is NULL
}
ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_count, ap_list));
```

<a name="rtos_http_server"></a>
### 9.8 HTTP Server
Für den HTTP-Server wird die netconn API von LwIP genutzt (http://www.nongnu.org/lwip/2_0_x/index.html).

<a name="rtos_http_server_new"></a>
#### 9.8.1 Eine (TCP-) Verbindung eröffnen
Eine neue Verbindung lässt sich mit der Funktion ```struct netconn * netconn_new(enum netconn_type t)``` implementieren. 
Die Form der Verbindung wird durch den Eingangsparameter (hier: NETCONN_TCP) bestimmt.
Ist die Verbindung erfolgreich erstellt worden, kann sie mithilfe der Methode ```err_t netconn_bind(struct netconn * aNetConn, ip_addr_t * aAddr, u16_t aPort)``` an eine IP-Addresse und einen Port gebunden werden.
Für die IP-Adresse kann ```IP_ADDR_ANY``` angegeben werden, um an eine beliebige Adresse zu binden. Eine TCP-Verbindung wird mit ```err_t netconn_listen(struct netconn * aNetconn)``` in den "listen mode" gesetzt.

##### Beispiel
```
struct netconn *xNetConn = netconn_new(NETCONN_TCP);   

netconn_bind(xNetConn, IP_ADDR_ANY, 80);
netconn_listen(xNetConn);
```

<a name="rtos_http_server_recv"></a>
#### 9.8.2 Eingehende Verbindungsanfragen abfangen

In einer Schleife können für eine eröffnete TCP-Verbindung eingehende Verbindungsanfragen abgefangen werden.
Dies geschieht durch die Methode ```err_t netconn_accept(struct netconn * aNetConn, struct netconn ** aNewConn)```.
Wird eine Verbindung festgestellt, wird eine neue netconn Struktur ```aNewConn``` für die weitere Nutzung allokiert.
Bei einer erfolgreichen Verbindungsetablierung liefert die Methode```err_t netconn_recv(struct netconn * aNetConn, struct netbuf ** aNetBuf)``` den Buffer mit den empfangenen Daten der zuvor neu allokierten Struktur und schreibt sie in ```aNetBuf```.

Im finalen Schritt werden die empfangenen Daten aus dem Buffer ```aNetBuf``` mithilfe der Methode ```err_t netbuf_data(struct netbuf * aNetBuf, void ** aData, u16_t * aLen)``` extrahiert und in ```aData``` geschrieben.

##### Beispiel 
```
struct netconn *xNetConn = netconn_new(NETCONN_TCP);   
struct netconn *client = NULL;

netconn_bind(xNetConn, IP_ADDR_ANY, 80);
netconn_listen(xNetConn);

while (1) {
    err_t err = netconn_accept(xNetConn, &client);
    if (err == ERR_OK) {
        struct netbuf *xNetBuf;
        if ((err = netconn_recv(client, &xNetBuf)) == ERR_OK) {
            void *data;
            u16_t len;
            netbuf_data(xNetBuf, &data, &len);
            // data can now be used
        }
    }
}
```

<a name="rtos_http_server_send"></a>
#### 9.8.3 Daten über eine TCP-Verbindung senden
Mit der Methode ```err_t netconn_write(struct netconn * aNetConn, const void * aData, size_t aSize, u8_t aApiFlags)``` lassen sich Daten über eine bestehende TCP-Verbindung zurücksenden.
Dabei ist ```aNetConn``` die TCP-Verbindung, ```aData``` der Adressanfang der zu sendenen Daten und ```aSize``` die Länge der zu sendenden Daten.
```aApiFlags``` kann entweder ```NETCONN_NOCOPY``` sein, falls die Daten für die Zeit der Überbringung stabil sind oder ```NETCONN_COPY```, falls nicht.

##### Beispiel
```
char buf[256];
const char *webpage = 
{
    "HTTP/1.1 200 OK\r\n"
    "Content-type: text/html\r\n\r\n"
    "<html>"
    "<head><title>Test Server</title></head>"
    "<body>"
    "<h1>Hello World</h1>"
    "</body>"
    "</html>"
}
// write webpage to buffer
snprintf(buf, sizeof(buf), webpage);

netconn_write(client, buf, strlen(buf), NETCONN_COPY);
```

<a name="rtos_http_server_close"></a>
#### 9.8.4 Verbindungen beenden
Mit ```void netbuf_delete(struct netbuf * aNetBuf)``` kann ein netbuf Objekt gelöscht und sein Speicher deallokiert werden.
Die komplette Verbindung lässt sich mit ```err_t netconn_close(struct netconn * aNetConn)``` schließen und mit ```err_t netconn_delete(struct netconn * aNetConn)``` löschen.
```
netbuf_delete(nb);
netconn_close(client);
netconn_delete(client)
```
<a name="rtos_flash"></a>
### 9.9 Schreiben und Lesen des Flash-Speichers

Um kleinere Werte persistent auf dem ESP-8266 speichern zu können orientiert sich der implementierte Code stark an der 
Umsetzung der von Arduino intern verwendeten Methoden (https://github.com/esp8266/Arduino/blob/master/libraries/EEPROM/EEPROM.cpp read und write) des EEPROM. 
(engl. Abk. für electrically erasable programmable read-only memory) Diese nutzen freie Bytes innerhalb des SPI Flash Speicherbereichs zum Lesen und Schreiben.


**Beispiel:**
##### Ausschnitt aus ```easy_flash_writer.c```

```c
void flash_write(void *value, FlashDataType dataType)
{
	uint32_t startSector = getStartSector(dataType);

	ESP_LOGI(TAG, "[flash_write]: Writing value to sector [%d], size: %d\n", startSector, SPI_FLASH_SEC_SIZE);

	esp_err_t status = spi_flash_erase_sector(startSector);
	if (status == SPI_FLASH_RESULT_OK)
	{

		status = spi_flash_write(startSector * SPI_FLASH_SEC_SIZE,
								 value, sizeof(value));

		if (status == SPI_FLASH_RESULT_OK)
		{
			ESP_LOGI(TAG, "[flash_write]: Successfully written to flash!");
			ret = true;
		}
		else
		{
			ESP_LOGI(TAG, "[flash_write]: Error with writing to flash, Error Code: [%d]", status);
		}
	}
}
```
So wird beispielsweise beim Schreiben eines Wertes zunächst der entsprechende Startsektor für die jeweilige Variable ausgewählt. 
Durch den Enum ```FlashDataType``` der mit in die Funktion gegeben wird,  hat jede Variable einen
fest definierten Sektor im frei verfügbaren Bereich des SPI Flashs zugewiesen.
Je nachdem welcher Wert gelesen oder geschrieben wird, unterscheidet sich die jeweilige Speicheradresse im SPI-Flash Range,
 damit Werte sich nicht gegenseitig überschreiben können.
 
Vor jedem Schreibvorgang wird zunächst mithilfe ``` spi_flash_erase_sector(startSector)``` vor dem Schreibvorgang der entsprechende Bereich gelöscht.
Bei Erfolg, also ``` SPI_FLASH_RESULT_OK``` wird die Adresse mithilfe der Funktion ```spi_flash_write(startSector * SPI_FLASH_SEC_SIZE,
 value, sizeof(value));``` ausgehend vom festgelegten ```startSector``` (zwischen den Speicheradressen 0x40200000 und 0x405FB000), 
 in Höhe einer festgelegten ```SPI_FLASH_SEC_SIZE``` von 4096bytes mit dem ```value``` und dessen tatsächlicher Größe ```sizeof(value)``` überschrieben. 


<a name="easy_grow"></a>
## 10. Easy Grow Projekt

<a name="eg_hardware"></a>
### 10.1 Hardware-Komponenten

| __Hardware-Komponente__ | __Bezeichnung__ | __Verwendungszweck__ | __Anzahl__ |
| :--- | :--- | :--- | :--- |
| NodeMCU | Amica V2 | Hardware-Logik<br>Webserver | 1 |
| Micro USB Port | - | Stromversorgung | 1 |
| Wasserpumpe | Fenteer 3V | Bewässerung der Pflanze | 1 |
| Photodiode | GL5528 | Aufzeichnung der Sonnenstunden | 1 |
| Hygrometer | FC-28 | Messung der Erdfeuchtigkeit | 1 |
| Amplifier (A-D) | - | Anschluss von:<br>- Photodiode<br>- Hygrometer | 2 |
| Button | - | Einstellung der Erdfeuchtigkeit | 2 |
| LED (Rot) | - | Erdfeuchtigkeitsanzeige<br>(geringe Feuchtigkeit) | 1 |
| LED (Gelb) | - | Erdfeuchtigkeitsanzeige<br>(mittlere Feuchtigkeit) | 1 |
| LED (Grün) | - | Erdfeuchtigkeitsanzeige<br>(hohe Feuchtigkeit) | 1 |
| LED (Blau) | - | Wasserstandanzeige<br>$`2^2`$ Zustände:<br>voll, gut, leer | 2 |
| Jumper-Kabel | - | Messung des Wassertankstandes<br>$`2^2`$ Zustände:<br>voll, gut, leer | 2 |

<a name="stromversorgung"></a>
### 10.2 Stromversorgung

Die Platine wird über einen Micro USB-B Anschluss mit Strom versorgt. Dabei sind der Pin 1 (VBUS) und der Pin 5 (GND) über einen Kippschalter mit dem Mikrocontroller verbunden. Pin 2, 3 und 4 als Datenleitungen wurden nicht verwendet, da die RX und TX Pins des NodeMCU Boards für das Interface genutzt werden. Zum Flashen ist daher der Mikrocontroller abnehmbar.

Neben der kompletten Abschaltung des Systems über den Kippschalter, wird diese Schaltung ebenfalls für den Pumpenbetrieb benötigt.
Mit einem Verbrauch von bis zu 8 Watt könnte der Power Regulator des NodeMCU je nach Betriebsart überlastet werden und sich zu hoch erhitzen.

Mit der Betriebsart über den Vin-Pin (Vin-PIn, Micro-USB-Anschluss, sowie 3,3V-Pin werden unterstützt) lassen sich nur um die 800 mA aus der V-Pins beziehen. Daher wird die Pumpe über die von uns entwickelte Platine mit Strom versorgt. 
Zur Ein- und Abschaltung wird ein Mosfet IRLZ44N genutzt.  Die 3,3 V der GPIO-Pins reichen hierbei zum Durchschalten des Mosfets aus. Der direkte Betrieb über die GPIO-Pins ist nicht möglich, da Verbraucher maximal 20mA über diese beziehen dürfen. Zudem wäre die Pumpleistung bei 3,3 V zu schwach. 

<a name="stromvebrauch"></a>
#### 10.2.1 Stromverbrauch

Der Stromverbrauch des Mikrocontrollers schwankt stark in Abhängigkeit zu dem Betriebsmodus. Funktioniert er als Accesspoint, während keine LED leuchtet, liegt der Verbrauch bei 108mA. Ist er mit einem WLAN Netzwerkverbunden benötigt die Schaltung 87 mA.
Pro eingeschaltete LED kommen 13 mA (rote LED, mit 2 V Flussspannung und 100 Ohm Vorwiderstand) hinzu. Die Pumpe verbraucht im Schnitt 1,2 A.

<a name="batteriebetrieb"></a>
#### 10.2.2 Batteriebetrieb

Für Evaluation des Batteriebetriebs muss zunächst der Verbrauch in Amperestunden ermittelt werden. Als typisches Beispiel wird daher angenommen, dass:
1. Der Controller mit einem WLAN verbunden ist (87 mA)
2. Neben den zwei Tank-LEDs eine weitere LED eingeschaltet ist (39 mA)
3. Die Pumpe 20 Sekunden pro Tag pumpt (0,012 mA)

Ein 5V Akku mit 2000 mAh wäre schon nach 15 Stunden leer.

Folgende Möglichkeiten könnte für den Batteriebetrieb in den Betracht gezogen werden:

+ Deaktivieren der LEDs nach wenigen Sekunden und Aktivierung erst wieder bei Knopfdruck
+ Deaktivieren der WiFi-Schnittstelle, wenn in einem gewissen Zeitraum keine SSID/Passworteingabe erfolgt ist.
+ Kompletter Headless-Modus (Keine Web- oder LED-Interface) nach einmaliger Feuchtigkeitseingabe über das Webinterface

<a name="eg_gpio"></a>
### 10.3 GPIO-Mapping

<img src="images/nodemcu.png" alt="NodeMCU GPIO-Mapping auf ESP8266">

| __Label__ | __GPIO__ | __Sensor__ | __Input__ | __Output__ | __ISR__ | __Bemerkung__ |
| :--- | :--- | :--- | :---: | :---: | :---: | :--- |
| D0    | 16   | Pumpe | x | ✓ | x | HIGH beim Boot. |
| D1    | 5    | LED Feuchtigkeit 2 (MID) | x | ✓ | x | - |
| D2    | 4    | LED Feuchtigkeit 1 (LOW) | x | ✓ | x | - |
| D3    | 0    | Decrease-Button | ✓ | x | ✓ | Boot schlägt fehl wenn auf LOW gezogen. |
| D4    | 2    | LED Feuchtigkeit 3 (HIGH) | x | ✓ | x | HIGH beim Boot.<br>Boot schlägt fehl wenn auf LOW gezogen. |
| D5    | 14   | Increase-Button | ✓ | x | ✓ | - |
| D6    | 12   | Wasserstandsensor 1 (Oben) | ✓ | x | x | - |
| D7    | 13   | Wasserstandsensor 2 (Unten) | ✓ | x | x | - |
| D8    | 15   | LED Wasserstand 1 (Oben) | x | ✓ | x | Boot schlägt fehl wenn auf HIGH gezogen. |
| RX    | 3    | Photodiode | ✓ | x | ✓ | HIGH beim Boot. |
| TX    | 1    | LED Wasserstand 2 (Unten) | x | ✓ | x | HIGH beim Boot.<br>Boot schlägt fehl wenn auf LOW gezogen. |
| A0    | ADC0 | Feuchtigkeitssensor | ✓ | x | x | Analog Input |

<a name="eg_circuit"></a>
### 10.4 Schaltbild

<img src="images/easy_grow_circuit.png" alt="Easy Grow Schaltbild">

<a name="eg_functionality"></a>
### 10.5 Funktionsweise

Dieses Kapitel beschreibt die detaillierte Funktionsweise des Easy Grow Bewässerungssystems.

<a name="eg_func_hw_logic"></a>
#### 10.5.1 Hardware-Logik

Die Hardware-Logik beschränkt sich auf folgende Features vom Easy Grow Projekt:

- [Einstellung der Erdfeuchtigkeit](#eg_func_hw_logic_set_moisture)
- [Messung der Erdfeuchtigkeit](#eg_func_hw_logic_read_moisture)
- [Bewässerung der Pflanze](#eg_func_hw_logic_watering)
- [Aufzeichnung der Sonnenstunden](#eg_func_hw_logic_sun_hours)

Folgende Dateien wurden für die Implementierung verwendet:

| __Datei__ | __Zweck__ |
| :---      | :---      |
| [```easy_gpio.c```](main/src/easy_gpio.c) | Konfiguration der GPIOs<br>Konfiguration des Analogeingangs<br>Initialisierung des Hardware-Timers |
| [```easy_controller.c```](main/src/easy_controller.c) | Funktionen zur automatischen und manuellen Steuerung des Bewässerungssystems |
| [```easy_debouncer.c```](main/src/easy_debouncer.c) | Software-Entprellung der Buttons zur Erdfeuchtigkeitseinstelllung |

<a name="eg_func_hw_logic_set_moisture"></a>
##### 10.5.1.1 Einstellung der Erdfeuchtigkeit

Die Einstellung der Erdfeuchtigkeit wird mittels zwei Buttons vorgenommen. Drei LEDs repräsentieren die vier Einstellungsmöglichkeiten der eingestellten Erdfeuchtigkeit. Sie werden durch die Enum ```MoistureLevel``` definiert.
Die vier Zustände der Erdfeuchtigkeit sind folgende:

| __Wert__ | __Zustand__ | __LED Zustand__<br>(0)=aus<br>(1)=ein |
| :---     | :---        | :---            |
| ```OFF``` | Deaktivieren des Bewässerungssystems | (0)(0)(0) |
| ```LOW``` | Niedrige Erdfeuchtigkeit<br>Erdfeuchtigkeitsbereich [733, 923] in V | (1)(0)(0) |
| ```MID``` | Mittlere Erdfeuchtigkeit<br>Erdfeuchtigkeitsbereich [544, 733] in V | (1)(1)(0) |
| ```HIGH``` | Hohe Erdfeuchtigkeit<br>Erdfeuchtigkeitsbereich [353, 543] in V | (1)(1)(1) |

Mittels den zwei Buttons kann die Erdfeuchtigkeit entweder inkrementiert oder dekrementiert werden. Diese Buttons befinden sich neben den Feuchtigkeits-LEDs.
Das Bewässerungssystem kann deaktiviert werden, in dem ein beliebiger Button so oft betätigt wird, bis keines der drei LEDs leuchten.

Die Betätigung der Buttons wird mittels einer ISR erkannt und behandelt. In dem ```gpio_task``` der ```easy_gpio.c``` Datei, ist der Handler ```moisture_button_handler(int io_num)``` für die Einstellung der Feuchtigkeit und LED-Zustände zuständig.

Um Fehlsignale des Buttons zu vermeiden wurde ein Software-Debouncer implementiert. Die Enum ```ButtonStates``` beinhaltet hierfür vier Zustände die ein Button besitzen kann:

| __Wert__ | __Zustand__ |
| :---     | :---        |
| ```UP``` | Der Button wird nicht betätigt |
| ```DOWN``` | Der Button ist betätigt |
| ```PRESS``` | Der Button wird gedrückt |
| ```RELEASE``` | Der Button wird losgelassen) |

Die ```ButtonStates delay_debounce(ButonStates button_state, int gpio_num)``` Funktion setzt den ```PRESS``` Zustand, wenn der Button für 25 ms gedrückt wird.
```RELEASE``` wird gesetzt, wenn der Button losgelassen wurde und für weitere 25 ms nicht betätigt wird.

Die Funktion und die Enum befinden sich in der ```easy_debouncer.c``` Datei. Der Aufruf der Funktion erfolgt in der ```easy_controller.c``` Datei, innerhalb der ```moisture_button_handler(int io_num)``` Funktion.

Das Speichern des eingestellten Feuchtigkeitswert wird im Kapitel [10.5.1.2 Messung der Erdfeuchtigkeit](#eg_func_hw_logic_read_moisture) behandelt.

<a name="eg_func_hw_logic_read_moisture"></a>
##### 10.5.1.2 Messung der Erdfeuchtigkeit

Die Messung der Erdfeuchtigkeit erfolgt mittels dem Analogeingang und Verwendung eines Hardware-Timers. Grundlagen für die Initialisierung des Analogpins und des Hardware-Timers befinden sich in den Kapitels [9.5.3 Analogeingang](#rtos_gpio_analog) bzw. [9.6 Timer](#rtos_timer).

Die Callback-Funktion ```read_moisture_level()``` wird alle 5 Minuten (```MOISTURE_READ_INTERVAL```) aufgerufen, um die Erdfeuchtigkeit zu lesen.  Der Callback ruft für das Lesen der Feuchtigkeit die ```MoistureValue get_moisture_level()``` Funktion auf, um 100 (```MOISTURE_READ_DEPTH```) Werte auszulesen, und anschließend den Mittelwert zu berechnen.
Der Callback ```read_moisture_level()``` und die Funktion ```get_moisture_level()``` befinden sich in der ```easy_controller.c``` Datei.

Der Wertebereich der Feuchtigkeit hat einen Bereich von [353,923]. Dieser Wert wird auf einen Wertebereich von 0 % bis 100 % abgebildet, um die Lesbarkeit zu gewährleisten. Hierfür wird der Funktion ```static uint8_t map(uint16_t x, uint16_t in_min, uint16_t in_max, uint8_t out_min, uint8_t out_max)``` der Mittelwert übergeben.

Nachdem der Mittelwert ausgelesen wurde und die prozentuale Feuchtigkeit berechnet wurde, werden diese Informationen im Struct ```MoistureValue``` gespeichert. Die Struct beinhaltet folgende Informationen:

| __Typ__ | __Name__ | __Zweck__ |
| :---    | :---     | :---      |
| ```Status``` | ```status``` | Enum signalisiert, ob das Lesen der Feuchtigkeit erfolgreich erfolgt ist |
| ```uint16_t``` | ```level_value``` | Gelesene Erdfeuchtigkeitswert über den ADC<br>Einheit: 1/1023 V |
| ```uint8_t``` | ```level_percentage``` | Abgebildeter Erdfeuchtigkeitswert auf 0 % bis 100% |
| ```MoistureLevel``` | ```level_target``` | Eingestellter Erdfeuchtigkeitsbereich<br>(```OFF```, ```LOW```, ```MID```, ```HIGH```) |

Die Enum ```Status``` beinhaltet folgende Werte:

| __Wert__ | __Zweck__ |
| :---     | :---      |
| ```FAILED``` | Der Lesevorgang der Feuchtigkeit ist nicht erfolgt |
| ```SUCCESS``` | Der Lesevorgang der Feuchtigkeit ist erfolgt |

Die Struct ```MoistureValue``` wird mittels des Konstruktors ```MoistureValue moisture_value_new(Status status, uint16_t level_value, uint8_t level_percentage, MoistureLevel level_target)``` initialisiert und in der globalen Variable ```moisture_value``` gespeichert und aktualisiert, sobald der Callback aufgerufen oder die Erdfeuchtigkeit mittels der Buttons verändert wurde.

<a name="eg_func_hw_logic_watering"></a>
##### 10.5.1.3 Bewässerung der Pflanze

Nach der Auslösung des Hardware-Timer-Callbacks (```read_moisture_value()```) wird zunächst überprüft, ob die gewünschte Erdfeuchtigkeit gegeben ist. Hierfür wird die Struct ```MoistureValue```, welche von der Funktion ```get_moisture_level()``` an die Funktion ```pump_handler(MoistureValue mv)``` übergeben wird.

Anhand des Werts ```mv.level_target```, welcher die gewünschte Feuchtigkeitseinstellung beinhaltet, wird die gelesene Feuchtigkeit, mit dem Wertebereich von ```mv.level_target``` verglichen. Befindet sich die gelesene Feuchtigkeit innerhalb des Wertebereichs, so muss die Pflanze nicht gegossen werden. Ebenso muss diese nicht gegossen werden, wenn der Zustand ```OFF``` aktiviert wurde oder die Erdfeuchtigkeit über den Wertebereich liegt, also die Pflanze überwässert ist.

Um den Wertebereich zu berechnen, muss die Funktion ```MoistureLevelRange get_moisture_level_target_range(MoistureLevel level_target)``` aufgerufen werden. Diese Funktion gibt den Wertebereich (Struct ```MoistureLevelRange```) der eingestellten Feuchtigkeit zurück und beinhaltet folgende Werte:

| __Typ__ | __Name__ | __Zweck__ |
| :---    | :---     | :---      |
| ```uint16_t``` | ```min``` | Untergrenze der Feuchtigkeit |
| ```uint16_t``` | ```max``` | Obergrenze der Feuchtigkeit |

Bevor die Pflanze bewässert wird, wird davor der Inhalt des Wassertanks überprüft. Hierzu wird die Funktion ```WaterLevel get_water_level()``` aufgerufen. Der Wassertank wird mittels zwei Sensoren überprüft. Diese Befinden sich zum einen auf der Oberkante und zum anderen auf der Unterkante des Behälters. Durch diese beiden Sensoren ergeben sich insgesamt $`2^2`$ Zustände. Diese Zustände werden durch die Enum ```WaterLevel``` definiert und sieht wie folgt aus:

| __Wert__ | __Zweck__ | __Zustand der Sensoren__<br>T;!T = Oberer Sensor<br>B;!B = Unterer Sensor
| :---     | :---      | :---      |
| ```EMPTY``` | Wassertank ist leer | !T && !B |
| ```GOOD``` | Wassertank ist gut gefüllt | !T && B |
| ```FULL``` | Wassertank ist voll | T && B |

Die Funktion ```WaterLevel water_level_leds_handler()``` stellt zudem den Zustand der Wasserstands-LEDs ein. Diese Funktion wird innerhalb der Funktion ```get_water_level()``` aufgerufen.

Je nach Wasserstand werden die Funktion ```activate_pump()``` oder ```deaktivate_pump()``` aufgerufen, um die Pflanze zu bewässern oder die Bewässerung zu unterbrechen.

Wurde die Funktion ```activate_pump()``` aufgerufen, so wird die Pflanze für 2 Sekunden (```PUMP_INTERVAL```) bewässert. Nach einem Delay von ```PUMP_INTERVAL``` wird die Funktion ```deactivate_pump()``` aufgerufen, um die Bewässerung zu unterbrechen.

Der Bewässerungsprozess wird erst nach dem nächsten Aufruf des Hardware-Timer-Callbacks wieder durchgeführt. Dieser Prozess stellt sicher, dass die Pflanze stets bewässert wird, sofern sich genügend Wasser im Behälter befindet.

<a name="eg_func_hw_logic_pap"></a>
##### 10.5.1.4 Programmablaufplan

<img src="images/easy_grow_pap.png" width="100%" alt="Programmablaufplan des Bewässerungssystems">

<a name="eg_func_hw_logic_sun_hours"></a>
##### 10.5.1.5 Aufzeichnung der Sonnenstunden

Easy Grow zeichnet die Sonnenstunden eines Tages (24 Stundenintervall) mittels der ```esp_timer``` API und einer ISR auf. Nachdem der Bootvorgang des NodeMCUs vollendet ist, startet der ```esp_timer```. Der ```esp_timer``` liefert mittels der ```esp_timer_get_time()``` Funktion die Zeit, die nach dem Bootvorgang bis zum aktuellen Zeitpunkt vergangen ist (in Mikrosekunden). Mit Hilfe der ISR werden Veränderungen der Lichtverhältnisse (an oder aus) ermittelt. Bei jeder ausgelösten ISR berechnet der Handler ```photo_diode_handler()``` die Zeitspanne der Tag- und Nachtwechsel (in Sekunden), bis insgesamt 24 Stunden vergangen sind. Nach 24 Stunden werden die aufgezeichneten Sonnenstunden (in Sekunden) wieder auf 0 zurückgestellt.

###### Initialisierung

Bei der Initialisierung der GPIO der Photodiode wird eine ISR eingesetzt, die auf positive und negative Flanken ausgerichtet ist. D.h., dass jede Veränderung des Lichtverhältnisses, den ISR auslöst.

Nach der Initialisierung der GPIO und der ISR wird der Sonnenstundenzähler implementiert. Dies geschieht in der ```init_sun_hours_counter()``` Funktion. Die Funktion liest zum Einen den initialen Zustand der Photodiode (```SUN_COUNTER_PREV_STATE```) und zum Anderen die aktuellen Zeit (```SUN_COUNTER_PREV_TIME```) aus und speichert diese im Flash-Speicher.

Die Zeitspannen der Tages- und Nachtdauer werden (```SUN_COUNTER_TIME_DAY``` und ```SUN_COUNTER_TIME_NIGHT``` Enums) werden initial auf 0 gesetzt und ebenfalls im Flash-Speicher persistiert.

###### Funktionsweise

Nachdem die ISR ausgelöst wurde, wird im Handler ```photo_diode_handler()``` die Zeitspanne der Lichtveränderung ausgerechnet.

Um die Lichtveränderungszeitspanne zu berechnen, muss zunächst herausgefunden werden, ob ein Tag-Nacht-Wechsel oder Nacht-Tag-Wechsel stattfand. Hierzu wird der ```SUN_COUNTER_PREV_STATE``` Wert ausgelesen und mit dem aktuellen Zustand (```STATE```) verglichen. Die folgende Tabelle veranschaulicht den Zustandswechsel:

| __PREV_STATE__ | __STATE__ | __Zustand__ |
| :---           | :---      | :---        |
| ```0``` | ```1``` | Nacht-Tag-Wechsel<br>Zeitspanne der Nacht muss berechnet werden. |
| ```1``` | ```0``` | Tag-Nacht-Wechsel<br>Zeitspanne des Tages muss berechnet werden. |

Die Zeitspanne (Tag oder Nacht) wird nur bei ungleichem Zustand  von ```PREV_STATE``` und ```STATE``` berechnet, also entweder bei einem Nacht-Tag- oder Tag-Nacht-Wechsel.
Der aktuelle ```STATE``` und die aktuelle ```TIME``` werden in der Funktion ```photo_diode_handler()``` gelesen.
Die Zeitspanne (```TIME_DELTA```) berechnet sich durch die Differenz der aktuellen Zeit (```TIME```) und der ```PREV_TIME```.

Danach wird ```PREV_STATE``` auf ```STATE``` und ```PREV_TIME``` auf ```TIME``` gesetzt, um bei der nächsten ISR die nächste Zeitspanne zu berechnen.
Der Tageszeitspanne (```SUN_HOURS_TIME_DAY```) bzw. der Nachtspanne (```SUN_HOURS_TIME_NIGHT```) wird ```TIME_DELTA``` aufaddiert, je nach Berechneter Zeitspanne (Tag oder Nacht).

Erreicht die Summe aus ```SUN_HOURS_TIME_DAY``` und ```SUN_HOURS_TIME_NIGHT``` 24 Stunden, so werden ```SUN_HOURS_TIME_DAY``` und ```SUN_HOURS_TIME_NIGHT``` auf 0 zurückgesetzt, um die Sonnenstunden der nächsten 24 Stunden aufzuzeichnen.

Die detaillierte Funktionsweise der Sonnenstundenaufzeichnung wird im folgenden Programmablaufplan beschrieben:

<img src="images/sun_hours_pap.png" alt="Programmablaufplan Sonnenstundenaufzeichnung">

<a name="eg_func_ap"></a>
#### 10.5.2 Access-Point / WiFi-Manager 
Für eine detaillierte Beschreibung der WiFi-Methoden und Funktionsweise siehe [Kapitel 9.7](#rtos_wifi). 
Im Folgenden wird vom Verständnis dieses Kapitels ausgegangen. 
 
Es gibt zwei verschiedene Ausgangslagen beim Start des ESP. Entweder er hat die Credentials eines zuvor verbundenen WLAN-Netzwerkes gespeichert oder nicht.
Dies wird differenziert, indem das WiFi zunächst im Station Mode gestartet wird und versucht wird, sich zu verbinden.
Bei gespeicherten Credentials sowie dem Vorhandensein des gespeicherten WLAN-Netzwerkes in Reichweite führt dies zu einer erfolgreichen Verbindung und somit zu [Kapitel 10.5.2.1.](#eg_func_ap_with_creds)
Der Ablauf bei einem fehlerhaften Rückgabewert wird in [Kapitel 10.5.2.2](#eg_func_ap_without_creds) behandelt.

##### Ausschnitt aus ```easy_wifi_manager.c```
```
wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
ESP_ERROR_CHECK(esp_wifi_init(&cfg));

// Try connecting first, will connect to station if it has saved credentials
ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
ESP_ERROR_CHECK(esp_wifi_start());
esp_err_t err = esp_wifi_connect();
// if it doesn't return ESP_OK, start ap mode
if (err != ESP_OK)
{
    ESP_LOGI(TAG, "Start in AP Mode");
    ap_wifi_init();
}
else
{
    // BUG: After erase_flash, esp_wifi_connect returns ESP_OK but never actually connects
    // create task to wait and check if it really connected -> https://github.com/esp8266/Arduino/issues/2235
    xTaskCreate(&check_conn_task, "wifi_config_server", 4096, NULL, 2, &check_conn_handle);
}
```

<a name="eg_func_ap_with_creds"></a>
##### 10.5.2.1 Start mit gespeicherten Credentials
Normalerweise könnte nach der erfolgreichen Verbindung sofort der Webserver mit der Easy Grow Webpage zur Steuerung der Bewässerungsanlage gestartet werden.
Ein Bug verhindert dies jedoch: Falls zuvor der Flash-Speicher des ESP gelöscht wurde, liefert ```esp_wifi_connect()``` fälschlicherweise ```ESP_OK``` zurück, obwohl keine Verbindung vorhanden ist.
Der Bug wird umgangen, indem mithilfe eines Tasks eine kurze Zeit gewartet wird. Anschließend wird geprüft, ob tatsächlich eine Verbindung besteht, indem geschaut wird, ob eine IP zugewiesen wurde.
Sobald eine IP zugewiesen wurde, wird der Webserver mit ```start_http(webMode webMode)``` im Modus ```EASY_MOISTURE``` gestartet.

##### Ausschnitt aus ```easy_wifi_manager.c```
```
case SYSTEM_EVENT_STA_GOT_IP:
    ...
    //Start moisture control http server
    start_http(EASY_MOISTURE);
    GOT_IP = true;
    break;
```

Ist nach dem Ablaufen des Tasks keine IP vorhanden, gelangen wir über den Aufruf von ```ap_wifi_init()``` zu [Kapitel 10.5.2.2](#eg_func_ap_without_creds).

<a name="eg_func_ap_without_creds"></a>
##### 10.5.2.2 Start ohne gespeicherte Credentials
Hat der ESP keine gespeicherten Credentials, muss das WiFi in den kombinierten Modus gesetzt werden (Station und AP). 
Dies ist nötig, da wir sowohl einen Access Point eröffnen wollen, als auch gleichzeitig den Station Mode benötigen, um nach verfügbaren WLAN-Netzwerken zu scannen. 
Das WiFi und der Scan werden konfiguriert und anschließend gestartet.

##### Ausschnitt aus ```easy_wifi_manager.c```
```
/*
 * Start wifi in AP-Mode (Access Point)
 */
void ap_wifi_init()
{
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0)
    {
    	wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    //Has to be AP + Station mode, in order to scan for available aps
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished.SSID:%s password:%s",
			 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);

    wifi_scan_config_t scan_conf = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = 0,
        .show_hidden = false
    };

    ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_conf, true));
} 
```
Ist der Scan beendet, werden die gefundenen WLAN-Netzwerke ausgelesen und mit der Funktion ```void set_aps(wifi_ap_record_t aps[], uint16_t apCount)``` in der Datei ```easy_http_server.c``` gesetzt.
Anschließend wird der Scan gestoppt und der HTTP-Server über ```void start_http(webMode webMode)``` im Config Mode ```EASY_CONFIG``` gestartet.

##### Ausschnitt aus ```easy_wifi_manager.c```
```
case SYSTEM_EVENT_SCAN_DONE:
{
    uint16_t ap_count = 0;
    //get number of available access points
    esp_wifi_scan_get_ap_num(&ap_count);
    if (ap_count == 0)
    {
        ESP_LOGI(TAG, "No AP found");
    }
    wifi_ap_record_t *ap_list = (wifi_ap_record_t *)malloc(sizeof(wifi_ap_record_t) * ap_count);
    if (!ap_list)
    {
        ESP_LOGI(TAG, "malloc error, ap_list is NULL");
        break;
    }
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_count, ap_list));
    set_aps(ap_list, ap_count);
    esp_wifi_scan_stop();
    free(ap_list);
    //Start config http server
    start_http(EASY_CONFIG);
    break;
}
```

<a name="eg_func_server"></a>
#### 10.5.3 Webserver
Um Web-Inhalte bereitzustellen zu können, wurde ein einfacher Webserver auf dem Gerät implementiert.
Für eine detaillierte Beschreibung der Funktionsweise des Servers und seiner Methoden siehe [Kapitel 9.8](#rtos_http_server).
Im Folgenden wird vom Verständnis dieses Kapitels ausgegangen.

<a name="eg_func_server_start"></a>
##### 10.5.3.1 Starten des Webserver-Tasks
Der Server läuft, wie bereits in [Kapitel 9.4](#rtos_xtasks) erwähnt, in einem eigenen Task. 
Über die Methode ```void start_http(webMode webMode)``` wird der Task gestartet oder, falls es bereits einen Task gibt, mit ```void vTaskResume(TaskHandle_t xTaskToResume)```fortgesetzt. 
Der übergebene  ```webMode``` wird in der Datei global gesetzt. Je nach Modus liefert der Server später eine andere Webpage zurück. 

##### Ausschnitt aus ```easy_http_server.c```
```
void start_http(webMode webMode) {
    mode = webMode;
    
    if(httpd_task_initialized) {
        vTaskResume(httpd_task_handle);
    } else {
        xTaskCreate(&httpd_task, "wifi_config_server", 14096, NULL, 2, &httpd_task_handle);
        httpd_task_initialized = 1;
    }
}
```

<a name="eg_func_server_page"></a>
##### 10.5.3.2 Webpage vorbereiten und senden
Im gestarteten Task werden nun die in [Kapitel 9.8](#rtos_http_server) beschriebenen Funktionen verwendet, um einen HTTP-Server zu starten.
Ruft der Nutzer nun mithilfe eines GET-Requests die Seite ab, wird dies abgefangen und die dem ```webMode``` entsprechende Webpage zurückgesendet. <br/>
Die verschiedenen Webpages liegen jeweils als ```static char *``` in der Datei ```easy_data.c```.
Dynamische Daten, wie die verfügbaren Access Points für die ```EASY_CONFIG```-Page, werden mithilfe der ```snprintf()```-Funktion an die mit ```%s``` markierten Stellen in den Webpage-String hineingeschrieben.

##### Ausschnitt aus ```easy_data.c```
```
static  char *WEBPAGE_NEW_CONFIG =
{
    ...
    "<div class='panel'>"
        "<h3>Verf&uuml;gbare WLAN:</h3>"
            "<p class='wifiLink'>%s</p>"
            "<p class='wifiLink'>%s</p>"
            "<p class='wifiLink'>%s</p>"
            "<p class='wifiLink'>%s</p>"
            "<p class='wifiLink'>%s</p>"
    "</div>"
    ...
};
```

##### Ausschnitt aus ```easy_http_server.c```
```
char webpage[5500];
strcpy(webpage, WEBPAGE_HEAD);
if (mode == EASY_CONFIG) {
    strcat(webpage, WEBPAGE_NEW_CONFIG);
    snprintf(buf, sizeof(buf), webpage, available_aps[0],
            available_aps[1], available_aps[2],
            available_aps[3], available_aps[4]);
} else if (mode == EASY_DOCUMENTATION) {
    strcat(webpage, WEBPAGE_DOCUMENTATION);
    snprintf(buf, sizeof(buf), webpage);
} else if (mode == EASY_MOISTURE) {
...
}
```

<a name="eg_func_server_interact"></a>
##### 10.5.3.3 Interaktionen auf der Webpage
Um Interaktionen zu realisieren, wurden Buttons eingeführt, die beim Klicken verschiedene Pfade an die URL anhängen und dorthin weiterleiten. 
Der erneute Request wird vom Server abgefangen und der angehängte Pfad kann ausgelesen werden.
Somit lässt sich beispielsweise bei einem Klick eines Feuchtigkeits-Buttons die Funktion für das Einstellen der Feuchtigkeit aufrufen.

##### Ausschnitt aus ```easy_data.c```
```
static  char *WEBPAGE_MOISTURE =
{
    ...
    "<div class='button-container'>"
        ...
        "<button class='b-lo' onclick=\"location.href='/low'\" type='button'>Niedrig</button>"
        "<button class='b-me' onclick=\"location.href='/medium'\" type='button'>Mittel</button>"
        "<button class='b-hi' onclick=\"location.href='/high'\" type='button'>Hoch</button>"
        ...
    "</div>"
    ...
}
```

##### Ausschnitt aus ```easy_http_server.c```
```
{
...
} else if (!strncmp(ptr, "/high", max_uri_len)) {
    set_moisture_level(HIGH);
} else if (!strncmp(ptr, "/medium", max_uri_len)) {
    set_moisture_level(MID);
} else if (!strncmp(ptr, "/low", max_uri_len)) {
    set_moisture_level(LOW);
}
```

<a name="eg_func_dns"></a>
#### 10.5.4 Easy_DNS


Der DNS Server wird genutzt um automatisiert die Setupwebseite anzuzeigen. 
Zunächst wird ein FreeRTOS Task erstellt, dieser läuft bis zur Auswahl eines Wlan Netzwerkes und dem anschließenden Wechsel von AP Mode zu Station Mode. Befindet sich der ESP in einem anderen Netzwerk wird der DNS Server nicht genutzt und kann daher beendet werden.
Bei der Erstellung des DNS Task wird der Namensserver für den UDP Port 53 registriert und die AF_INET Adressen Familien genutzt.
So kann ein Hostname oder eine IPv4 Adresse einem Port, hier alle eigehenden IP-Adressen dem DNS Port, zugewiesen werden. 
Ist der Task gestartet und die Socketverbindung erstellt, werden alle DNS Nachrichten empfangen. Diese müssen im nächsten Schritt gefiltert werden. Dabei werden zu lange (über 512 Bytes), zu kurze (unter 12 Bytes) Nachrichten und DNS Antworten der Klienten ignoriert. 
DNS Antworten werden nicht verarbeitet, weil diese für das Anzeigen einer Netzwerkanmeldung nicht benötigt werden. Ist die DNS Nachricht ein Request wird eine Antwort mit der Weiterleitung an die Netzwerkadresse des ESPs generiert. Das Anzeigen der Netzwerkanmeldung funktioniert je nach Betriebssystem unterschiedlich. Dabei besteht die Gemeinsamkeit im Erkennen der Weiterleitung durch den DNS Server. Das Endgerät versucht eine Webseite aufzurufen (Android z.B. ```connectivitycheck.android.com```) und erhält als Antwort HTTP Status 302 (temporary redirect) anstatt HTTP 204. HTTP 204 würde bedeuten die Seite ist verfügbar aber leer, wodurch das Endgerät weiß, dass eine Internetverbindung besteht. Mit HTTP Status 302, den das Gerät durch unsere DNS Server Weiterleitung erhält, wird die Aufforderung zur Netzwerkanmeldung angezeigt.


<a name="eg_func_server_gui"></a>
#### 10.5.5 Bedienung der Weboberflächen

Die in den vorherigen Abschnitten beschriebenen Funktionalitäten lassen sich über zwei verschiedene Weboberflächen steuern. 
Die Funktionaliät und Bedienung dieser wird im Folgenden beschrieben.

<a name="eg_func_server_gui-initial"></a>
##### 10.5.5.1 Setupwebseite
<img src="images/wifi-mockup.png" width="100%">

Nach der automatischen Weiterleitung bei Auswahl des ```EasyGrow_Initial_Config``` Netzwerkes wird diese Oberfläche je nach Betriebssytem in einem Popup-Fenster, oder im Browser geöffnet.
Der Nutzer hat hier die Möglichkeit ein WLAN Netzwerk in seinem Umfeld auszuwählen, um in den Station Modus zu wechseln. 
In einer Liste im oberen Bereich des Bildschirms lässt sich per Mausklick eine SSID auswählen, welche dann neben dem Feld ``Wlan-Name`` als ausgewählt dargestellt wird.
Über das Feld ``Passwort eingeben`` lässt sich im Anschluss das benötigte Wifi Passwort eingeben. 
Die darunterliegenden Buttons bieten die folgende Funktionalität:

**(1)** Bestätigen der eingegebenen Wifi Credentials, Schließen des Popups & Wechsel in den Station Modus.

**(2)** Öffnen der Access Point Ansicht (Steuerung des Systems)

**(3)** Öffnen der Systemdokumentation (Die Anzeige dieser gesamten Dokumentation war durch den kleinen Gerätespeicher leider nicht möglich. Daher wird über einen Link auf diese GitLab Seite verwiesen.)

#####WICHTIG für Punkt 1: 
Nach dem Absenden der Credentials schließt sich das ```EasyGrow_Initial_Config``` Netzwerk. Falls es weiterhin auftaucht, wurde vermutlich etwas falsch eingegeben und das Setup muss von vorne begonnen werden.
Falls nicht, verbindet man sein Endgerät nun mit dem zuvor eingegebenen Netzwerk. 
Die IP-Adresse des ESP im Netzwerk kann nicht mehr übermittelt werden, weswegen sie herausgefunden werden muss.
Dies lässt sich bei vielen Netzanbietern über das Router-Menü tun. (meist 192.168.1.1 oder 192.168.2.1). Alternativ lassen sich Apps wie Fing
(https://play.google.com/store/apps/details?id=com.overlook.android.fing&hl=de) verwenden. Sobald die richtige IP-Adresse gefunden wurde, muss sie nur noch im Browser der Wahl eingegeben werden.


<a name="eg_func_server_gui-ap"></a>
##### 10.5.5.2 Access-Point Webseite
<img src="images/overview-mockup.png" width="100%">

Nach Klick auf den Button AP-Mode in der vorherigen Ansicht oder nach erfolgreicher Verbindung zum Heimnetzwerk und Eingabe der IP-Adresse des ESP im Browser, gelangt man auf diese Ansicht, die Access-Point Webseite. 
Sie bietet einen schnellen Überblick über alle zur Steuerung des Systems relevanten Funktionen, ohne dafür ein WLAN-Netzwerk auswählen zu müssen. 
Die angzeigten Werte werden durch einen Reload der Webseite, alle 5 Sekunden aktualisiert.

Im oberen Bereich wird eine Auflistung aller wichtigen Parameter gezeigt, diese sind: (von Oben nach Unten) 
- der gewählte Feuchtigkeitswert (Soll-Wert) 
- der aktuelle Feuchtigkeitswert (Ist-Wert, in %)
- der Wasserstand im Tank (Niedrig, Normal, Hoch)
- die gemessenen täglichen Sonnenstunden
- Laufzeit des Systems (uptime)
- der restliche frei verfügbare Speicherplatz (in byte, zu monitoring Zwecken)

Darunter befindet sich eine Reihe mit Buttons zur Steuerung des Systems, sie bieten die folgende Funktionalität: 

**(1)** Zurücksetzen der Wifi-Konfiguration & Verlassen des Popups

**(2)** Steuerung des ausgewählten Feuchtigkeitswertes (Aus, Niedrig, Mittel, Hoch)

**(3)** Manuelles Betätigen der Wasserpumpe



<a name="git"></a>
### 10.6 Git
Als Version Control System (VCS) wird das MI-Gitlab der Hochschule der Medien genutzt.
Bei der Entwicklung des Projektes wurde Git mit Orientierung am Gitflow Workflow eingesetzt. Der Gitflow Workflow definiert ein strenges Modell für die Arbeit mit verschiedenen Branches, welches
besonders auf Projekt-Releases ausgerichtet ist. Dies bietet einen robusten Ablaufplan für die Verwaltung größerer Projekte. Anstelle eines einzigen Master-Branches verwendet der Giflow Workflow zwei Branches um den Fortschritt des Projekts zu versionieren.
Der Master-Branch verzeichnet dabei die offizielle Release-Historie und der Dev-Branch dient als Integrationszweig für Features. 
Es ist daher auch gängigige Praxis, Commits im Master-Branch mit einer Versionsnummer zu versehen.

Dadurch, dass die gesamte Feature-Entwicklung in bestimmten Feature-Branches und nicht im Master-Branch stattfindet, können Entwickler 
an einem bestimmten Feature arbeiten, ohne die Stabilität des gesamten Codes zu beeinträchtigen oder zu gefährden. 
Feature-Branches ermöglichen es außerdem, bestimmten Code zu reviewen bevor dieser in das offizielle Projekt integriert wird. 
Sobald auf dem Dev-Branch genügend Features für einen Release zusammengekommen sind, 
wird ein Release-Branch abgeforked und erlaubt es noch abschließende Bug-Fixes oder Dokuentation hinzuzufügen. 
Sobald dies abgeschlossen ist, kann der Release-Branch mit einer Versionsnummer versehen und in den Master-Branch gemerged werden.
Auf den Master-Branch kann somit nur getesteter und lauffähiger Code gelangen. 


<a name="git-cicd"></a>
#### 10.6.1 Continuous Integration

Mithilfe von GitLab wurde eine CI / CD (Continuous Integration / Continuous Deployment) Pipeline für das Projekt eingerichtet.  
Continuous Integration funktioniert nach dem Prinzip, dass bei jedem Push eine Pipeline von Skripten ausgeführt wird, 
welche die Code-Änderungen automatisch anwendet, testet und validiert, bevor sie in den entsprechenden Branch integriert werden.
Dieses Vorgehen ermöglicht es, Fehler schon frühzeitig im Entwicklungszyklus zu erkennen und sicherzustellen, 
dass der gesamte Code den festgelegten Anforderungen entspricht. 
Die einzelnen Schritte inerhalb einer GitLab CI/CD Pipeline werden über eine .gitlab-ci.yml Datei im Hauptverzeichnis des Projektes konfiguriert. 
Diese wird im Folgenden beschrieben.

```yaml
image: mirohero/docker-esp8266

sdk-test:
  script:
    - cd ${CI_PROJECT_DIR}/tests
    - chmod +x sdk-test.sh
    - ./sdk-test.sh

build-test:   
  script:
    - cd ${CI_PROJECT_DIR}
    - make
  only:
    - master
    - dev
  artifacts:
    name: "${CI_JOB_NAME}_${CI_COMMIT_REF_NAME}_${CI_JOB_ID}"
    when: always
    expire_in: 7d
    paths:
      - "${CI_PROJECT_DIR}/build/easy_grow.bin"

```

**(1)** Docker image
```yaml
image: mirohero/docker-esp8266
```
Für den Bau einer GitLab-Build-Pipeline würde sich im Normalfall ebenfalls Docker zum Bau gut eignen. 
Da die MI-Gitlab Pipeline jedoch das Bauen eines Docker-in-Docker Images nicht unterstützt, wurde sie mit diesem Befehl so konfiguriert, dass sie ein 
im Docker Hub bereitgestelltes Image (https://hub.docker.com/r/mirohero/docker-esp8266) pullt, welches alle benötigten Abhängigkeiten des esp8266 enthält.

**(2)** SDK Test Stage
```yaml
sdk-test:
  script:
    - cd ${CI_PROJECT_DIR}/tests
    - chmod +x sdk-test.sh
    - ./sdk-test.sh
```
In dieser Stage der Pipeline wird durch das Ausführen eines Skriptes die Funktionalität des installierten SDKs getestet. 
Über Kommandozeilenbefehle navigiert die .yml Datei in den Unterordner /tests, macht das Test-Script ```sdk-test.sh``` ausführbar und startet es.

**(3)** Build Test Stage
```yaml
build-test:   
  script:
    - cd ${CI_PROJECT_DIR}
    - make
  only:
    - master
    - dev
  artifacts:
    name: "${CI_JOB_NAME}_${CI_COMMIT_REF_NAME}_${CI_JOB_ID}"
    when: always
    expire_in: 7d
    paths:
      - "${CI_PROJECT_DIR}/build/easy_grow.bin"
```
Diese letzte Stage führt einen ```make``` Befehl im Hauptverzeichnis aus und baut so den implementierten C - Code des Projektes. 
```only``` gibt an, dass der Job nur für den entsprechenden Branches -master und -dev auszuführen ist.
Artifacts, beschreiben die Liste von Dateien oder Verzeichnissen, welche final von einem CICD Job erstellt werden, sobald dieser beendet ist. 
An dieser Stelle werden der Artefaktname, der Pfad und das Ablaufdatum (7Tage) gesetzt um die entsprechenden Dateien zu bauen. 
Dabei werden die folgenden (von GitLab) vordefinierten Umgebungsvariablen genutzt:

```CI_JOB_NAME```:
Der Name des Jobs, wie er in der .gitlab-ci.yml Datei vordefiniert ist.

```CI_COMMIT_REF_NAME```:
Der Branch- oder Commit-Name, für den das Projekt gebaut wird.

```CI_JOB_ID```:
Eine eindeutige ID des aktuellen Jobs, die GitLab CI intern erzeugt und verwendet.

```CI_PROJECT_DIR```:
Der vollständige Pfad auf dem das Repository geklont wird und auf dem der Job ausgeführt wird. 



<a name="eg_design"></a>
### 10.7 Produktdesign

Für die Entwicklung des ersten Prototyps war die Wahl eines passenden Produktgehäuses von grundlegender Wichtigkeit. 
Dieses sollte sowohl optisch ansprechen und alle benötigten Bauteile platzsparend zusammenfassen, als auch die sensible Elektronik vor Spritzwasser schützen. 
Mithilfe der kostenfreien online Platform www.tinkercad.com wurde ein entsprechendes 3D-Modell entwickelt, welches die gelisteten, gewünschten Funktionalitäten bietet.
Die entwickelte, schützende Kunststoffhülle lässt sich somit während des Betriebs von Easy Grow auf einen mit 
Wasser gefüllten Topf stülpen. Dabei entsteht zusammen mit dem Gehäuse ein säulenförmiger Standfuß für eine beliebige zu bewässernde Pflanze.
Im Inneren des Gehäuses lassen sich der ESP-Controller, sowie alle nötigen Bedienelemente wie LEDs und Schalter befestigen. 
Das Gehäuse bietet an seinen Seiten Öffnungen zur Verkabelung der verschiedenen Sensoren, Bedienelemente, Netzkabel und Pumpe.

Auf dem folgenden Bild ist das fertige 3D-Modell in Frontalansicht zu sehen. 
Sichtbar sind hier beispielsweise die Öffnungen für (von links nach rechts): 
- eine Power-LED
- die beiden vertikal angeordneten LEDS zur Wasserstandsanzeige
- Schalter zum Senken der gewählten Feuchtigkeit
- drei nebeneinander angeordnete LEDS zum Anzeigen des gewählten Feuchtigkeitsstands
- Schalter zum Erhöhen der gewählten Feuchtigkeit

<img src="images/3D-model.png" width="100%">

Das gewählte Material PETG (Polyethylene Terephthalate Glycol-modified), ist ein beliebtes 3D-Druckmaterial welches die jeweiligen Vorteile von ABS und PLA Kunststoffen vereint. 
PETG-Kunststoff bietet steife, dauerhafte Festigkeit (wie ABS) und einfache Handhabung (wie PLA) - da beim Druck kein Heizbett erforderlich ist. 
Außerdem bietet das Material eine gute Bodenhaftung und bildet gedruckt eine glatte, glänzende Oberfläche. Ein vergleichbares Produkt kann unter folgendem Link gefunden werden: https://www.amazon.com/AmazonBasics-Printer-Filament-1-75mm-Purple/dp/B07D68V8JB

Auf besondere, abdichtende Eigenschaften wurde bei der Wahl des Werkstoffes für diesen Prototyp noch kein Wert gelegt. 
Bei einer produktiver Umsetzung des Projektes wäre ein wasserabweisendes Material, sowie der Einsatz von Gummidichtungsringen an verschiedenen Stellen empfehlenswert.
Gedruckt wurde der Prototyp mithilfe eines Tevo Tarantula 3D-Druckers im privaten Gebrauch: https://www.tevo.cn/products/3d-printers/tevo-tarantula/



<a name="changelog"></a>
## 11. [Changelog](changelog.md)
