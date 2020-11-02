FROM ubuntu:20.04

RUN apt-get update
RUN export DEBIAN_FRONTEND=noninteractive; \
    export DEBCONF_NONINTERACTIVE_SEEN=true; \
    apt-get -y install \
    cmake \
    g++ \
    git \
    libprotobuf-dev \
    libqt5svg5-dev \
    libshp-dev \
    protobuf-compiler \
    qtbase5-dev \
    qtdeclarative5-dev

COPY build-tilegenerator.sh /usr/bin/
