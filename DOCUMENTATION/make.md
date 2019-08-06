# Includierung von Dateien mit Make

Das vordefinierte Make-File des SDKs bietet verschiedene Möglichkeiten Dateien einzubinden. Dafür benötigt jedes Projekt ein eigenes Makefile, das auf das SDK-Makefile verweist:
```
PROJECT_NAME := easy_grow

include $(IDF_PATH)/make/project.mk
```
In diesem können weitere Include-Pfade spezifiziert werden, dies funktioniert in Abhängigkeit der Commitversion sowie des Pfades dennoch nicht zuverlässig. 
```COMPONENT_ADD_INCLUDEDIRS```
```COMPONENT_SRCDIRS```

Daher wurden in dem Projekt EasyGrow Pseudo-Makefiles in die relevanten Ordner eingefügt. Diese leeren ```component.mk``` Dateien werden im Buildprozess erkannt und Dateien auf gleiche Dateiebene hinzugefügt.

## Flash-Argumente
```make build``` ruft den Buildprozess auf und die erzeugte Firmware kann mit  ```make flash``` auf den Microkontroller übertragen werden. 
```make flash``` nutzt hierbei die Einstellungen aus der sdkconfig. Diese Datei kann manuell erzeugt oder mit ```make menuconfig``` generiert werden. In ihr sind Daten wie die Baudrate und der USB-Port enthalten. 
Diese Einstellungen können mit ```make print_flash_cmd``` ausgegeben werden und bei direktem flashen über das Phyton -Programm esptool.py direkt gesetzt werden:
```python esptool.py --chip esp8266 --port /dev/ttyUSB0 --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode qio --flash_freq 40m --flash_size detect 0 bootloader/bootloader.bin 0x10000 example_app.bin 0x8000 partitions_singleapp.bin
```
Der Aufruf über das esptool erfolgt innerhalb des Makefiles und bildet somit keinen differenten Flashprozess ab.


