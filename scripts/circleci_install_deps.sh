#!/bin/sh

# This script installs the dependencies required to run the CI scripts in a
# Ubuntu 18 host. It is only intended to be used within CircleCI.

export DEBIAN_FRONTEND=noninteractive
mkdir -p /etc/apt/apt.conf.d/
printf 'APT::Get::Assume-Yes "true";\nAPT::Get::force-yes "true";' > /etc/apt/apt.conf.d/90forceyes
apt-get update
apt-get install --yes --force-yes tcl-dev libtcl8.6 libbsd-dev libreadline-dev
