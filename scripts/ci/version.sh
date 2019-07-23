#!/bin/sh

# Tests that the nocsim version reported by the binary matches what we expect.
#
# This also implicitly tests that the TCL library is correctly imported.

set -u
set -e

. scripts/util.lib

echo '#### VERSION ##################################################################'
tail_on_error make DEVELOP=true clean
tail_on_error make DEVELOP=true build
printf "PrintNocsimInfo\n" | ./build/bin/nocsim | grep "nocsim version: $(make version)"
