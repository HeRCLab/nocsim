#ifndef NOCVIZ_TEST_UTIL
#define NOCVIZ_TEST_UTIL

#define fail(fmt, ...) do { \
		fprintf(stderr, "TEST FAILED (%s, %s:L%i): ", __func__, __FILE__, __LINE__); \
		fprintf(stderr, fmt, __VA_ARGS__); \
		fprintf(stderr, "\n"); \
		exit(1); \
	} while(0);

#define str_should_equal(str1, str2) do { \
		if (strcmp((str1), (str2)) != 0) { fail("'%s' ('%s') should equal '%s' ('%s')", \
			str1, #str1, str2, #str2); } \
	} while(0)

#define str_should_not_equal(str1, str2) do { \
		if (strcmp((str1), (str2)) == 0) { fail("'%s' ('%s') should no equal '%s' ('%s')", \
			str1, #str1, str2, #str2); } \
	} while(0)

#define should_equal(v1, v2) do { \
		if ( (v1) != (v2) ) { fail("'%s' should equal '%s'", #v1, #v2); } \
	} while(0)

#define should_not_equal(v1, v2) do { \
		if ( (v1) == (v2) ) { fail("'%s' should not equal '%s'", #v1, #v2); } \
	} while(0)

#define should_be_true(expr) do { \
		if (!(expr)) { fail("'%s' should have been true and was not", #expr); } \
	} while(0)

#define should_be_false(expr) do { \
		if (!(expr)) { fail("'%s' should have been false and was not", #expr); } \
	} while(0)

#define should_be_null(expr) do { \
		if ((expr) != NULL) { fail("'%s' should have been NULL and was not", #expr); } \
	} while(0)

#define should_not_be_null(expr) do { \
		if ((expr) == NULL) { fail("'%s' should not have been NULL and was not", #expr); } \
	} while(0)

#define tcl_should_eval(interp, fmt, ...) do { \
		if (Tcl_Evalf(interp, (fmt), __VA_ARGS__) != TCL_OK) { \
			fprintf(stderr, "TCL Result: %s\n", Tcl_GetStringResult(interp)); \
			fprintf(stderr, "errorInfo: %s\n", Tcl_GetVar(interp, "errorInfo", 0)); \
			char* __cmd; \
			if (asprintf(&__cmd, fmt, __VA_ARGS__) < 0) { \
				warn("asprintf error!"); \
			} \
			fail("TCL command '%s' should have executed without error and did not", __cmd); \
		} \
	} while(0)

#define tcl_should_not_eval(interp, fmt, ...) do { \
		if (Tcl_Evalf(interp, (fmt), __VA_ARGS__) != TCL_ERROR) { \
			fprintf(stderr, "TCL Result: %s\n", Tcl_GetStringResult(interp)); \
			fprintf(stderr, "errorInfo: %s\n", Tcl_GetVar(interp, "errorInfo", 0)); \
			char* __cmd; \
			if (asprintf(&__cmd, fmt, __VA_ARGS__) < 0) { \
				warn("asprintf error!"); \
			} \
			fail("TCL command '%s' should have thrown an error and did not", __cmd); \
		} \
	} while(0)

#endif
