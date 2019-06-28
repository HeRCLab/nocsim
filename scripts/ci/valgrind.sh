#!/bin/sh

set -u
set -e

echo '#### VALGRIND #################################################################'
make clean
make
valgrind --error-exitcode=1 --leak-check=full ./src/nocsim < sample.txt 2>&1 > /dev/null
