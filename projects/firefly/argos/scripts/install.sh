#!/bin/bash

set -eu -o pipefail #fail on error and report it, debug all lines
export DEBIAN_FRONTEND=noninteractive #disable phpmyadmin prompts during installation

sudo -n true
test $? -eq 0 || exit 1 "you should have sudo privileges to run this script"

echo "Installing the required pre-requisities"
sudo apt-get update --allow-releaseinfo-change -y && sudo apt-get upgrade -y
while read -r p ; do sudo apt-get install -yq $p ; done < <(cat << "EOF"
	perl
	zip unzip
	exuberant-ctags
	curl
	wget
	mariadb-server
	mariadb-client
	php-mysql
	phpmyadmin
	python-pip
	python-flask
EOF
)

echo -e "\n"
echo "Install python libraries..."

sudo pip install flask
sudo pip install PyDrive

echo "Setting up argos service..."

sudo mkdir ~/argos_data
sudo chmod 777 -R ~/argos_data
sudo cp argos-mon.service /etc/systemd/system
sudo systemctl enable argos-mon.service

echo -e "\n"
echo "argos-mon service running...done."
echo "Run <sudo sytemctl start argos-mon.service> to start monitoring"
