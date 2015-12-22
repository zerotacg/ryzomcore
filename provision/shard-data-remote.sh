#!/usr/bin/env bash

source /vagrant/provision/.profile

if [ ! -d ${SHARD_DATA_PATH} ]; then
    hg clone https://bitbucket.org/ryzom/ryzomcore-data ${SHARD_DATA_PATH}
fi

function copy-data() {
    local source=$1
    local target=$2

    cp --force --recursive ${SHARD_DATA_PATH}/${source} ${SHARD_PATH}/${target}
}

copy-data common common/data_common
copy-data leveldesign common/data_leveldesign/
copy-data primitives common/data_leveldesign/
