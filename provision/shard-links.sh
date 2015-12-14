#!/usr/bin/env bash

source /vagrant/provision/.profile

function link-provision () {
    local source=$1
    local target=${source#/vagrant/provision}

    echo "creating link: ${source} -> ${target}"
    ln -s ${source} ${target}
}

for i in $(find /vagrant/provision/etc -type f); do
    link-provision "$i"
done


a2dissite 000-default
a2ensite 010-shard

service apache2 reload
