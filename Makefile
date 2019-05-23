#
# Easy Grow Makefile
#


#IDF_PATH := $(HOME)/esp/ESP8266_RTOS_SDK
#BIN := tools/esp/xtensa-lx106-elf/bin
#PATH := tools/esp/xtensa-lx106-elf/bin

PROJECT_NAME := easy_grow

COMPONENT_ADD_INCLUDEDIRS := $(PWD)/components/fsdata $(IDF_PATH)/components/lwip/lwip/src/include/lwip
COMPONENT_ADD_INCLUDEDIRS := $(PWD)/components/fsdata/fs $(pwd)/components/extras/httpd 

include $(IDF_PATH)/make/project.mk


CFLAGS+=-I$(PWD)/components/fsdata \
	-I$(PWD)/components/fsdata/fs \
	-I$(PWD)/components/extras/httpd \
	#-I$(IDF_PATH)/components/lwip/lwip/src/include/lwip/

html:
	@echo "Generating fsdata.."
	cd components/fsdata && ./makefsdata















