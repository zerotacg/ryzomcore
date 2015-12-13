#!/usr/bin/env bash

source /vagrant/provision/.profile

echo "cmake --build ${BUILD_PATH} --target install"
cmake --build ${BUILD_PATH} --target install
