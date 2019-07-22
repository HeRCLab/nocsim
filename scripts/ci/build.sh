#!/bin/sh

set -u
set -e

echo '#### BUILD ####################################################################'
make DEVELOP=true clean
make DEVELOP=true ECFLAGS=-Werror build
exit $?
