#!/usr/bin/env bash

source /vagrant/provision/.profile

SHARD_DATA_PATH=${SHARD_PATH}/ryzomcore-data
hg clone https://@bitbucket.org/ryzom/ryzomcore-data ${SHARD_DATA_PATH}

ln -s ${SHARD_DATA_PATH}/leveldesign/game_elem/sheet_id.bin ${SHARD_PATH}/backup_service/sheet_id.bin