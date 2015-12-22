#!/usr/bin/env bash

source /vagrant/provision/.profile

function copy-shard() {
    local item=$1
    local path=$(dirname $item)
    local target=${SHARD_PATH}/${path}
    local source=${RYZOM_PATH}/${item}

    mkdir --parents ${target}
    cp --force --recursive ${source} ${target}/
}

copy-shard common/data_common
copy-shard common/data_leveldesign
copy-shard client/data
