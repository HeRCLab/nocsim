
#!/bin/sh

set -u
set -e

# unit tests
echo '#### UNIT TESTS ###############################################################'
make clean
make

FAILED=0
for f in scripts/ci/unittest/*.tcl ; do
	temp="$(mktemp)"
	if ! ./src/nocsim < "$f" > "$temp" 2>&1; then
		FAILED=$(expr $FAILED + 1)
		echo "$f: FAIL"
		echo '----- failing test output -----------------------------------------------------'
		cat "$temp"
	elif grep "FAILED" < "$temp" > /dev/null 2>&1 ; then
		FAILED=$(expr $FAILED + 1)
		echo "$f: FAIL"
		echo '----- failing test output -----------------------------------------------------'
		cat "$temp"
	else 
		echo "$f: PASS"
	fi
	rm -f "$temp"
done

echo "$FAILED tests failed"

exit $FAILED
