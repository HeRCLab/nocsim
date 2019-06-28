#!/bin/sh

set -u
set -e

echo '#### BUILD ####################################################################'
make clean
make ECFLAGS=-Werror
exit $?
