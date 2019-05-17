#!/bin/bash
FULL_PATH=""
ESP="ESP"
BASH_PROFILE_PATH=$HOME/.bash_profile
# boolean indicating if --dir command argument is provided
DEFAULT=0

function script_usage {
    echo "For installing software environment in the project's root run (default):"
    echo "sudo ./initial_setup.sh"
    echo ""
    echo "For installing software environment in an arbitrary path provide the --dir <path> argument:"
    echo "sudo ./initial_setup.sh --dir <path>"
    echo ""
    echo "The software environment will be installed in the following directory:"
    echo -e "When default (no --dir <path> argument is passed):\t'<project-path>/ESP'"
    echo -e "When --dir <path> argument is passed:\t\t\t'<path>/ESP'"
    echo ""
    echo "The ESP directory will contain following directories when the setup is completed:"
    echo -e "ESP/xtensa-lx106-elf:\tcontains the toolchain to build the application for ESP8266EX"
    echo -e "ESP/ESP8266_RTOS_SDK:\tcontains the API and scripts to operate the toolchain"
}

# Check if script is run as root
if [[ $EUID -ne 0 ]]; then
    echo -e "Error: Rerun initial_setup.sh as root!"
    exit 1
fi

# No command arguments passed
if [[ "$#" -eq 0 ]]; then
    DEFAULT=1
    FULL_PATH="$PWD"
    # Check if active directory is "setup"
    if [[ ${FULL_PATH##/*/} != "setup" ]]; then
        echo -e "Error: Rerun initial_setup.sh inside the setup directory of the project's root!"
        exit 1
    else
        # Remove "setup" from FULL_PATH. Equivalent to "cd .."
        FULL_PATH=${FULL_PATH%/*}
    fi
fi

# Command arguments passed
if [[ $DEFAULT -eq 0 ]]; then
    # Get command arguments
    while [[ "$#" -gt 0 ]]; do
        case $1 in
            -d | --dir )            shift
                                    if [[ "$1" == "" ]]; then
                                        script_usage
                                        exit 1
                                    fi
                                    FULL_PATH=$1
                                    ;;
            -h | --help )           script_usage
                                    exit
                                    ;;
            * )                     script_usage
                                    exit 1
        esac
        shift
    done
    # Remove trailing "/" of path if present
    if [[ "${FULL_PATH: -1}" == "/" ]]; then
        FULL_PATH=${FULL_PATH%?}
    fi
    # Create directory if it does not exist
    if [[ ! -d $FULL_PATH ]]; then
        echo `mkdir -p $FULL_PATH`
    fi
fi

function main {
    install_toolchain
    set_env_vars
    install_rtos_sdk
}

function fn_cd {
    cd $1
}

function install_toolchain {
    echo "Installing toolchain for ESP8266..."

    if hash pip; then
        install_pyserial
    else
        if hash easy_install; then
            install_pip
            install_pyserial
        else
            echo -e `hash easy_install`
            echo -e "Error: Please install easy_install!"
            exit 1
        fi
    fi

    echo `mkdir -p $FULL_PATH/$ESP`
    fn_cd "$FULL_PATH/$ESP"

    echo "Downloading ESP8266 toolchain from Espressif..."
    echo `wget https://dl.espressif.com/dl/xtensa-lx106-elf-osx-1.22.0-92-g8facf4c-5.2.0.tar.gz`
    echo `tar -xzf xtensa-lx106-elf-osx-1.22.0-92-g8facf4c-5.2.0.tar.gz`

    fn_cd "$FULL_PATH"
    echo `chmod -R 777 $ESP`
    fn_cd "$FULL_PATH/$ESP"

    echo "Toolchain for ESP8266 successfully installed into directory: $FULL_PATH/$ESP/xtends-lx106-elf/"
}

function set_env_vars {
	echo "export PATH=$FULL_PATH/$ESP/xtensa-lx106-elf/bin:\$PATH" >> $BASH_PROFILE_PATH
	echo "export IDF_PATH=$FULL_PATH/$ESP/ESP8266_RTOS_SDK" >> $BASH_PROFILE_PATH
    PYTHON=$(eval which python)
    echo "export PYTHONPATH=$PYTHON" >> $BASH_PROFILE_PATH
    echo "Environment variables set to $BASH_PROFILE_PATH"
}

function install_rtos_sdk {
    echo "Installing ESP8266_RTOS_SDK..."

    echo "Cloning git repository https://github.com/espressif/ESP8266_RTOS_SDK.git"
    echo `git clone --recursive https://github.com/espressif/ESP8266_RTOS_SDK.git`
    echo "Checking out commit d83c9f7866e59dcbb254ce5366c27418d410e84e"
    fn_cd "$FULL_PATH/$ESP/ESP8266_RTOS_SDK"
    echo `git checkout d83c9f7866e59dcbb254ce5366c27418d410e84e`
    echo "ESP8266_RTOS_SDK successfully installed into directory: $FULL_PATH/$ESP/ESP8266_RTOS_SDK"
}

function install_pyserial {
    echo "Installing pyserial via pip..."
    echo `pip install pyserial`
    echo "pyserial successfully installed!"
}

function install_pip {
    echo "Installing pip via easy_install..."
    echo `easy_install pip`
    echo "pip successfully installed!"
}

main
