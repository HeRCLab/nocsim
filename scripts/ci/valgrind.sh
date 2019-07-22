#!/bin/sh

set -u
set -e

echo '#### VALGRIND #################################################################'
make DEVELOP=true clean
make DEVELOP=true build

# we ignore possibly lost because lots of things internal to TCL show up as
# possibly lost
valgrind --suppressions=scripts/ci/tcl.supp --error-exitcode=1 --leak-check=full ./build/bin/nocsim < sample.txt
