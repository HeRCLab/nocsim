# nocsim TCL Package Documentation

`nocsim` implements a simple DSL (domain-specific language) for performing NoC
simulations, based around TCL. Your system's standard TCL interpreter is used
as a base, and special *magic variables* linked to aspects of the internal
simulation state are exposed for use from TCL script files. Several functions
are exposed also via TCL to make it easy to manage the simulation state, and
collect statistical information about the simulation's results.

## Magic Variables

When `nocsim` was a standalone program which implemented a TCL scripting
interface, rather than a TCL module, information was presented using "magic
variables" injected into the TCL interpreter. To preserve compatibility, this
is still done, although all magic variables are now placed within the `nocsim`
namespace.

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
| `nocsim_RNG_seed` | r | value used to seed the random number generator |
| `nocsim_num_PE` | r | number of instantiated PEs |
| `nocsim_num_router` | r | number of instantiated routers |
| `nocsim_num_node` | r | number of instantiated nodes |
| `nocsim_flit_no` | r | number of injected flits so far + 1 |
| `nocsim_tick` | r | current tick number, starting at 0 |
| `nocsim_title` | r+w | simulation title, defaults to "unspecified" |
| `nocsim_version` | r | list containing `nocsim` major, minor, and patch numbers in that order |
| `nocsim_injected` | r | total number of flits injected so far |
| `nocsim_spawned` | r | total number of flits spawned so far |
| `nocsim_dequeued` | r | total number of flits dequeued so far |
| `nocsim_backrouted` | r | total number of flits backrouted so far |
| `nocsim_routed` | r | total number of flits routed so far |
| `nocsim_arrived` | r | total number of flits which have arrived so far |

## Simulation Procedures

All simulation procedures are made available under the `nocsim` namespace.

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

### `link FROM_ID TO_ID` / `link FROM_ID TO_ID TO_DIR` / `link FROM_ID TO_ID FROM_DIR TO_DIR`

Creates a new link, connecting the specified node IDs.

If only the node IDs are provided, then the link direction is inferred
automatically. Note that when inferring link direction, any link from a PE to a
router is assumed to have the direction `PE`, regardless of relative row/col
values.

If `TO_DIR` is given, then the command proceeds as if `FROM_DIR` was given as
the opposite of `TO_DIR` (note that the opposite of `PE` is defined as `PE` in
this case).

If `TO_DIR` and `FROM_DIR` are given, then the outgoing link from node
`FROM_ID` always has the direction `FROM_DIR` and the incoming link to `TO_ID`
always has the direction `TO_DIR`.

Consider this example network:

```plaintext
+-----------------------------------------------------+
|                                                     |
|                                                     |
|                                                     |
|                                                     |
|                                                     v
|       +----------------+                   +--------+------+
|       |       N        |                   |       N       |
|       |                |                   |               |
|       |                |                   |               |
+-------+   router1      |                   |     router2   |
        |W  (0,0)      E |                   | W   (0,1)   E |
        |                |                   |               |
        |                |                   |               |
        |         S      |                   |       S       |
        +----------------+                   +---------------+
```

This configuration could be obtained by issuing `link router1 router2 [dir2int
W] [dir2int N]`.

In general, it is recommended to allow nocsim to infer the directions of nodes
relative to one another. However the ability to override the default link
direction is provided to generating particularly novel network configurations.
It is assumed that if you are using this capability, you know what you are
doing.

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
| `routed` | int | number of flits routed thus far |
| `spawned` | int | total number of flits spawned thus far |
| `injected` | int | total number of flits injected thus far |
| `dequeud` | int | total number of flits dequeued thus far |
| `backrouted` | int | total number of flits backrouted by this node (if node is a router), or which originated by this node and were backrouted (if node is a PE) |
| `arrived` | int | total number of flits that have arrived at this node so far (i.e. number flits whose destination was this node and who were routed into this node) |

### `linkinfo FROM TO ATTR`

