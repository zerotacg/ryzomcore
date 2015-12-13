#!/usr/bin/env bash

cat /vagrant/provision/.profile >> ~/.profile

source ~/.profile

ln -s /vagrant/code ${SOURCE_PATH}
mkdir --parents ${BUILD_PATH}
