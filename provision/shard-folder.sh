#!/usr/bin/env bash

source /vagrant/provision/.profile

function service-prepare-folder() {
    local service_name=$1
    local service_path=/home/vagrant/${service_name}
    local config=${service_name}
    local config_default=${service_name}_default
    local config_common=common

    mkdir --parents ${service_path}

    config-link ${service_name} ${config}
    config-link ${service_name} ${config_default}
    config-link ${service_name} ${config_common}
}

function config-link () {
    local service_name=$1
    local service_path=/home/vagrant/${service_name}
    local config_name=$2
    local file=${config_name}.cfg
    local link=${service_path}/${file}

    if [ ! -f ${link} ]; then
        ln -s ${RYZOM_SERVER_PATH}/${file} ${service_path}/
    fi
}

service-prepare-folder admin_service
service-prepare-folder admin_executor_service
service-prepare-folder backup_service
ln -s ${RYZOM_PATH}/common ${SHARD_PATH}/common
cp -R ${SHARD_PATH}/backup_service ${SHARD_PATH}/backup_pd_service
service-prepare-folder entities_game_service
config-link entities_game_service used_continents
config-link entities_game_service backup_service_interface