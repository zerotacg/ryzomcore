#!/usr/bin/env bash

source /vagrant/provision/.profile

cmake -H${SOURCE_PATH} -B${BUILD_PATH}
cmake --build ${BUILD_PATH}
