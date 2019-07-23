#!/bin/sh

set -u
set -e

# sample should not fail
echo '#### SAMPLE ###################################################################'
make DEVELOP=true clean
make DEVELOP=true build
./build/bin/nocsim < examples/mesh_DOR.tcl > /dev/null 2>&1 
