#!/usr/bin/env bash

source /vagrant/provision/.profile

if [ ! -d ${SHARD_DATA_PATH} ]; then
    hg clone https://bitbucket.org/ryzom/ryzomcore-data ${SHARD_DATA_PATH}
fi

SHEETS_PATH=${SHARD_PATH}/sheets
SHEET_ID_BIN=${SHEETS_PATH}/sheet_id.bin

mkdir --parents $SHEETS_PATH

make_sheet_id -o${SHEET_ID_BIN} \
    ${SHARD_DATA_PATH}/leveldesign/game_elem \
    ${SHARD_DATA_PATH}/leveldesign/game_element \
    ${SHARD_DATA_PATH}/leveldesign/world \
    ${SHARD_DATA_PATH}/leveldesign/ecosystems \
    ${SHARD_DATA_PATH}/sound

function copy-sheet-id-bin () {
    local service_name=$1
    local target=${SHARD_PATH}/${service_name}/
    cp --force ${SHEET_ID_BIN} ${target}
}

copy-sheet-id-bin backup_service
copy-sheet-id-bin backup_pd_service
copy-sheet-id-bin entities_game_service
