# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure(2) do |config|
    config.vm.box = "ubuntu/trusty64"

    config.vm.provider "virtualbox" do |vb|
        vb.memory = "2048"
    end

    config.vm.provision "common-links",
        type: "shell",
        privileged: false,
        path: "provision/common-links.sh"

    config.vm.define "build", autostart: false do |build|
        build.vm.provision "shell",
            path: "provision/build-dependencies.sh"

        build.vm.provision "build",
            type: "shell",
            privileged: false,
            path: "provision/build.sh"
    end

    config.vm.define "shard", primary: true do |shard|
        shard.vm.provision "shard-dependencies",
            type: "shell",
            path: "provision/shard-dependencies.sh"

        shard.vm.provision "shard-links",
            type: "shell",
            path: "provision/shard-links.sh"

        shard.vm.provision "shard-folder",
            type: "shell",
            privileged: false,
            path: "provision/shard-folder.sh"

        shard.vm.provision "shard-data-local",
            type: "shell",
            privileged: false,
            path: "provision/shard-data-local.sh"

        shard.vm.provision "shard-data-remote",
            type: "shell",
            privileged: false,
            path: "provision/shard-data-remote.sh"

        shard.vm.provision "sheet-id-bin",
            type: "shell",
            privileged: false,
            path: "provision/sheet-id-bin.sh"

        shard.vm.provision "packed-sheets",
            type: "shell",
            privileged: false,
            path: "provision/packed-sheets.sh"

        shard.vm.network "private_network", :type => 'dhcp', :name => 'vboxnet0', :adapter => 2
    end
end
