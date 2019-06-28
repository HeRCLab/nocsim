#!/bin/sh

set -u

echo '#### BUILD ####################################################################'
make clean
make ECFLAGS=-Werror
exit $?
