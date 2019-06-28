#!/bin/sh

set -u

PASS=YES
cd "$(dirname "$0")/.."

echo '#### BUILD ####################################################################'
make clean
if ! make ECFLAGS=-Werror ; then
	PASS=NO
fi

if [ "$PASS" = YES ] ; then
	exit 0
else
	exit 1
fi
