#!/bin/sh

set -u

PASS=YES
cd "$(dirname "$0")/.."

for f in scripts/ci/*.sh ; do
	if ! sh "$f" ; then
		PASS=NO
	fi
done

if [ "$PASS" = YES ] ; then
	echo "ALL TESTS PASSED"
	exit 0
else
	echo "TEST FAILURES DETECTED"
	exit 1
fi
