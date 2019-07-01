# `nocsim` TCL Scripting Manual

`nocsim` implements a simple DSL (domain-specific language) for performing NoC
simulations, based around TCL. Your system's standard TCL interpreter is used
as a base, and special *magic variables* linked to aspects of the internal
simulation state are exposed for use from TCL script files. Several functions
are exposed also via TCL to make it easy to manage the simulation state, and
collect statistical information about the simulation's results.

## Programming Model

A single invocation of `nocsim`, the corresponding script file, and the
parameters given to it constitute a single *simulation run*. Each simulation run
is independent from each other simulation run, and is fully synchronous,
single-threaded, and independent from every other simulation run. This approach
was selected because in most uses for `nocsim`, dozens or hundreds of different
parameters need to be evaluated for performance, so it is simpler for each
simulation to be single threaded and simply run many simulations in parallel
with a tool such as GNU parallel.

A useful strategy can be to generate one line of tab-separated values from each
simulation run such that all lines from all simulation runs may be concatenated
together at a later time, and thereby produce a valid TSV file that can be used
in conjunction with any standard statistics or plotting tools.

When `nocsim` is compiled without debugging enabled, the only output on
standard out originates from the TCL script file, and several convenient hooks
are provided for the programmer to create callbacks that enable performance
counters to be implemented. Some performance counters are implemented in the
underlying simulation engine and exposed as magic variables.

It is suggested that the programmer should implement any statistical analysis
directly in TCL, by calling utility programs from TCL, or by using the TCL
script file to generate output that is convenient to parse via an external
tool.

Implementation details such as these are beyond the scope of this document,
however the programmer should be advised that TCL includes a robust statistics
library [(documented
here)](https://tools.ietf.org/doc/tcllib/html/statistics.html) in the form of
`math::statistics` which may be of value both for data generation, and for
analysis.

## TCL Environment

`nocsim` requires at least TCL version 8.5, so any script files may assume at
least this version. `nocsim` will correctly populate `$argv0`, `$argc`, and
`$argv` for the script file's use, and further `$tcl_library` is extracted from
the system copy of `tclsh` (or the first occurrence of such in `$PATH`). The
system `init.tcl` file is sourced from the detect `$tcl_library`.

This means that any TCL libraries installed at a system level should be
available for use from within `nocsim` script files.

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

**TODO**

## Magic Variables

Note that additional magic variables are available during callback execution
and are documented in the *Callbacks* section later in this document. The magic
variables described here should be treated as read-only while the simulation is
running, including during callback execution.

| variable name | r/w | description |


## Methods

**TODO**

## Callbacks

**TODO**
