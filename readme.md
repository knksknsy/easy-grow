## Easy Grow Code Page

Easy Grow ist ein automatisches Bewässerungssystem für Pflanzen. Das System bietet drei verschiedene Einstellungsmöglichkeiten der Erdfeuchtigkeit. Diese Einstellungen können direkt über das System oder über ein Heimnetzwerk mittels einer Web-Applikation vorgenommen werden.
Das Bewässerungssystem hält die Erdfeuchtigkeit über den ganzen Tag feucht und ermöglicht die Langlebigkeit der Pflanzen.

## Externe Komponenten
Dieses Projekt nutzt Teile des DNS Servers aus der freien Quelle libesphttpd.
In der Datei easy_dns.c ist die Lizenz und der Author vermerkt, weitere Informationen sowie das Original finden sich unter  https://github.com/Spritetm/libesphttpd/blob/master/util/captdns.c.



## Table of Contents
- [1. Dokumentation](#docu) 
    * [1.1 Stromversorgung](#stromversorgung)
     + [1.1.1 Stromverbrauch](#stromvebrauch)
     + [1.1.2 Batteriebetrieb](#batteriebetrieb)
    * [1.2 File includes mit make](#make)
     + [1.2.1 Flash-Argumente](#flash_argumente)
     + [1.2.2 Erstellen der Dokumentation mit make](#make_documentation)
    * [1.3 Systembausteine](#systembausteine)
        * [1.3.3 Easy_DNS](#dns)
            - [2.1.1.1 MacOS](#inst_docker_mac)
            - [2.1.1.2 Windows](#inst_docker_win)
    + [2.1.2 Bauen des Docker-Images](#build_docker)
        + [2.1.3 Freigeben des seriellen Ports vom Hosts zum Docker-Container](#serial_port)
            - [2.1.3.1 MacOS](#serial_port_mac)
            - [2.1.3.2 Windows](#serial_port_windows)
        + [2.1.4 Ausführen des Docker-Containers](#cont_docker)
            - [2.1.4.1 MacOS](#cont_docker_mac)
            - [2.1.4.2 Windows](#cont_docker_win)
    * [2.2 ESP8266 Toolchain-Setup auf einer lokalen Maschine (Nur MacOS)](#local)
- [3. Konfigurierung des Espressif IoT Development Frameworks](#idf_config)
- [4. Bauen, Flashen und Monitoring](#build)
- [5. Bauen und Flashen des LED-Beispiels](#led_example)
- [6. Changelog](#changelog)
<!-- toc -->

<a name="docu"></a>
## 1. Dokumentation
<a name="stromversorgung"></a>
### 1.1 Stromversorgung

Die Platine wird über einen Micro USB-B Anschluss mit Strom versorgt. Dabei sind der Pin 1 (VBUS) und der Pin 5 (GND) über einen Kippschalter mit dem Microkontroller verbunden. Pin 2,3,4 als Datenleitungen wurden nicht verwendet, da die RX und TX Pins des NodeMCU Boards für das Interface genutzt werden.
Zum FLashen ist daher der Microkontroller abnehmbar.

Neben der kompletten Abschaltung des Systems über den Kippschalter, wird diese Schaltung ebenfalls für den Pumpenbetrieb benötigt.
Mit einem Verbauch von bis zu 8 Watt könnte der Power Regulator des NodeMCU je nach Betriebsart überlastet werden und sich zu hoch erhitzen.

Mit der Betriebsart über den Vin-Pin (Vin-PIn, Micro-USB-Anschluss, sowie 3.3V-Pin werden unterstützt) lassen sich nur um die 800mA aus der V-Pins beziehen. Daher wird die Pumpe über die von uns entwickelte Platine mit Strom versorgt. 
Zur Ein- und Abschaltung wird ein Mosfet IRLZ44N genutzt.  Die 3.3V der GPIO-Pins reichen hierbei zum Durchschalten des Mosfets aus. Der direkte Betrieb über die GPIO-Pins ist nicht möglich, da Vebraucher maximal 20mA über diese beziehen dürfen. Zudem wäre die Pumpleistung bei 3.3V zu schwach. 

<a name="stromvebrauch"></a>
#### 1.1.1 Stromvebrauch
Der Stromvebrauch des Microkontrollers schwankt stark in Abhängigkeit zu dem Betriebsmodus. Funktioniert er als Accesspoint, während keine LED leuchtet, liegt der Vebrauch bei 108mA. Ist er mit einem WLAN Netzwerkverbunden benötigt die Schaltung 87mA.
Pro eingeschaltete LED kommen 13mA (rote LED,mit 2V Flussspannung und 100 Ohm Vorwiderstand) hinzu. Die Pumpe verbraucht im Schnitt 1.2A.

<a name="batteriebetrieb"></a>
#### 1.1.2 Batteriebetrieb
Für Evaluation des Batteriebetriebs muss zunächst der Verbrauch in Amperestunden ermittelt werden. Als typisches Beispiel wird daher angenommen, dass:
1. Der Kontroller mit einem Wlan verbunden ist (87mA)
2. Neben den zwei TankLEDs eine weitere LED eingeschalten ist (39mA)
3. Die Pumpe 20 Sekunden pro Tag pumpt (0.012 mA)

Ein 5V Akku mit 2000mAh wäre schon nach 15 Stunden leer.

Folgende Möglichkeiten könnte für den Batteriebetrieb in den Betracht gezogen werden:

+ Deaktivieren der LEDs nach wenigen Sekunden und Aktivierung erst wieder bei Knopfdruck
+ Deaktivieren der WiFi-Schnittstelle, wenn in einem gewissen Zeitraum keine SSID/Passworteingabe erfolgt ist.
+ Kompletter Headless-Modus (Keine Web- oder LED-Interface) nach einmaliger Feuchtigkeitseingabe über das Webinterface


<a name="make"></a>
### 1.2 Includierung von Dateien mit Make

Das vordefinierte Make-File des SDKs bietet verschiedene Möglichkeiten Dateien einzubinden. Dafür benötigt jedes Projekt ein eigenes Makefile, das auf das SDK-Makefile verweist:
```
PROJECT_NAME := easy_grow

include $(IDF_PATH)/make/project.mk
```
In diesem können weitere Include-Pfade spezifiziert werden, dies funktioniert in Abhängigkeit der Commitversion sowie des Pfades dennoch nicht zuverlässig. 
```COMPONENT_ADD_INCLUDEDIRS```
```COMPONENT_SRCDIRS```

Daher wurden in dem Projekt EasyGrow Pseudo-Makefiles in die relevanten Ordner eingefügt. Diese leeren ```component.mk``` Dateien werden im Buildprozess erkannt und Dateien auf gleiche Dateiebene hinzugefügt.

<a name="flash_argumente"></a>
#### 1.2.1 Flash-Argumente
```make build``` ruft den Buildprozess auf und die erzeugte Firmware kann mit  ```make flash``` auf den Microkontroller übertragen werden. 
```make flash``` nutzt hierbei die Einstellungen aus der sdkconfig. Diese Datei kann manuell erzeugt oder mit ```make menuconfig``` generiert werden. In ihr sind Daten wie die Baudrate und der USB-Port enthalten. 
Diese Einstellungen können mit ```make print_flash_cmd``` ausgegeben werden und bei direktem flashen über das Phyton -Programm esptool.py direkt gesetzt werden:
```python esptool.py --chip esp8266 --port /dev/ttyUSB0 --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode qio --flash_freq 40m --flash_size detect 0 bootloader/bootloader.bin 0x10000 example_app.bin 0x8000 partitions_singleapp.bin
```
Der Aufruf über das esptool erfolgt innerhalb des Makefiles und bildet somit keinen differenten Flashprozess ab.

<a name="make_documentation"></a>

#### 1.2.2 Erstellen der Dokumentation mit make
```make documentation``` bietet die Möglichkeit die aktuelle Dokumentation aus der Datei readme.md in ein HTML-File umzuwandeln und diese im Anschluss auf einer Webseite des ESP anzuzeigen. Für die Erstellung des HTMLs wird Pandoc https://pandoc.org/ benötigt. Der Benutzer kann sich somit die aktuelle Dokumentation des Projekts in dem produktiven System anzeigen lassen. Jedoch werden Bilder dabei nicht abgebildet.


<a name="systembausteine"></a>
### 1.3 [Systembausteine]

<a name="dns"></a>
#### [1.3.3 Easy_DNS]

<a name="sw_env"></a>
## 2. Aufsetzen der Softwareumgebung

Es stehen zwei Setup-Möglichkeiten zur Verfügung um die ESP8266 Software-Umgebung aufzusetzen:

1. Mittels eines Docker-Images (Linux Ubuntu 16.04 64bit)
2. Installierung auf einer lokalen Maschine mittels eines Scripts (Nur MacOS)

Für die Installation wird Docker empfohlen.

<a name="tool_docker"></a>
### 2.1. ESP8266 Toolchain-Setup mit Docker

<a name="inst_docker"></a>
#### 2.1.1 Installierung von Docker

<a name="inst_docker_mac"></a>
##### 2.1.1.1 MacOs

1. Brew installieren: https://brew.sh
2. Docker Engine und Docker-Machine installieren:<br/>```$ brew install docker```
3. Cask zum Brew Tap hinzufügen:<br/>```$ brew tap caskroom/cask```
3. Docker Client installieren:<br/>```$ brew cask install docker```
4. Optional: Falls Virtualbox nach dem 2. Schritt fehlt. Virtualbox installieren mit:<br/>```$ brew cask install virtualbox```

<a name="inst_docker_win"></a>
##### 2.1.1.2 Windows

Folge den Anweisungen auf https://docs.docker.com/docker-for-windows.

<a name="build_docker"></a>
#### 2.1.2 Bauen des Docker-Images

Das Docker-Image muss nur einmal gebaut werden. Mit ```$ docker images``` kann überprüft werden, ob das Image bereits installiert wurde.

Das Image muss nicht installiert werden, falls die Zeile ```docker-esp8266``` im Terminal ausgegeben wird.

Falls ```docker-esp8266``` im Terminal nicht erscheint, müssen die folgenden Schritte ausgeführt werden:

1. Wechsle das Verzeichnis indem sich die ```Dockerfile``` befindet:<br/>```$ cd <project_path>/docker```
2. Baue das Docker-Image: ```$ docker build -t docker-esp8266 .```

Im 2. Schritt wird ein Ubuntu-Image gebaut und die vorausgesetzten Packages für den ESP8266 Toolchain installiert. Der Befehl führt nach der Ausführung einen Script aus, der die Toolchain installiert.

<a name="serial_port"></a>
#### 2.1.3 Freigeben des seriellen Ports vom Hosts zum Docker-Container

<a name="serial_port_mac"></a>
##### 2.1.3.1 MacOS

Für MacOS muss der "CP210x USB to UART Bridge VCP" Treiber installiert werden.

Lade den Treiber herunter und folge den Anweisungen auf der Silicon Labs Website (https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers).

Überprüfe ob der Treiber erfolgreich installiert wurde. Die folgende Zeile ```$ ls /dev/tty.*``` sollte ```/dev/tty.SLAB_USBtoUART``` ausgeben.

Die nächsten Schritte zeigen, wie ein serieller Port vom Host an den Docker-Container freigegeben werden kann.
Hierfür muss zunächst ein Virtualbox Treiber für die Docker-Maschine erzeugt werden.

1. ```$ docker-machine create --driver virtualbox default```
2. Überprüfe ob der Treiber erzeugt wurde: ```$ docker-machine ls```
3. Stoppe die Docker-Maschine: ```$ docker-machine stop```
4. Jetzt kann die VM konfiguriert und der serielle Port exportiert werden. Öffne die Virtualbox Anwendung. Wähle die 'default' VM aus und klicke auf den 'Settings' Button, um die USB Einstellungen vorzunehmen.
![Virtualbox 'default' VM Einstellungen](images/virtualbox_default_settings.png)
5. Klicke auf 'Ports' und anschließend auf den 'USB' Tab. Aktiviere die 'Enable USB Controller' Checkbox. Wähle 'USB 2.0 (EHCI) Controller' aus. Füge einen USB Filter hinzu (USB Icon mit grünem Plus Symbol). Wähle den 'Silicon Labs CP2102 USB to UART Bridge Controller [0100]' Treiber aus.
![Virtualbox serieller Port](images/virtualbox_port_usb.png)
6. Falls USB 2.0 nicht ausgewählt werden kann, muss der 'Oracle VM VirtualBox Extension Pack' installiert werden. Die Anweisungen für die Installation befindet sich hier: https://www.virtualbox.org/wiki/Downloads
7. Starte die Docker-Maschine mit: ```$ docker-machine start```
8. Es müssen einige Umgebungsvariablen gesetzt werden, damit Docker die VM verwendet anstelle des nativen Modus. Das ```$ docker-machine env``` Kommando gibt die notwendigen Schritte für das Setzen der Variablen an. Führe folgenden Befehl aus um diese zu setzen: ```& eval "$(docker-machine env default)"```

Der serielle Port des Hosts ist nun vom Docker-Container aus ansprechbar.

<a name="serial_port_win"></a>
##### 2.1.3.2 Windows

Leider gibt es bis auf Weiteres keine Unterstüztung der 'Device Assignment' und 'Sharing Workloads' in Hyper-V-isolierte Windows Containern. 

<a name="cont_docker"></a>
#### 2.1.4 Ausführen des Docker-Containers

Stelle sicher, dass Docker installiert, das ```docker-esp8266``` Image gebaut und der serielle Port des Hosts an den Docker-Container freigegeben wurde, bevor der Docker container gestartet werden soll.

Es gibt zwei Möglichkeiten einen Container vom ```docker-esp8266``` Image zu instanziieren:

1. Über den ```docker.sh``` Script.<br/>(Nur MacOS)
2. Über den ```docker run``` Befehl.<br/>(Empfohlen für Windows)

<a name="cont_docker_mac"></a>
##### 2.1.4.1 MacOS

1. Wechsle in den ```docker``` Ordner im Projektverzeichnis: ```$ cd <project_path>/docker```
2. Starte den Container mit dem Script: ```$ ./docker.sh```. Dieser Script startet einen Container für das ```docker-esp8266``` Image. Der Script mountet zudem das Projektverzeichnis in das ```/easy-grow``` Verzeichnis des Containers. Zudem exportiert er den Hosts ```/dev/ttyUSB0``` Port an den Port ```/dev/ttyUSB0``` des Containers.

<a name="cont_docker_win"></a>
##### 2.1.4.2 Windows

Führe folgendes Befehl im Projektverzeichnis ```<project_path>``` aus:<br/>
```docker run -ti --rm --name esp8266 -v <project_path>:/easy-grow docker-esp8266 /bin/bash```

<a name="local"></a>
### 2.2 ESP8266 Toolchain-Setup auf einer lokalen Maschine (Nur MacOS)

Wechsle in das ```setup``` Verzeichnis im Projektverzeichnis um die Softwareumgebung für den ESP8266 aufzusetzen.
Dort befindet sich der ```initial_setup.sh``` Script, der folgendes automatisch aufsetzt:

- Die Toolchain um Applikationen für den ESP8266 zu bauen.
- Die ESP8266_RTOS_SDK, die die API und Scripte beinhaltet um die Toolchain zu betreiben.

Übergebe das ```--dir <path>``` Argument, um die Softwareumgebung in einem beliebigen Verzeichnis zu installieren. 
Wird dieses Argument nicht gesetzt, wird standardmäßig die Softwareumgebung im ```ESP``` Ordner des Projektverzeichnis installiert.

Die Toolchain befindet sich im Pfad ```<path>/ESP/xtensa-lx106-elf``` wohingegen die SDK sich im Pfad ```<path>/ESP/ESP8266_RTOS_SDK``` befindet.

Öffne ein neues Terminalfenster und füge den folgende Befehl ein um das Setup zu starten:
- Wechsle in das ```setup``` Verzeichnis im Projektpfad:<br>```$ cd setup```
- Führe den Script als ein User aus (nicht als Root):<br>```$ ./initial-setup.sh```
- Warte bis das Setup beendet ist.
- Vergiss nicht dein ```bash_profile``` nach dem Setup zu sourcen:<br> ```$ source ~/.bash_profile```

<a name="idf_config"></a>
## 3. Konfigurierung des Espressif IoT Development Frameworks

1. Führe ```$ make menuconfig``` im Projektverzeichnis aus um das Framework zu konfigurieren.
2. Wähle 'Serial flasher config' aus. Ändere den 'Default serial port' zu ```/dev/ttyUSB0```. Das wird nur vorausgesetzt, wenn MacOS or Linux verwendet wird. Unter Windows Hosts wird es nicht benötigt, da es keine Unterstützung für 'Device Assignments' gibt.
3. Wähle 'Flash SPI mode' aus und wähle anschließend 'DIO' aus.
4. Speichere die Einstellungen und verlasse das Konfigurationsmenü.

<a name="build"></a>
## 4. Bauen, Flashen und Monitoring

1. Baue das Projekt mit ```$ make``` aus dem Projektverzeichnis.
2. Führe ```$ make flash``` aus, um den nodeMCU zu flashen. Dieser Schritt funktioniert nur unter MacOS und Linux, jedoch nicht unter Windows Hosts.
3. Verbinde den nodeMCU nach dem Flashen. Um das Monitoring auszuführen führe ```$ make monitor``` aus. Dieser Schritt funktioniert nur unter MacOS und Linux, jedoch nicht unter Windows Hosts.

<a name="led_example"></a>
## 5. Bauen und Flashen des LED-Beispiels

1. Klone das Git Repository:<br>```$ git clone git@gitlab.mi.hdm-stuttgart.de:embedded/ss19/easy-grow.git```
2. Wechsle in das Projektverzeichnis ```easy-grow```:<br>```$ cd easy-grow```
3. Checke den ```example``` Branch aus:<br>```$ git checkout example```
4. Wechsle in das ```docker``` Verzeichnis:<br>```$ cd docker```
5. Verbinde den ESP8266 nodeMCU mittels USB.
6. Führe den ```docker.sh``` Script aus. Dieser Script erzeugt das Docker Image ```docker-esp8266``` und initialisier einen Container mit der ESP IDF Umgebung.<br>```$ ./docker.sh```
7. Wechsle innerhalb des Containers zum ```easy-grow``` Projektverzeichnis:<br>```$ cd easy-grow```
8. Konfiguriere den 'serial flasher' der ESP IDF. Weitere Informationen befinden sich hier: [2. Configuring the Espressif IoT Development Framework](#idf_config).
9. Baue das Projekt, flashe den nodeMCU, und aktiviere das Monitoring mit:<br>```$ make && make flash && make monitor```.<br> Falls eine Fehlermeldung erscheint, führe den folgenden Befehl erneut aus: ```$ make flash && make monitor```.

<a name="changelog"></a>
## 6. [Changelog](changelog.md)
