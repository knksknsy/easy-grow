FROM ubuntu:16.04

LABEL maintainer="Kaan Keskinsoy kk148@hdm-stuttgart.de"

ENV PATH="$PATH:$HOME/ESP/xtensa-lx106-elf/bin"
ENV IDF_PATH="$HOME/ESP/ESP8266_RTOS_SDK"
ENV PYTHONPATH="/usr/bin/python"

COPY python_requirements.txt /
COPY ./initial_setup.sh /

RUN DEBIAN_FRONTEND=noninteractive apt-get update -qq \
    && DEBIAN_FRONTEND=noninteractive apt-get install -yq \
        gcc \
        git \
        wget \
        yum \
        unzip \
        make \
        libncurses-dev \
        flex \
        bison \
        gperf \
        nano \
        python \
        python-pip \
        python-serial

RUN chmod +x /initial_setup.sh
RUN /initial_setup.sh
