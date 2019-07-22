#!/bin/sh

set -u
set -e

echo '#### COMPLEXITY ###############################################################'
make DEVELOP=true clean
lizard src
