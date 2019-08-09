## Easy Grow Code Page

Easy Grow is a C++ Program to take care of your plants.

## External Components
The project uses modified parts of the dns server from libesphttpd.
See the file easy_dns.c for the licence and https://github.com/Spritetm/libesphttpd/blob/master/util/captdns.c for the original file.


## Table of Contents
- [1. Dokumentation](#docu) 
    * [1.1 Stromversorgung](#stromversorgung)
    * [1.1.1 Stromverbrauch](#stromvebrauch)
    * [1.1.2 Batteriebetrieb](#batteriebetrieb)
    * [1.2 File includes with make](#make)
        * [1.2.1 Flash-Argumente](#flash_argumente)
        * [1.2.2 Building Documentation with make](#make_documentation)
    * [1.3 Systembausteine](#systembausteine)
        * [1.3.3 Easy_DNS](#dns)
    
 
    
    
- [2. Setting up the software environment](#sw_env)  
    * [2.1 ESP8266 toolchain setup using Docker](#tool_docker)
        + [2.1.1 Install Docker](#inst_docker)
            - [2.1.1.1 MacOS](#inst_docker_mac)
            - [2.1.1.2 Windows](#inst_docker_win)
        + [2.1.2 Build Docker image](#build_docker)
        + [2.1.3 Export host's serial port to container](#serial_port)
            - [2.1.3.1 MacOS](#serial_port_mac)
            - [2.1.3.2 Windows](#serial_port_windows)
        + [2.1.4 Run the Docker container](#cont_docker)
            - [2.1.4.1 MacOS](#cont_docker_mac)
            - [2.1.4.2 Windows](#cont_docker_win)
    * [2.2 ESP8266 toolchain setup to your local machine (MacOS only)](#local)
- [3. Configuring the Espressif IoT Development Framework](#idf_config)
- [4. Building, flashing, and monitoring](#build)
- [5. Build and flash LED example](#led_example)
- [6. Changelog](#changelog)
<!-- toc -->

<a name="docu"></a>
## 1. Dokumentation
<a name="stromversorgung"></a>
### 1.1 [Stromversorgung]

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

#### 1.2.2 
```make documentation``` bietet die Möglichkeit die aktuelle Dokumentation aus der Datei readme.md in ein HTML-File umzuwandeln und diese im Anschluss auf einer Webseite des ESP anzuzeigen. Für die Erstellung des HTMLs wird Pandoc https://pandoc.org/ benötigt. Der Benutzer kann sich somit die aktuelle Dokumentation des Projekts in dem produktiven System anzeigen lassen. Jedoch werden Bilder dabei nicht abgebildet.


<a name="systembausteine"></a>
### 1.3 [Systembausteine]

<a name="dns"></a>
#### [1.3.3 Easy_DNS]






<a name="sw_env"></a>
## 2. Setting up the software environment

There are two options for setting up the esp8266 software environment:
1. Docker image (Linux Ubuntu 16.04 64bit)
2. Local machine (MacOS only)

It's recommended to use Docker for the setup.

<a name="tool_docker"></a>
### 2.1. ESP8266 toolchain setup using Docker

<a name="inst_docker"></a>
#### 2.1.1 Install Docker

<a name="inst_docker_mac"></a>
##### 2.1.1.1 MacOs

1. Install Brew: https://brew.sh
2. Install Docker Engine and Docker-Machine:<br/>```$ brew install docker```
3. Add Cask to your brew tap:<br/>```$ brew tap caskroom/cask```
3. Install Docker Client:<br/>```$ brew cask install docker```
4. Optional: If virtualbox is missing after step 2, install virtualbox with:<br/>```$ brew cask install virtualbox```

<a name="inst_docker_win"></a>
##### 2.1.1.2 Windows

Follow the guide on https://docs.docker.com/docker-for-windows.

<a name="build_docker"></a>
#### 2.1.2 Build Docker image

The Docker Image only needs to be built once. You can check if the Image is already installed with the following line: ```$ docker images```

If it prints the ```docker-esp8266``` image, you can skip the build step.
If not continue with the following steps:

1. Change your directory to where the ```Dockerfile``` is placed:<br/>```$ cd <project_path>/docker```
2. Build the Docker image: ```$ docker build -t docker-esp8266 .```

Step 2 will build an Ubuntu image and install the required packages for the esp8266 toolchain. Afterwards it will also run a script, which installs the toolchain.

<a name="serial_port"></a>
#### 2.1.3 Export host's serial port to container

<a name="serial_port_mac"></a>
##### 2.1.3.1 MacOS

If you're working on MacOS make sure you have installed the CP210x USB to UART Bridge VCP Driver.

Get the driver from https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers and follow the instructions on Silicon Labs website.

Check if the driver was installed properly. The following line ```$ ls /dev/tty.*``` should print ```/dev/tty.SLAB_USBtoUART```.

After the installation you are ready to export this serial port to the container. The next step is to create a virtualbox driver for your docker-machine.

1. ```$ docker-machine create --driver virtualbox default```
2. Check if it's created with: ```$ docker-machine ls```
3. Stop docker-machine with: ```$ docker-machine stop```
4. Now you are able to configure the VM and export the serial port. Open the virtualbox application. Select the 'default' VM and click on the 'Settings' icon to configure the USB settings.
5. Select 'Ports' then the 'USB' tab. Activate the 'Enable USB Controller' checkbox. Select 'USB 2.0 (EHCI) Controller'. Add a new USB filter (USB icon with green plus sign). Select 'Silicon Labs CP2102 USB to UART Bridge Controller [0100]'.
6. You may have to install the 'Oracle VM VirtualBox Extension Pack' for enabling the USB 2.0 settings. Follow the instructions on https://www.virtualbox.org/wiki/Downloads for the installation.
7. Start docker-machine with: ```$ docker-machine start```
8. We need to set a few environment variables to tell Docker to use the new VM instead of the native mode. ```$ docker-machine env```command tells us how to do that. All you have to do is: ```& eval "$(docker-machine env default)"```

You are now able to export the host's serial port to the Docker container.

<a name="serial_port_win"></a>
##### 2.1.3.2 Windows

Unfortunately there is no support yet for device assignment and sharing workloads in Hyper-V-isolated Windows containers.

<a name="cont_docker"></a>
#### 2.1.4 Run the Docker container

Make sure you have installed Docker, built the ```docker-esp8266``` image, and enabled exporting the host's serial ports to the container before you continue with running a Docker container.

There are two options for instantiating a container from the ```docker-esp8266``` image:

1. Conveniently with the the ```docker.sh``` script.<br/>(MacOS only)
2. Over with the ```docker run``` command.<br/>(Windows only)

<a name="cont_docker_mac"></a>
##### 2.1.4.1 MacOS

1. Change your directory to: ```$ cd <project_path>/docker```
2. Start the container with the script: ```$ ./docker.sh```. This script enables you to start a container from the ```docker-esp8266``` image. It also mounts the project directory to the ```/easy-grow``` directory of the container. It also exports the host's ```/dev/ttyUSB0``` port to the container's ```/dev/ttyUSB0``` port.

<a name="cont_docker_win"></a>
##### 2.1.4.2 Windows

Just run the following line with the respective ```<project_path>```:<br/>
```docker run -ti --rm --name esp8266 -v <project_path>:/easy-grow docker-esp8266 /bin/bash```

<a name="local"></a>
### 2.2 ESP8266 toolchain setup to your local machine (MacOS only)

Navigate into the ```setup``` directory in the project's root in order to set up the software environment for the ESP8266.

There you'll find the ```initial_setup.sh``` script which will automatically set up the followings:

- Toolchain to build the application for ESP8266
- ESP8266_RTOS_SDK that contains the API and scripts to operate the toolchain.

Pass the ```--dir <path>``` command argument to install the software environment to an arbitrary path.
If this argument is omitted the software environment will be installed in the ```ESP``` directory of the project's root by default.

The toolchain is placed in ```<path>/ESP/xtensa-lx106-elf``` whereas the SDK is placed in ```<path>/ESP/ESP8266_RTOS_SDK```.

Open a new terminal window and paste the following commands in order to trigger the setup:
- Navigate into the ```setup``` directory in the project's root:<br>```$ cd setup```
- Execute the script as user (not root):<br>```$ ./initial-setup.sh```
- Wait until the setup is completed
- Don't forget to source your ```bash_profile``` after the setup:<br> ```$ source ~/.bash_profile```

<a name="idf_config"></a>
## 3. Configuring the Espressif IoT Development Framework

1. Run ```$ make menuconfig``` in the project's root directory to configure the framework.
2. Select 'Serial flasher config'. Then change the 'Default serial port' to ```/dev/ttyUSB0```. This is only required if you're using MacOS or Linux. This is not required on Windows host since there is no support for device assignments.
3. Select 'Flash SPI mode' and change it to 'DIO'.
4. Save the settings and exit the configuration menu.

<a name="build"></a>
## 4. Building, flashing, and monitoring

1. Build the project with ```$ make``` from the projects root directory.
2. Execute ```$ make flash``` for flashing the nodeMCU. This is only possible with MacOS and Linux hosts and not on Windows.
3. If it's finished and connected to the nodeMCU you can monitor the nodeMCU with ```$ make monitor```. This is only possible with MacOS and Linux hosts and not on Windows.

<a name="led_example"></a>
## 5. Build and flash LED example

1. Clone the Git repository:<br>```$ git clone git@gitlab.mi.hdm-stuttgart.de:embedded/ss19/easy-grow.git```
2. Navigate to the ```easy-grow``` directory:<br>```$ cd easy-grow```
3. Checkout the ```example```branch:<br>```$ git checkout example```
4. Navigate to the ```docker```directory:<br>```$ cd docker```
5. Plug your ESP8266 nodeMCU into your USB port
6. Run the ```docker.sh``` script inside of the directory. This will create the Docker image ```docker-esp8266``` and initialize a container with the ESP IDF environment.<br>```$ ./docker.sh```
7. Inside the container navigate to the ```easy-grow``` project directory:<br>```$ cd easy-grow```
8. Configure the ESP IDF serial flasher. For reference see: [2. Configuring the Espressif IoT Development Framework](#idf_config).
9. Build the project, flash the nodeMCU, and monitor with:<br>```$ make && make flash && make monitor```.<br> If an error happens during flashing then rerun the following ```$ make flash && make monitor```.

<a name="changelog"></a>
## 6. [Changelog](changelog.md)






