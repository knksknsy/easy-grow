#!/bin/bash

# check if docker-esp8266 image is created
DOCKER_IMAGE=$(docker images | grep -c "docker-esp8266")
FULL_PATH="$PWD"

function script_usage {
    echo "This script creates and runs a Docker container from the docker-esp8266 image."
    echo "Run this script inside the docker directory from the projects path."
    echo ""
}

if [[ $DOCKER_IMAGE -eq 0 ]]; then
    echo -e "Error: Build the missing Docker image: docker-esp8266"
    echo -e "Run the following line from the docker directory for building the image: 'docker build -t docker-esp8266 .'"
    exit 1
fi

if [[ "$#" -gt 0 ]]; then
    while [[ "$#" -gt 0 ]]; do
        case $1 in
            -h | --help )           script_usage
                                    exit 1
                                    ;;
            * )                     script_usage
                                    exit 1
        esac
        shift
    done
fi

if [[ ${FULL_PATH##/*/} != "docker" ]]; then
    echo -e "Error: Rerun docker.sh inside the docker directory of the project's root!"
    exit 1
else
    # Remove "docker" from FULL_PATH. Equivalent to "cd .."
    FULL_PATH=${FULL_PATH%/*}
fi

docker run -ti --rm --name esp8266 --device /dev/ttyUSB0:/dev/ttyUSB0 -v ${FULL_PATH}:/easy-grow docker-esp8266 /bin/bash
