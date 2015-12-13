#!/usr/bin/env bash

cmake -H${SOURCE_PATH} -B${BUILD_PATH}
cmake --build ${BUILD_PATH}
