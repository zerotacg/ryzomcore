#!/usr/bin/env bash

#apt-add-repository --yes ppa:kervala/ppa

apt-get -qq update

#autoconf \
#automake \
#bison \

apt-get -y install \
build-essential \
cmake \
libluabind-dev \
libcpptest-dev \
libogg-dev \
libvorbis-dev \
libopenal-dev \
libgif-dev \
libfreetype6-dev \
libxml2-dev
