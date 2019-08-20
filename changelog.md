# CHANGELOG
## ReleaseNumber.VersionNumber.BuildNumber (dd.mm.yyyy)
- releasenumber is updated with every release
- versionnumber is increased with every new feature
- buildnumber with fixes


## 0.1.0 (23.04.2019)

- Main: Initial Commit

## 0.2.0 (30.04.2019)
- HttpServer: Custom Responds
- ComponentFolder: Removed

## 0.2.1 (03.05.2019)
- HttpServer: Display of Website

## 0.3.0 (08.05.2019)
- GPIO: Added Input Methods
- GPIO: Fixed wrong declaration

## 0.4.0 (13.05.2019)
- Setup: Added script to install ESP8266 software environment (ESP RTOS SDK and Toolchain) for Linux and Mac hosts
- Setup: Added script for building 'ESP8266 Docker image' and starting container
- Docker: Enable exporting serial port from host machine to Docker container

## 0.5.0 (18.05.2019)
- GPIO: Added Output Methods
- GPIO: Added Interrupt Service Routine for GPIO inputs
- GPIO: Added analog input
- GPIO: Use HW timer to read ACD input every 5 minutes

## 0.6.0 (21.05.2019)
- Gitlab CI: Added automated build jobs

## 0.6.1 (24.05.2019)
- Gitlab CI: Added docker container to CI buildjob
- Gitlab CI: Added SDK Test
- HttpServer: Finished implementation

## 0.7.0 (27.05.2019)
- WiFi: Access point can be initialized

## 0.7.1 (03.06.2019)
- WiFi: Connection to custom WiFi through config page
- HttpServer: Automatically switch from config to moisture task on connection

## 0.7.2 (17.06.2019)
- HttpServer: Moved HTML Code to own class
- Bug Fix: HttpServer was showing same page for config and moisture page

## 0.8.1 (18.06.2019)
- Flash_Writer: writing and reading functions


## 0.9.0 (20.06.2019)
- GPIO: Implemented counting sun hours per day

## 0.10.x (25.06.2019)
- DNSServer: Added DNS Server

## 1.0.0 (30.06.2019)
- Released Version 1.0
- Included all must-haves
