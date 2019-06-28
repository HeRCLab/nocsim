#!/bin/sh

set -u

PASS=YES
cd "$(dirname "$0")/.."

echo '#### BUILD ####################################################################'
make clean
if ! make ECFLAGS=-Werror ; then
	PASS=NO
fi



echo '#### LINT #####################################################################'
make clean
make lint

# 1 line header
if [ "$(ikos-report --status-filter error -f csv src/nocsim.db 2>&1 | wc -l)" -gt 1 ] ; then
	PASS=NO
fi


if [ "$PASS" = YES ] ; then
	exit 0
else
	exit 1
fi
