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
	git checkout c343b45ad5c5c1be503d40f5dce13b787c08725d && \
	cd /tmp/libagar && ./configure --enable-debug --enable-debug-core --disable-sg --disable-sk --disable-map && \
	make && make install && cd / && rm -rf /tmp

# we don't get /tmp by default, but ./configure needs it
RUN mkdir -p /tmp && chmod 1777 /tmp

# we want to use the host locales and terminfo
RUN rm -rf /usr/share/i18n/locales ; rm -rf /usr/lib/locale/locale-archive ; rm -rf /etc/terminfo
