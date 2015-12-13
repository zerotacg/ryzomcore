# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure(2) do |config|
    config.vm.box = "ubuntu/trusty64"

    config.vm.provider "virtualbox" do |vb|
        vb.memory = "2048"
    end

    config.vm.provision "shell",
        path: "provision/dependencies.sh"

    config.vm.provision "before-build",
        type: "shell",
        privileged: false,
        path: "provision/before-build.sh"

    config.vm.provision "build",
        type: "shell",
        privileged: false,
        path: "provision/build.sh"

    config.vm.define "shard" do |shard|
    end
end
