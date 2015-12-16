#!/usr/bin/env bash

source /vagrant/provision/.profile

SHARD_DATA_PATH=${SHARD_PATH}/ryzomcore-data
SHEET_ID_BIN=${SHARD_DATA_PATH}/leveldesign/game_elem/sheet_id.bin

if [ ! -d ${SHARD_DATA_PATH} ]; then
    hg clone https://@bitbucket.org/ryzom/ryzomcore-data ${SHARD_DATA_PATH}
fi

cp ${SHEET_ID_BIN} ${SHARD_PATH}/backup_service/
cp ${SHEET_ID_BIN} ${SHARD_PATH}/backup_pd_service/
