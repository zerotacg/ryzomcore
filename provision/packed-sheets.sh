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

SHEETS_PACKER_PATH=${SHARD_PATH}/tools/sheets_packer/
copy-shard tools/sheets_packer/sheets_packer.cfg
(cd ${SHEETS_PACKER_PATH} && sheets_packer)
cp --force ${SHARD_PATH}/client/data/*.packed_sheets ${SHARD_PATH}/entities_game_service/data_shard/