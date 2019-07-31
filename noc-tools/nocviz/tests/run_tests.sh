#!/bin/sh

cd "$(dirname "$0")"

# configuration -- allow overriding from environment
if [ -z "$ENABLE_VALGRIND" ] ; then
	ENABLE_VALGRIND="YES"
fi

if [ -z "$ENABLE_GDB" ] ; then
	ENABLE_GDB="YES"
fi

set -u


# Display a divider with a message inserted into it. This is just cosmetic, but
# helps a lot to differentiate different sections of test output. The divider
# is always 78 characters wide. If your message is generated dynamically, you
# may want to use `strtrunc` to guarantee it is short enough to fit.
#
# Parameters:
#
# $1 . . . character to print the divider with (such as #, = -, and so on);
#          multi-character dividers will produce incorrect formatting
# $2 . . . offset to display the message from the left side of the screen,
#          in characters
# $3 . . . message to display on top of the dividers
print_divider() {
	divchar="$1"
	divoffset="$2"
	divmsg="$3"
	divlen=78

	awk -v divchar="$divchar" -v divlen="$divlen" -v divoffset="$divoffset" -v divmsg="$divmsg" \
		'BEGIN {for (i = 0 ; i < divoffset  ; i++) {printf(divchar)}; \
		printf(" %s ", divmsg) ; for (i = divoffset +2 + length(divmsg) ; i < divlen; i++) { \
		printf(divchar); } ; printf("\n"); }'
}

# Truncate a string to be at most a given length. If the string exceeds the
# length, truncate it to that length and replace the last three characters
# with ...
#
# Parameters:
#
# $1 . . . string to be truncated
# $2 . . . maximum allowed length in characters
strtrunc() {
	awk -v str="$1" -v len="$2" 'BEGIN {if (length(str) < len) { print(str) } else { \
		printf("%s...", substr(str, 1, len-3));}}'
}

# Execute a command, displaying a divider indicating either success or failure.
# If the command fails, then the last 50 lines of output are displayed below
# the divider.
#
# Parameters:
#
# $@ . . . command to execute
show_on_err() {

	cmd="$1"
	logfile="$(mktemp)"
	status=0

	# want to print RC later, so we grab it here
	$@ > "$logfile" 2>&1
	rc=$?

	if [ $rc -ne 0 ] ; then
		print_divider '-' 8 "FAILED: $cmd"

		# show command that was run
		echo "\$ $@"
		echo ""

		# and it's exit code
		echo "exit code: $rc"
		echo ""


		tail -n 50 "$logfile"

		# exit with the given return code
		status=$rc
	else
		print_divider '-' 8 "OK: $cmd"
	fi

	rm -f "$logfile"

	return $status

}

# Execute a test file. It is assumed that this will be a .test.c file. It will
# be compiled with the CFLAGS and LIBS given by the nocviz Makefile, and
# executed. If it exits with a nonzero exit code, the test is considered
# to be failed.
#
# If valgrind is enabled, valgrind must run on the test file without error.
#
# If gdb is enabled, test segfaults will result in a traceback included in
# the output.
#
# Parameters:
#
# $1 . . . test file to compile and execute
run_test() {
	test_file="$1"
	test_name="$(basename "$test_file" .test.c)"
	test_failed=0
	test_segfaulted=0

	print_divider '#' 4 "SUITE: $test_name"

	rm -f "$test_name.bin"

	# try to compile the test
	if ! show_on_err $CC $CFLAGS $test_file ../*.o $LIBS -o $test_name.bin ; then

		# cant do any other part of the test with no binary
		test_failed=1
		rm -f "$test_name.bin"
		return $test_failed
	fi

	# try to execute the test
	if ! show_on_err ./$test_name.bin ; then
		test_failed=1
	fi

	# run GDB, if enabled
	if [ "$ENABLE_GDB" = "YES" ] ; then

		# but only if the test failed
		if [ $test_failed -ne 0 ] ; then

			gdb_log="$(mktemp)"

			# re-run under GDB and ask for a backtrace after
			printf 'r\nbt\nquit\ny\n' | gdb -q ./"$test_name.bin" > "$gdb_log" 2>&1

			# if GDB says there was no stack, then the program
			# didn't segfault
			if ! grep -q '(gdb) No stack.' < "$gdb_log" ; then

				# used to decide weather to run valgrind in
				# cases where the test binary exists nonzero
				test_segfaulted=1

				# display divider and traceback (otherwise
				# be silent)
				print_divider - 8 "GDB traceback"
				cat "$gdb_log"
				echo ""
				echo ""
			fi

			rm -f "$gdb_log"
		fi
	fi

	# Don't pollute output, since valgrind will always thrown an error if
	# the binary errors. Also, many test cases return early on error,
	# possibly preventing cleanup from happening.
	if [ $test_failed -ne 0 ] ; then

		# if the test segfaulted, we still want valgrind output
		if [ $test_segfaulted -eq 0 ] ; then
			rm -f "$test_name.bin"
			return $test_failed
		fi
	fi

	# If valgrind is enabled, make sure it dosen't find any memory leaks.
	# This is also handy for debugging segfaults.
	if [ "$ENABLE_VALGRIND" = "YES" ] ; then

		# the suppression's file is needed because of how TCL allocates
		# memory internally
		if ! show_on_err valgrind --suppressions=./tcl.supp --error-exitcode=1 --leak-check=full "./$test_name.bin" ; then
			test_failed=1
		fi
	fi

	rm -f "$test_name.bin"

	return $test_failed

}

# XXX: -DUSE_TCL_STUBS must not be used to build anything during testing, since
# it is intended only for extensions.

# print welcome message/header
print_divider '=' 25 "NOCVIZ TEST SUITE"
CFLAGS="$(make -s -C .. show_cflags | sed 's/-DUSE_TCL_STUBS//g')"
LIBS="$(make -s -C .. show_libs)"
CC="$(make -s -C .. show_cc)"
echo "CFLAGS=$CFLAGS"
echo "LIBS=$LIBS"
echo "CC=$CC"

# re-compile with stubs disabled
show_on_err make -C ../../ STUBS_CFLAGS="" clean
show_on_err make -C ../../ STUBS_CFLAGS=""

# count up any failed tests
TEST_FAILURES=0
for f in *.test.c ; do
	if ! run_test "$f" ; then
		TEST_FAILURES=$(expr $TEST_FAILURES + 1)
	fi
done

# clean, that way we don't run make later without stubs enabled
make -C ../../ clean > /dev/null 2>&1

echo "$TEST_FAILURES tests failed"

exit $TEST_FAILURES
