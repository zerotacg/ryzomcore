#!/usr/bin/env bash

source /vagrant/provision/.profile

mkdir --parents ${BUILD_PATH}
cmake -H${SOURCE_PATH} -B${BUILD_PATH}
cmake --build ${BUILD_PATH}
