#!/bin/sh

set -u
set -e

echo '#### COMPLEXITY ###############################################################'
make clean
lizard src
