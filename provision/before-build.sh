#!/usr/bin/env bash

cat /vagrant/provision/.profile >> ~/.profile

source ~/.profile

if [ ! -d ${SOURCE_PATH} ]; then
    echo "link: ln -s /vagrant/code ${SOURCE_PATH}"
    ln -s /vagrant/code ${SOURCE_PATH}
fi

mkdir --parents ${BUILD_PATH}
