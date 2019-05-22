## Easy Grow Code Page

Easy Grow is a C++ Program to take care of your plants.

## Table of Contents
1. [Setting up the software environment](#sw_env)  

    1.1 [ESP8266 Toolchain with Docker](#tool_docker)

    1.1.1 [Install Docker](#inst_docker)

    1.1.2 [Build Docker Image](#build_docker)

    1.1.3 [Export host's serial port to container](#serial_port)

    1.1.4 [Run the Docker Container](#cont_docker)

    1.2 [ESP8266 Toolchain to your Local Machine (MacOS only)](#local)

2. [Configuring the Espressif IoT Development Framework](#idf_config)

3. [Building, flashing, and monitoring](#build)

<a name="sw_env"></a>
## 1. Setting up the software environment

There are two options for setting up the esp8266 software environment:
1. Docker (Linux Ubuntu 16.04 64bit)
2. Local machine (supports only MacOS)

The recommended way for the setup is Docker.

<a name="tool_docker"></a>
### 1.1. ESP8266 Toolchain with Docker

<a name="inst_docker"></a>
#### 1.1.1 Install Docker

1. Install Brew: https://brew.sh
2. Install Docker Engine and Docker-Machine:<br/>```$ brew install docker```
3. Add Cask to your brew tap:<br/>```$ brew tap caskroom/cask```
3. Install Docker Client:<br/>```$ brew cask install docker```
4. Optional: If virtualbox is missing after step 2, install virtualbox with:<br/>```$ brew cask install virtualbox```

<a name="build_docker"></a>
#### 1.1.2 Build Docker Image

The Docker Image only needs to be built once. You can check if the Image is already installed with the following line: ```$ docker images```

If it prints the ```docker-esp8266``` Image, you can skip the build step.
If not continue with the following steps:

1. Change your directory to where the ```Dockerfile``` is placed:<br/>```$ cd <project_path>/docker```
2. Build the Docker Image: ```$ docker build -t docker-esp8266 .```

Step 2 will build an Ubuntu Image and install the required packages for the esp8266 toolchain. Afterwards it will also run a script, which installs the toolchain.

<a name="serial_port"></a>
#### 1.1.3 Export host's serial port to container

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

You are now able to export the host's serial port to the Docker container. How to run a Docker container will be shown in the next chapter.

<a name="cont_docker"></a>
#### 1.1.4 Run the Docker Container

Make sure you have installed Docker, built the ```docker-esp8266``` Image, and enabled exporting the host's serial ports to the container before you continue with running a Docker container.

1. Change your directory to: ```$ cd <project_path>/docker```
2. Start the container with the script: ```$ ./docker.sh```. This script enables you to start a container from the ```docker-esp8266``` image. It also mounts the project directory to the ```/easy-grow``` directory of the container. It also exports the host's ```/dev/ttyUSB0``` port to the container's ```/dev/ttyUSB0``` port.

<a name="local"></a>
### 1.2 ESP8266 Toolchain to your Local Machine (MacOS only)

Navigate into the ```setup``` directory in the project's root directory in order to set up the software environment for the ESP8266EX.

There you'll find the ```initial_setup.sh``` shell script which will automatically set up the followings:

- Toolchain to build the application for ESP8266EX
- ESP8266_RTOS_SDK that contains the API and scripts to operate the toolchain.

Pass the ```--dir <path>``` command argument to install the software environment to an arbitrary path.
If this argument is omitted the software environment will be installed in the ```ESP``` directory of the project's root by default.

The toolchain is placed in ```<path>/ESP/xtensa-lx106-elf``` whereas the SDK is placed in ```<path>/ESP/ESP8266_RTOS_SDK```.

Open a new terminal window and paste the following commands in order to trigger the setup:
- Navigate into the ```setup``` directory in the project's root directory:<br>```$ cd setup```
- Execute the shell script as user (not root):<br>```$ ./initial-setup.sh```
- Wait until the setup is completed
- Don't forget to source your ```bash_profile``` after the setup:<br> ```$ source ~/.bash_profile```

<a name="idf_config"></a>
## 2. Configuring the Espressif IoT Development Framework

1. Run ```$ make menuconfig``` in the project's root directory to configure the framework.
2. Select 'Serial flasher config'. Then change the 'Default serial port' to ```/dev/ttyUSB0```.
3. Select 'Flash SPI mode' and change it to 'DIO'.
4. Save the settings and exit the configuration menu.

<a name="build"></a>
## 3. Building, flashing, and monitoring

1. Build the project with ```$ make``` from the projects root directory.
2. Execute ```$ make flash``` for flashing the nodeMCU.
3. If it's finished and connected to the nodeMCU you can monitor the nodeMCU with ```$ make monitor```.