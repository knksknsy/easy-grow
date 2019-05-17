## Easy Grow Code Page

Easy Grow is a C++ Program to take care of your plants.

## Get Started

### Setting up the software environment

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

## ToDo

Here you see our main points to do:

- Getting started
- Finish Coding