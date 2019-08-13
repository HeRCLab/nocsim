FROM charlesdaniels/ikos_ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive
RUN printf 'APT::Get::Assume-Yes "true";\nAPT::Get::force-yes "true";' > /etc/apt/apt.conf.d/90forceyes
RUN apt-get update
RUN apt-get upgrade --yes --force-yes
RUN apt-get install --yes --force-yes tcl-dev libtcl8.6 libbsd-dev libreadline-dev valgrind python3 python3-pip tcllib
RUN pip3 install lizard
RUN apt-get install --yes --force-yes libfreetype6-dev libxinerama-dev libfontconfig-dev libsdl-dev libpng-dev libjpeg-dev
RUN git clone https://github.com/JulNadeauCA/libagar.git /tmp/libagar/ && \
	cd /tmp/libagar && \
	git checkout 28b2d84f127f18c17007f3f75dfd96a1939dffe2 && \
	cd /tmp/libagar && ./configure --disable-sg --disable-sk --disable-map && \
	make && make install && cd / && rm -rf /tmp
