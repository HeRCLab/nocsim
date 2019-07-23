#!/bin/sh

set -u
set -e

. scripts/util.lib

echo '#### BUILD ####################################################################'
tail_on_error make DEVELOP=true clean
tail_on_error make DEVELOP=true ECFLAGS=-Werror build
exit $?
