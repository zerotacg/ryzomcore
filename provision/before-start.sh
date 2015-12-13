#!/usr/bin/env bash

source /vagrant/provision/.profile

function service-prepare-folder() {
    local service_name=$1
    local service_path=/home/vagrant/${service_name}
    local config=${service_name}.cfg
    local config_default=${service_name}_default.cfg
    local config_common=common.cfg

    mkdir --parents ${service_path}
    ln -s ${RYZOM_SERVER_PATH}/${config} ${service_path}/${config}
    ln -s ${RYZOM_SERVER_PATH}/${config_default} ${service_path}/${config_default}
    ln -s ${RYZOM_SERVER_PATH}/${config_common} ${service_path}/${config_common}
}

service-prepare-folder admin_service
service-prepare-folder admin_executor_service
