#!/usr/bin/env bash

source /vagrant/provision/.profile

function service-prepare-folder() {
    local service_name=$1
    local service_path=${SHARD_PATH}/${service_name}
    local config=${service_name}
    local config_default=${service_name}_default
    local config_common=common
    local home_path=/vagrant/home/${service_name}

    mkdir --parents ${service_path}

    if [ ! -d ${home_path} ]; then
        ln -s ${service_path} ${home_path}
    fi

    config-copy ${service_path} ${config}
    config-copy ${service_path} ${config_default}
    config-copy ${service_path} ${config_common}
}

function config-copy () {
    local service_path=$1
    local config_name=$2
    local file=${config_name}.cfg
    local target=${service_path}/${file}

    if [ ! -f ${target} ]; then
        cp ${RYZOM_SERVER_PATH}/${file} ${service_path}/
    fi
}

service-prepare-folder admin_service

service-prepare-folder admin_executor_service

service-prepare-folder backup_service
if [ ! -d ${SHARD_PATH}/common ]; then
    cp -R ${RYZOM_PATH}/common ${SHARD_PATH}/
fi

cp -R ${SHARD_PATH}/backup_service ${SHARD_PATH}/backup_pd_service

service-prepare-folder entities_game_service
config-copy entities_game_service used_continents
config-copy entities_game_service backup_service_interface

service-prepare-folder gpm_service
config-copy gpm_service used_continents

service-prepare-folder input_output_service
config-copy input_output_service backup_service_interface

service-prepare-folder naming_service
#config-copy input_output_service backup_service_interface