Retrieve information about the link connecting the nodes with ID `FROM` and
`TO`.

`ATTR` specifies the information to retrieve.

| `ATTR` | Type | Description |
|-|-|-|
| `in_flight` | list of int | list of flit numbers currently in the link |
| `current_load` | int | number of flits currently in the link |
| `load` | int  | number of flits routed through this link so far |
| `from_dir` | int | outgoing direction of link from it's source node |
| `to_dir` | int | incoming direction of link to it's destination node |

**NOTE** `current_load` should be used with care, as it may yield inaccurate
results if accessed during a behavior callback.

**NOTE** directions are given in nocsim's internal integer representation.

**TIP** remember that links are strictly directional, i.e. the link `foo bar`
is not the same as the link `bar baz`.

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

### `registerinstrument INSTRUMENT PROCEDURE`

Register the TCL procedure `PROCEDURE` to be called by the specified
instrument. Each instrument may only have one registered procedure at a time.
See the *Instrumentation* section below for more information.

### `conswrite STR`

Write a specified string standard output.

As of noc-tools 2.0.0, `conswrite` is deprecated and should not be used for new
code. In previous versions, this was used to interface with the GUI text
console, which no longer exists.

### `errwrite STR`

Write a specified string to standard error.

As of noc-tools 2.0.0, `errwrite` is deprecated and should not be used for new
code. In previous versions, this was used to interface with the GUI text
console, which no longer exists.

### `route FROM TO` (routing behaviors only)

Routes the flit incoming from the direction `FROM` to the direction `TO`. `TO`
and `FROM` should both be integers corresponding to directions, the magic
variables `dir_*` are provided to make this process easier.

### `route_priority FROM ...` (routing behaviors only)

