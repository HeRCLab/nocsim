#ifndef NOCVIZ_TEST_UTIL
#define NOCVIZ_TEST_UTIL

#define fail(fmt, ...) do { \
		fprintf(stderr, "TEST FAILED (%s, %s:L%i): ", __func__, __FILE__, __LINE__); \
		fprintf(stderr, fmt, __VA_ARGS__); \
		fprintf(stderr, "\n"); \
		exit(1); \
	} while(0);

#define str_should_equal(str1, str2) do { \
		if (strcmp((str1), (str2)) != 0) { fail("'%s' should equal '%s'", str1, str2); } \
	} while(0);

#define str_should_not_equal(str1, str2) do { \
		if (strcmp((str1), (str2)) == 0) { fail("'%s' should no equal '%s'", str1, str2); } \
	} while(0);

#endif
