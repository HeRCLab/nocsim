#!/bin/sh

# Tests that the nocsim version reported by the binary matches what we expect.
#
# This also implicitly tests that the TCL library is correctly imported.

set -u
set -e

echo '#### VERSION ##################################################################'
make DEVELOP=true clean
make DEVELOP=true build
printf "NocsimTCL::PrintNocsimInfo\n" | ./build/bin/nocsim | grep "nocsim version: $(make version)"
