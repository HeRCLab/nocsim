FROM charlesdaniels/ikos_ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive 
RUN printf 'APT::Get::Assume-Yes "true";\nAPT::Get::force-yes "true";' > /etc/apt/apt.conf.d/90forceyes
RUN apt-get update
RUN apt-get upgrade --yes --force-yes
RUN apt-get install --yes --force-yes tcl-dev libtcl8.6 libbsd-dev libreadline-dev valgrind python3 python3-pip tcllib
RUN pip3 install lizard
