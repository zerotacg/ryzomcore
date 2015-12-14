#!/usr/bin/env bash

source /vagrant/provision/.profile

function link-provision () {
    local target=$1
    ln -s /vagrant/provision/${target} ${target}
}

function site-enable () {
    local site=$1

    link-provision /etc/apache2/sites-available/${site}.conf
    a2ensite ${site}
}

site-enable 010-shard
a2dissite 000-default

service apache2 reload

link-provision /etc/init.d/admin-executor-service
link-provision /etc/init.d/admin-service
link-provision /etc/init.d/backup-player-service
link-provision /etc/init.d/backup-service
