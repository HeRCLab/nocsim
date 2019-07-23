#!/bin/sh

set -u
set -e

. scripts/util.lib

# sample should not fail
echo '#### SAMPLE ###################################################################'
tail_on_error make DEVELOP=true clean
tail_on_error make DEVELOP=true build
tail_on_error ./build/bin/nocsim < examples/mesh_DOR.tcl