Routes the flit incoming from the direction `FROM` into the highest priority
available outgoing link. Remaining arguments after `FROM` are a list of
directions (in nocsim's internal integer representation), in depending order of
preference. `route_from` will automatically select the highest priority link
which exists and is available.

The direction of the select link will be returned if one is found, and
otherwise no value (`""`) will be returned.

For example, `route_priority [dir2int east] [dir2int north] [dir2int south]`
would route the flit incoming from the east to the north if it is available,
and then to the south if the north is unavailable.

Note that the existence/availability of all such links is checked without
`route_priority`, and failures are silent (aside from the return value).

### `drop FROM` (routing behaviors only)

Drops the  flit incoming from the direction `FROM`. This will immediately
delete the flit from the simulation. This will trigger neither the `arrive` nor
the `route` instruments with respect to the specified flit. Performance
counters for the relevant router and it's links are not updated.

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

### `avail DIR` (routing behaviors only)

Considers the outgoing link `DIR` for the current node (where `DIR` is one of
the `dir_*` magic variables). Returns one of the following values:

| return value | meaning |
|-|-|
| 0 | link available for use |
| 1 | link already used this tick |
| 2 | no such link |

### `incoming DIR` (routing behaviors only)

`DIR` should be specified as an integer in `nocsim`'s internal format.

Returns 1 if there is a flit incoming from the specified direction, and 0
otherwise (including if the link in that direction does not exist). Note that
routing a flit removes it from the relevant incoming link;

### `allincoming` (routing behaviors only)

Returns a list of all directions (in `nocsim`'s internal integer format) from
which there is an incoming flit awaiting processing. Using `route` to route the
flit elsewhere will cause it to stop appearing in this list.

### `spawn TO` (PE behaviors only)

Spawn a new flit destined for the node ID `TO`, The originating node is always
the current node, which may be tested via the `current` procedure.

### `inject TO` (PE behaviors only)

Alias for `spawn`.

## Utility Procedures

### `PrintNocsimInfo`

Display the `nocsim` and TCL versions.

### `dir2int DIR`

Converts a string direction (`DIR`) to an internal integer representation used
by `nocsim`. Valid direction strings include:

* `n`
* `north`
* `up`
* `s`
* `south`
* `down`
* `e`
* `east`
* `right`
* `w`
* `west`
* `left`
* `p`
* `pe`

**NOTE** direction strings are case insensitive.

**BEST PRACTICE** it is suggested to always use the abbreviated style
(`n`/`s`/`e`/`w`/`pe`). Other styles are made available for user friendliness in
interactive interpreters.

### `dir2list ...`

Converts a list of direction strings to a list of integers using `dir2int`.

This is especially useful for commands such as `route_priority`.

**TIP**: [the TCL list expansion
operator](http://www.wellho.net/mouth/3583_Expanding-a-list-of-parameters-in-Tcl-and-eval.html)
may be of use to you.

### `int2dir DIR`

Converts an integer direction (`DIR`) in `nocsim`'s internal representation to
to a string representation. The generated string representation will always be
in the abbreviated `n`/`s`/`e`/`w`/`pe` style.

### `type2int TYPE`

Converts a string node type to `nocsim`'s internal integer representation.
Valid node types are:

* `router`
* `pe`

**NOTE** node type strings are case insensitive

### `int2type TYPE`

Converts an integer type in `nocsim`'s internal representation to a string.

### `all_adjacent ID`

Returns a list of all node IDs which are adjacent (i.e. north, south, east, and
west) to the node with the specified ID. Note that links need not be present
for adjacent nodes to be detected, this makes this function suitable for use
during link generation.

### `lshift LISTVAR` / `lshift LISTVAR COUNT`

The `lshfit` method described [here](https://wiki.tcl-lang.org/page/lshift)
(specifically fredderic's variation).  This procedure pops the first element
from a list and returns it.

If `COUNT` is specified, then that many list elements are popped and returned
as a list.

**NOTE**: `LISTVAR` should be the name of the list to be operated on (i.e.
`listname`, not `$listname`).

### `lremove LISTVAR VAL`

Remove an element from a list by value, rather than by index.  If multiple
instances of the same value are present, only the first will be removed. The
modified list is returned, rather than updated in-place.

For example"

```tcl
set mylist {a b c d}
set mylist [lremove mylist b]
puts $mylist
```

Would output: `a c d`.

### `allnodes`

Return a list of all currently instantiated node IDs.

## Topography Generation Procedures

### `create_mesh WIDTH HEIGHT INJECT_BEHAVIOR ROUTE_BEHAVIOR`

Creates an undirected mesh topography with `WIDTH` many columns and `HEIGHT`
many rows. All instantiated PEs will have the specified injection behavior, and
all instantiated routers will have the specified `ROUTE_BEHAVIOR`. Nodes will
have automatically generated IDs.

Returns a list of node IDs that were generated.

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

**NOTE** routing callbacks are **required** to handle all incoming flits each
time they are called. Flits must either be routed, or dropped using the `drop`
procedure. Failure to do so will cause the `nocsim` to exit with an error.

**TIP** the magic variables provided by nocsim are defined in the top-level
namespace of the interpreter instance. This means they are not directly
accessibly from within procedure calls, including behavior callbacks. It is
suggested that they might be accessed using the
[`upvar`](https://wiki.tcl-lang.org/page/upvar) command. For example, to alias
`tick` from the top-level namespace into a behavior callback, issue `upvar 1
tick tick`.

**WARNING** although behavior callbacks are executed in serial, care is taken
with respect to the design of the simulation engine to ensure that the
simulation results are as if every behavior for every node on a given tick
happened "at once". It is ostensibly possible to construct behavior callbacks
which have dependencies on execution order or intermediary simulation state
(such as via `linkinfo`). Such designs are explicitly not supported, and the
execution order of behavior callbacks is an internal implementation detail of
`nocsim` and should not be relied upon.

**TIP** it is very important to ensure that all incoming flits are handled when
defining a routing behavior, otherwise those flits may be lost while the engine
flips the next state into the current state at the end of the tick.  If you are
concerned that you might be missing some incoming flits, try using the `inject`
and `arrive` instruments to track which flits are still in-flight (have not
arrived yet).

### Example Behavior Callback

```tcl
proc simpletest {} {
	upvar 1 tick tick
	set current [current]
	puts "===== simpletest ===="
	puts "current is: $current"
	puts "type is: [nodeinfo $current type]"
	puts "row is: [nodeinfo $current row]"
	puts "col is: [nodeinfo $current col]"
	puts "tick is: $tick"
}
```

## Performance Counters

For convenience and performance, many useful statistics are collected and
exposed via performance counters. These are maintained internally by the
simulation engine. Consult the documentation for the `nodeinfo` and `linkinfo`
procedures, as well as for magic variables.

## Instrumentation

More detailed performance information can be obtained via instrumentation. An
*instrument* is a TCL procedure which is automatically called at a specific
point in the simulation engine's execution. Additional information is passed
into instruments via parameters.

**TIP** some instruments are called within the simulation engine's hot loop,
and can significantly degrade performance. Prefer the use of performance
counters wherever possible.

**TIP** need to store data collected by an instrument? Try using
`upvar` to attach it to the interpreter's top-level scope.

**WARNING** Keep in mind that most instruments are called while the TCL
interpreter is within the scope of a behavior, so you should always use `upvar
#0 ...` to attach to the global interpreter scope directly, rather than the
scope of the currently executing behavior. The specific arrangement of TCL
scopes within `nocsim` is an implementation detail -- you should always use
`upvar #0` rather than `upvar 1`, `upvar 2`, and so on unless you really know
what you're doing.

**BEST PRACTICE** don't make assumptions about the number of parameters that
will be passed to an instrument -- more information might be added in future
releases of `nocsim`.

Instruments are *registered* so that the simulation engine is aware of them via
the `registerinstrument` procedure.

The following instruments are available:

### `inject`

Executes any time a flit is injected.

Parameters:

* origin node ID
* destination node ID
* flit number

**NOTE:** it is only possible to know if a flit is injected or backrouted at
least one tick after it is spawned (i.e. when it finishes traveling from it's
origin PE to it's first router). Keep this in mind when comparing tick numbers
in the `inject` instrument, as opposed to `spawn`, or `dequeue`.

### `spawn`

Executes any time a flit is spawned.

Parameters:

* origin node ID
* destination node ID
* flit number

### `dequeue`

Executes any time a flit is dequeued from a PE, regardless of weather or not it
is later backrouted.

Parameters:

* origin node ID
* destination node ID
* flit number

### `route`

Executes any time a flit is routed.

Parameters:

* origin node ID
* destination node ID
* flit number
* tick number on which the flit was spawned
* tick number on which the flit was injected
* number of hops so far
* node ID of node from which the flit was routed
* node ID of the node to which the flit was routed

### `arrive`

Executes any time a flit arrives at it's destination.

Parameters:

* origin node ID
* destination node ID
* flit number
* number of hops so far
* tick number on which the flit was spawned
* tick number on which the flit was injected

### `backroute`

Executes any time a flit is backrouted.

Parameters:

* origin node ID
* destination node ID
* flit number
* number of hops so far
* tick number on which the flit was spawned
* tick number on which the flit was injected

### `tick`

Executes at the beginning of each new tick. No special parameters are passed to
this instrument. This instrument is mostly useful for performing cleanup or
initialization between ticks.

## Terminology

* **origin node** -- in the context of a flit, the node at which was flit was
  spawned.
* **destination node**  -- in the context of a flit, the node to which a flit
  is being transmitted.
* **spawn** -- a flit is *spawned* when it's origin node creates it via the
  `inject` command.
* **inject** -- a flit is *injected* when it is transmitted from it's origin
  node (a PE) to a router without being backrouted.
* **dequeue** -- a flit is *dequeued* when it is transmitted from it's origin
  node (a PE) to a router, regardless of weather it is backrouted or not.
* **backroute** -- `nocsim` simulates backpressure by counting any event where
  a flit is transmitted back to it's origin node (a PE) from an immediately
  adjacent router (i.e. if the router lacks the buffer or outgoing link
  capacity to handle the flit this tick). When backrouting occurs, the
  backrouted flit is installed at the head, rather than the tail of the
  outgoing flit queue from it's origin node (i.e. it continues to be "first in
  line"). Backrouting still counts as a "hop". The total number of times
  backrouting has occurred is the total number of dequeue events less the total
  number of inject events.
* **hop** -- any tick where a specific flit travels between two adjacent nodes.
* **instrument** -- a TCL procedure called in response to a specific event
  occurring within the simulation.
* **behavior** -- a TCL procedure used to define how a particular node in the
  network under simulation should act.

## Advanced Topics

### Reporting Strategies

Information is intended to be collected via the "instrumentation" feature of
`nocsim`. Instruments might either output information of interest via
`conswrite` for later analysis by an external program, or might use `upvar` to
save data into a global variable for later analysis after the simulation is
concluded (i.e. after the last call to `step` has returned).

As long as the magic variables and procedures that `nocsim` exposes are not
collided with, the programmer has complete free reign to use any TCL programmer
patterns or strategies that are desired, including defining procedures, import
libraries, setting variables, or performing file I/O operations.

### Simulation Parameterization

It is often the case that a given simulation might need to be run with several
different slight variations; for example, different injection rates or link la
tenses. To that end, `argv` and `argc` are passed in from the top-level
`nocsim` program, sans any flags or other options used to set the behavior of
nocsim itself. The suggested approach to simulation parametrization is to
simply provide any needed parameters as positional arguments to `nocsim`, and
then inspecting the `$argc` and `$argv` variables.

**TIP** The GUI version of `nocsim` does not pass in it's `argc` and `argv`,
however you may simply set these variables before `source`-ing the code you
intend to run.

### Data Formats

TCL supports a variety of common file formats which may be of use for
inter-operating `nocsim` simulations with other tools.

* XML
	* [tDOM](https://core.tcl-lang.org/tdom/timeline)
		* Appears to be the most active / up-to-date XML implementation
	* [Tcler's Wiki XML page](https://wiki.tcl-lang.org/page/XML)
	* [TclXML Library](http://tclxml.sourceforge.net/tclxml/3.2/README.html)
* JSON
	* The TCL standard library includes a [JSON module](https://core.tcl-lang.org/tcllib/doc/tcllib-1-18/embedded/www/tcllib/files/modules/json/json.html)
* YAML
	* The TCL standard library includes a [YAML module](https://core.tcl-lang.org/tcllib/doc/tcllib-1-18/embedded/www/tcllib/files/modules/yaml/yaml.html)
* INI
	* The TCL standard library includes an [INI module](https://core.tcl-lang.org/tcllib/doc/tcllib-1-18/embedded/www/tcllib/files/modules/inifile/ini.html)
* CSV
	* The TCL standard library includes a [CSV module](https://core.tcl-lang.org/tcllib/doc/tcllib-1-18/embedded/www/tcllib/files/modules/csv/csv.html)

## TCL Resources

* [Tcl Library Source Code](https://core.tcl-lang.org/tcllib/doc/trunk/embedded/md/toc.md)
* [Practical Programming in Tcl and Tk (4th Edition)](https://www.amazon.com/Practical-Programming-Tcl-Tk-4th/dp/0130385603)
* [The Tcler's Wiki](https://wiki.tcl-lang.org/page/Online+Tcl+and+Tk+Tutorials)
* [Tcl/Tk Documentation](https://www.tcl-lang.org/man/tcl/contents.htm)
* [Tcl Programming/Tk examples](https://en.wikibooks.org/wiki/Tcl_Programming/Tk_examples)
