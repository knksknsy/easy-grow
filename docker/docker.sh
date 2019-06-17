#!/bin/bash
FULL_PATH="$PWD"
RED='\033[0;31m'
YELLOW='\033[1;33m'
GREEN='\033[0;32m'
NC='\033[0m'

function script_usage {
    echo "This script creates and runs a Docker container from the docker-esp8266 image."
    echo "Run this script inside the docker directory from the projects path."
    echo ""
}

function create_machine {
	echo -e "${RED}Default machine with virtualbox driver does not exist.${NC}"
	echo -e "${GREEN}Creating a default machine with the virtualbox driver...${NC}"
	
	docker-machine create --driver virtualbox default
	
	echo -e "${GREEN}Created machine.${NC}"
	
	echo -e "${YELLOW}Do not forget to export your host's serial ports to the container.${NC}"
	echo -e "${YELLOW}More information on: https://gitlab.mi.hdm-stuttgart.de/embedded/ss19/easy-grow/blob/master/readme.md${NC}"
	echo -e "${YELLOW}In chapter '1.1.3 Export host's serial port to container'.${NC}"
}

function start_machine {
	echo -e "${RED}The default machine is not running.${NC}"
	echo -e "${GREEN}Starting default machine...${NC}"
	
	docker-machine start
	
	echo -e "${GREEN}Setting environment variables for the machine${NC}"
	
	eval "$(docker-machine env default)"
	
	echo -e "${GREEN}Started default machine.${NC}"
}

function build_image {
	echo -e "${RED}Docker image 'docker-esp8266' not created.${NC}"
	echo -e "${GREEN}Creating 'docker-esp8266' image...${NC}"
	
	docker build -t docker-esp8266 .
	
	echo -e "${GREEN}Created Docker image 'docker-esp8266'.${NC}"
}

if [[ $(docker-machine ls | grep -c "default.*virtualbox") -eq 0 ]]; then
	#echo -e "Error: Please create a machine with the virtualbox driver."
	#echo -e "Create a machine with the following: 'docker-machine create --driver virtualbox default'"
	create_machine
fi

if [[ $(docker-machine ls | grep -c "default.*virtualbox.*Running") -eq 0 ]]; then
#	echo -e "Error: The default machine is not running."
#	echo -e "Start the machine with the following: 'docker-machine start'"
	start_machine
fi

# check if docker-esp8266 image is created
if [[ $(docker images | grep -c "docker-esp8266") -eq 0 ]]; then
    #echo -e "Error: Build the missing Docker image: docker-esp8266"
    #echo -e "Run the following line from the docker directory for building the image: 'docker build -t docker-esp8266 .'"
    #exit 1
    build_image
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
    echo -e "${RED}Error: Rerun docker.sh inside the docker directory of the project's root!${NC}"
    exit 1
else
    # Remove "docker" from FULL_PATH. Equivalent to "cd .."
    FULL_PATH=${FULL_PATH%/*}
fi

eval "$(docker-machine env default)"

docker run -ti --rm --name esp8266 --device /dev/ttyUSB0:/dev/ttyUSB0 -v ${FULL_PATH}:/easy-grow docker-esp8266 /bin/bash
