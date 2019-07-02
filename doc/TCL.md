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
running, including during callback execution -- modifying the value of these
variables during such times will result in undefined behavior.

**NOTE** for technical reasons, the read-only or read+write status may or may
not actually be enforced by the TCL interpreter. Even if they are not, you
should respect them or risk causing undefined behavior.

| variable name | r/w | description |
|-|-|-|
| `RNG_seed` | r | value used to seed the random number generator |
| `num_PE` | r | number of instantiated PEs |
| `num_router` | r | number of instantiated routers |
| `num_node` | r | number of instantiated nodes |
| `flit_no` | r | number of injected flits so far + 1 |
| `tick` | r | current tick number, starting at 0 |
| `title` | r+w | simulation title, defaults to "unspecified" |
| `dir_N` | r | internal enum value of the `N` direction |
| `dir_S` | r | internal enum value of the `S` direction |
| `dir_E` | r | internal enum value of the `E` direction |
| `dir_W` | r | internal enum value of the `W` direction |
| `dir_PE` | r | internal enum value of the `PE` direction |
| `type_router` | r | internal enum value of the `router` node type |
| `type_PE` | r | internal enum value of the `PE` node type |

## Procedures

### `router ID ROW COL BEHAVIOR`

Creates a new router instance with the given row and column values. `ID` may be
any non-empty identifier string, but should be unique (a stringified version of
`$num_node + 1` may be a good option). `BEHAVIOR` should be the name of a
behavior callback which will be called once per tick to define how this node
behaves.

### `PE ID ROW COL BEHAVIOR`

Exactly as with `router`, except that the instantiated node will be a PE. Note
that the behavior callbacks for PEs and routers may not be interchangeable,
depending on how they are implemented.

### `link FROM_ID TO_ID`

Creates a new link, connecting the specified node IDs.

### `current`

Returns the node ID for which the behavior callback is currently executing.

### `step` / `step N`

Advances the simulation by `N` ticks, or by 1 tick if `N` is not provided.

### `nodeinfo ID ATTR`

Retrieve Information about the node `ID`. The following attributes are
available:

| `ATTR` | Type | Description |
|-|-|-|
| `type` | int | either `type_router` or `type_PE` indicating the node type |
| `row` | int | row number of the node |
| `col` | int | column number of the node |
| `behavior` | string | behavior callback for this node |

### `findnode` / `findnode ROW COL` / `findnode ROWL ROWU COLL COLU`

Depending on the number of parameters provided:

* return a list of all node IDs
* return a list of node IDs that have the row and column `ROW`, `COL`,
* return a list of node IDs which have a row number bounded by `ROWL` and
  `ROWU` (inclusive) and a column number bounded by `COLL` and `COLU`
  (inclusive).

### `behavior ID BEHAVIOR`

Modify the assigned behavior for the node with the specified ID.

### `randnode` / `randnode ROW COL` / `randnode ID`

Depending on the number of parameters provided:

* return a random node ID
* return a random node ID which does not have the row and column values `ROW`,
  `COL`
* return a random node ID which is not `ID`.

In each case, `randnode` will only ever return nodes of type PE.

### `route FROM TO` (routing behaviors only)

Routes the flit incoming from the direction `FROM` to the direction `TO`. `TO`
and `FROM` should both be integers corresponding to directions, the magic
variables `dir_*` are provided to make this process easier.

### `peek DIR ATTR` (routing behaviors only)

Retrieves the attribute `ATTR` from the incoming flit in the direction `DIR`.

The following attributes are available:

| `ATTR` | Type | Description |
|-|-|-|
| `from` | string | `id` of originating node |
| `to` | string | `id` of destination node |
| `from_row` | int | row of originating node |
| `from_col` | int | column of originating node |
| `to_row` | int | row of destination node |
| `to_col` | int | column of destination node |
| `spawned_at` | int | tick number at which the flit instantiated |
| `injected_at` | int | tick number at which the flit was injected |
| `age` | int | number of ticks since the flit was injected |

### `inject TO` (PE behaviors only)

Inject a new flit destined for the node ID `TO`,

## Behavior Callbacks

The behavior of each node in simulate network is defined by a *behavior
callback* (sometimes shortened to simply *behavior*). This is a TCL function
which implements the abhor that is desired for the particular node.

There are two types of behaviors -- PE behaviors and router behaviors. While in
principle, it would be possible to write a single behavior that detects the
type of node it is operating on and acts accordingly, it is suggested to define
separate behaviors for each. Attempting to inject flits from a routing
behavior, or perform routing operations from a PE behavior will result in
undefined behavior.

A behavior is implemented as a TCL procedure, which may be defined either in
your simulation script, or included from a separate TCL library. The procedure
should take no arguments; all necessary information about the simulation state
is provided via magic variables and procedures.

**TIP**: the magic variables provided by nocsim are defined in the top-level
namespace of the interpreter instance. This means they are not directly
accessibly from within procedure calls, including behavior callbacks. It is
suggested that they might be accessed using the
[`upvar`](https://wiki.tcl-lang.org/page/upvar) command. For example, to alias
`current` from the top-level namespace into a behavior callback, issue `upvar 1
current current`.o

### Example Behavior Callback

```tcl
proc simpletest {} {
        upvar 1 current current
        puts "===== simpletest ===="
        puts "current is: $current"
        puts "type is: [nodeinfo $current type]"
        puts "row is: [nodeinfo $current row]"
        puts "col is: [nodeinfo $current col]"
}
```
