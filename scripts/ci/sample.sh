#!/bin/sh

set -u
set -e

# sample should not fail
echo '#### SAMPLE ###################################################################'
make clean
make
./src/nocsim < sample.txt 2>&1 > /dev/null
