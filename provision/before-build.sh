#!/usr/bin/env bash

cat /vagrant/provision/.profile > ~/.profile

source /vagrant/provision/.profile

mkdir --parents ${BUILD_PATH}
