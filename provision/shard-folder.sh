#!/usr/bin/env bash

source /vagrant/provision/.profile

function service-prepare-folder() {
    local service_name=$1
    local config=${service_name}
    local config_default=${service_name}_default
    local config_common=common

    make-service-path ${service_name}

    copy-config ${service_name} ${config}
    copy-config ${service_name} ${config_default}
    copy-config ${service_name} ${config_common}
}

function make-service-path() {
    local service_name=$1
    local service_path=${SHARD_PATH}/${service_name}

    mkdir --parents ${service_path}
}

function copy-config () {
    local service_name=$1
    local config_name=$2
    local file=${config_name}.cfg

    copy ${service_name} ${file}
}

function copy () {
    local service_name=$1
    local service_path=${SHARD_PATH}/${service_name}
    local file=$2
    local target=${service_path}/${file}

    cp -R ${RYZOM_SERVER_PATH}/${file} ${service_path}/
}

ln -s ${SHARD_PATH} /home/vagrant/shard

service-prepare-folder admin_service

service-prepare-folder admin_executor_service

service-prepare-folder backup_service
if [ ! -d ${SHARD_PATH}/common ]; then
    cp -R ${RYZOM_PATH}/common ${SHARD_PATH}/
fi

cp -R ${SHARD_PATH}/backup_service ${SHARD_PATH}/backup_pd_service

service-prepare-folder entities_game_service
copy-config entities_game_service used_continents
copy-config entities_game_service backup_service_interface
copy entities_game_service data_shard

service-prepare-folder gpm_service
copy-config gpm_service used_continents

service-prepare-folder input_output_service
copy-config input_output_service backup_service_interface

service-prepare-folder naming_service
#copy-config input_output_service backup_service_interface
