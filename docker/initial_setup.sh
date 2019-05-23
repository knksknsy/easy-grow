#!/bin/bash
ESP="ESP"

function main {
	install_python_requirements
    install_toolchain
    install_rtos_sdk
}

function fn_cd {
    cd $1
}

function install_python_requirements {
	echo "Installing python requirements..."
    echo ""

    echo `python -m pip install --user -r python_requirements.txt`
    echo `rm -rf python_requirements.txt`
}

function install_toolchain {
    echo "Installing toolchain for ESP8266..."
    echo ""

    echo `mkdir -p /$ESP`
    fn_cd "/$ESP"

    echo "Downloading ESP8266 toolchain from Espressif..."
    echo ""

    echo `wget https://dl.espressif.com/dl/xtensa-lx106-elf-linux64-1.22.0-92-g8facf4c-5.2.0.tar.gz`
    echo `tar -xzf xtensa-lx106-elf-linux64-1.22.0-92-g8facf4c-5.2.0.tar.gz`
    echo `rm -rf xtensa-lx106-elf-linux64-1.22.0-92-g8facf4c-5.2.0.tar.gz`

    echo "Toolchain for ESP8266 successfully installed into directory: /$ESP/xtensa-lx106-elf/"
    echo ""
}

function install_rtos_sdk {
    echo "Installing ESP8266_RTOS_SDK..."
    echo ""

    echo "Cloning git repository https://github.com/espressif/ESP8266_RTOS_SDK.git"
    echo ""

    echo `git clone --recursive https://github.com/espressif/ESP8266_RTOS_SDK.git`
    
    echo "Checking out commit d83c9f7866e59dcbb254ce5366c27418d410e84e"
    fn_cd "/$ESP/ESP8266_RTOS_SDK"
    echo `git checkout d83c9f7866e59dcbb254ce5366c27418d410e84e`
    
    echo "ESP8266_RTOS_SDK successfully installed into directory: /$ESP/ESP8266_RTOS_SDK"
    echo ""
}

main
