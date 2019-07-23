#!/bin/sh

set -u
set -e

. scripts/util.lib

echo '#### COMPLEXITY ###############################################################'
tail_on_error make DEVELOP=true clean
tail_on_error lizard src
