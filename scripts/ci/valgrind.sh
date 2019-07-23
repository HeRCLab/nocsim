#!/bin/sh

set -u
set -e

. scripts/util.lib

echo '#### VALGRIND #################################################################'
tail_on_error make DEVELOP=true clean
tail_on_error make DEVELOP=true build

# we ignore possibly lost because lots of things internal to TCL show up as
# possibly lost
tail_on_error valgrind --suppressions=scripts/ci/tcl.supp --error-exitcode=1 --leak-check=full ./build/bin/nocsim < examples/mesh_DOR.tcl
