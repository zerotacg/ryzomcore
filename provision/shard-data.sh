#!/usr/bin/env bash

source /vagrant/provision/.profile

SHARD_DATA_PATH=${SHARD_PATH}/ryzomcore-data
SHEET_ID_BIN=${SHARD_DATA_PATH}/leveldesign/game_elem/sheet_id.bin

hg clone https://@bitbucket.org/ryzom/ryzomcore-data ${SHARD_DATA_PATH}

ln -s ${SHEET_ID_BIN} ${SHARD_PATH}/backup_service/
ln -s ${SHEET_ID_BIN} ${SHARD_PATH}/backup_pd_service/
