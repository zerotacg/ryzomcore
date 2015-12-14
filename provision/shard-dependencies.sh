#!/usr/bin/env bash

apt-get -qq update

debconf-set-selections <<< 'mysql-server mysql-server/root_password password root'
debconf-set-selections <<< 'mysql-server mysql-server/root_password_again password root'

debconf-set-selections <<< 'phpmyadmin phpmyadmin/mysql/admin-pass password root'
debconf-set-selections <<< 'phpmyadmin phpmyadmin/mysql/app-pass password root'
debconf-set-selections <<< 'phpmyadmin phpmyadmin/setup-password password root'
debconf-set-selections <<< 'phpmyadmin phpmyadmin/password-confirm password root'
debconf-set-selections <<< 'phpmyadmin phpmyadmin/app-password-confirm password root'
debconf-set-selections <<< 'phpmyadmin phpmyadmin/reconfigure-webserver multiselect apache2'
debconf-set-selections <<< 'phpmyadmin phpmyadmin/dbconfig-install boolean true'


apt-get -y install \
apache2 \
php5 \
php5-mcrypt \
mysql-server \
phpmyadmin \
cmake \
mercurial

#libcurl4-openssl-dev \
#libgl1-mesa-dev \
#libmysqlclient15-dev \
#libluabind-dev \
#libogg-dev \
#libvorbis-dev \
#libopenal-dev \
libgif4 \
#libjpeg62-dev \
#libpng12-dev \
#libfreetype6-dev \
#libxml2-dev


php5enmod mcrypt