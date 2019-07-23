
#!/bin/sh

set -u
set -e

. scripts/util.lib

# unit tests
echo '#### UNIT TESTS ###############################################################'
tail_on_error make DEVELOP=true clean
tail_on_error make DEVELOP=true build

FAILED=0

onfail () {
	FAILED=$(expr $FAILED + 1)
	echo "------ $(basename "$f"): FAIL"
	cat "$temp"
}

for f in scripts/ci/unittest/*.tcl ; do
	temp="$(mktemp)"
	if ! ./build/bin/nocsim < "$f" > "$temp" 2>&1; then
		onfail
		echo '-------- backtrace ---------------------------------------------------------------'
		printf 'r < '"$f"'\nbt\nquit\ny\n' | gdb ./build/bin/nocsim
	elif grep "FAILED" < "$temp" > /dev/null 2>&1 ; then
		onfail
	else
		echo "------ $(basename "$f"): PASS"
	fi
	rm -f "$temp"
done

echo "====== $FAILED tests failed"

exit $FAILED
