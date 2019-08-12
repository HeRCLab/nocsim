Test skeleton:

```tcl

# standard way to add noc tools TCL packages to package load path
source ../../scripts/noc_tools_load.tcl

# Allow referring to commands without prefixing the namespace (i.e. route,
# rather than nocsim::route).
namespace import ::nocsim::*

# tcltest is used as the unit testing framework
package require tcltest

# 001 is the test number
tcltest::test 001 {description of test} -body {

	return 0

} -result {0}

# This is required, because otherwise the nocsim_state* and nocviz_graph* and
# so on will not be de-allocated, as the namespace is never explicity deleted.
# If these lines are omitted, then valgrind will report spurios memory leaks.
namespace delete nocsim
namespace delete nocviz
```
