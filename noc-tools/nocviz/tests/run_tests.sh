#!/bin/sh

cd "$(dirname "$0")"

if [ -z "$ENABLE_VALGRIND" ] ; then
	ENABLE_VALGRIND="YES"
fi

if [ -z "$ENABLE_GDB" ] ; then
	ENABLE_GDB="YES"
fi

set -u


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

strtrunc() {
	awk -v str="$1" -v len="$2" 'BEGIN {if (length(str) < len) { print(str) } else { \
		printf("%s...", substr(str, 1, len-3));}}'
}

show_on_err() {
	cmd="$1"
	logfile="$(mktemp)"
	status=0
	if ! $@ > "$logfile" 2>&1 ; then
		print_divider '-' 8 "FAILED: $cmd"
		echo "\$ $@"
		echo ""
		tail -n 50 "$logfile"
		status=1
	else
		print_divider '-' 8 "OK: $cmd"
	fi
	rm -f "$logfile"
	return $status

}

run_test() {
	test_file="$1"
	test_name="$(basename "$test_file" .test.c)"
	test_failed=0

	print_divider '#' 4 "SUITE: $test_name"

	rm -f "$test_name.bin"

	if ! show_on_err $CC $CFLAGS $test_file $LIBS -o $test_name.bin ; then
		test_failed=1
	fi

	if ! show_on_err ./$test_name.bin ; then
		test_failed=1
	fi

	if [ "$ENABLE_VALGRIND" = "YES" ] ; then
		if ! show_on_err valgrind --error-exitcode=1 --leak-check=full "./$test_name.bin" ; then
			test_failed=1
		fi
	fi

	rm -f "$test_name.bin"

	return $test_failed

}

print_divider '=' 25 "NOCVIZ TEST SUITE"
CFLAGS="$(make -s -C .. show_cflags)"
LIBS="$(make -s -C .. show_libs)"
CC="$(make -s -C .. show_cc)"
echo "CFLAGS=$CFLAGS"
echo "LIBS=$LIBS"

TEST_FAILURES=0
for f in *.test.c ; do
	if ! run_test "$f" ; then
		TEST_FAILURES=$(expr $TEST_FAILURES + 1)
	fi
done

echo "$TEST_FAILURES tests failed"
