#!/bin/sh

set -u
set -e

echo '#### VALGRIND #################################################################'
make clean
make
# we ignore possibly lost because lots of things internal to TCL show up as
# possibly lost
valgrind --show-possibly-lost=no --error-exitcode=1 --leak-check=full ./src/nocsim < sample.txt 2>&1 > /dev/null
