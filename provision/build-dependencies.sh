#!/usr/bin/env bash

apt-get -qq update

apt-get -y install \
build-essential \
cmake \
libcurl4-openssl-dev \
libgl1-mesa-dev \
libmysqlclient15-dev \
libluabind-dev \
libcpptest-dev \
libogg-dev \
libvorbis-dev \
libopenal-dev \
libgif-dev \
libjpeg62-dev \
libpng12-dev \
libfreetype6-dev \
libxml2-dev
