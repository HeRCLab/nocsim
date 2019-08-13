# noc-tools User Manual

noc-tools is implemented as a collection of TCL modules, implemented in both
pure TCL and as C extensions for performance critical code. At present, there
are two major modules:

* [nocsim](./nocsim.md) -- a simulation engine for simulating NoCs
* [nocviz](./nocviz.md) -- a general-purpose visualization tool oriented
  towards NoCs

A script is provided to automatically import (via `package require`) all
noc-tools packages, which is located in the `scripts/` directory. You should
access it using the `source` command in a TCL interpreter, for example: `source
/path/to/noc-tools/scripts/noc_tools_load.tcl`.

For those seeking an interactive TCL scripting environment,
[eltclsh](http://homepages.laas.fr/mallet/soft/shell/eltclsh) is recommended.

## Debugging Methodology

TCL has had a variety of debuggers and debugging approaches over it's long
life. Any debugging approach that works within an embedded TCL instance should
work correctly with `nocsim`. However, for your convenience, an approach to TCL
script debugging is discussed and documented here.

TclPro was once a popular distribution of TCL and associated tools which
retailed for a hefty price tag. It was eventually open sourced, but waned in
popularity and languished in obscurity for some time. Eventually, the TclPro
debugger was forked by [FlightAware,
LLC](https://github.com/flightaware/TclProDebug) and updated for TCL 8.5 and
higher. This updated version of TclProDebug is recommended for use with
`nocsim`, and compatibility is specifically tested as it is used as part of the
`nocsim` development process.

For the purposes of this document, it is assumed that TclProDebug is installed
in `/opt/TclProDebug`. You may need to adjust some steps according to it's
actual install location on your system.

First, you should start a debugging session by running
`/opt/TclProDebug/bin/prodebug`, selecting *File->New Project*. Make sure to
select "Remote Debugging" in the dialog window, then click "Apply".

Next up, you will want to wrap your TCL code you would like to debug in a
`debugger_eval` block. Consider this example file:

```tcl
source /opt/TclProDebug/lib/tcldebugger/initdebug.tcl
debugger_init
debugger_eval {
	debugger_break
	set x 5
	set y 10
	set z 11
	puts "value of z is $z"
}
debugger_break
```

If you save this file and `source` it from within a `nocsim` interpreter, it
should automatically connect to your running instance of TclProDebug, and you
should see the code appear in your TclProDebug window. It should look something
like this:

![Example of a running TclProDebug session](./../assets/TclProDebug.png)

You can now inspect variables via the interactive inspection tool, and single
step your code or allow it to run until it hits a breakpoint.

**TIP** you can leave your debugging code in even after development has
completed -- if TclProDebug is not running on your system, `debugger_break`
does nothing, and `debugger_eval` acts like `eval`.

For more information, you may wish to consult [the TclPro User's
Guide](https://www.tcl.tk/software/tclpro/doc/TclProUsersGuide12.pdf).
