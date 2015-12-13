#!/usr/bin/env bash

source /vagrant/provision/.profile

echo "cmake --build ${BUILD_PATH} --target install"
cmake --build ${BUILD_PATH} --target install


function site-enable () {
    local site=$1

    link-provision /etc/apache2/sites-available/${site}.conf
    a2ensite ${site}
}

function link-provision () {
    local target=$1
    ln -s /vagrant/provision/${target} ${target}
}

site-enable 010-shard
