#!/bin/sh

set -u
set -e

# sample should not fail
echo '#### SAMPLE ###################################################################'
make DEVELOP=true clean
make DEVELOP=true build
./build/bin/nocsim < sample.txt 2>&1 > /dev/null
