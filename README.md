# `nocsim` - Network On Chip Simulator

[![CircleCI](https://circleci.com/gh/HeRCLab/nocsim.svg?style=svg)](https://circleci.com/gh/HeRCLab/nocsim)

This repository contains a simulator used to compare the relative performance
of various Network On Chip routing algorithms and topographies.

This project is supported by a University of South Carolina Magellan Apprentice
grant, and was used to collected research results for the poster *A Review of
FPGA-Oriented Network-on-Chip Routing Algorithms & Topographies* presented at
Discover USC, April 2019.

## About `nocsim`

`nocsim` is written in C, and allows for the simulation of arbitrary bufferless
network on chip topographies and routing algorithms. The collection of nodes in
the network, their connections, as well as their injection and routing
behaviors are fully configured via TCL. The nocsim TCL scripting manual can
be found [here](./doc/TCL.md).

## How to build `nocsim`

The nocsim source code is located in the [src/](./src/) directory, and may be
compiled via `make`. Dependencies:

* libbsd
* readline
* libtcl

On Ubuntu 18.04: `sudo apt install --yes libbsd-dev libreadline-dev tcl-dev libtcl8.6`

To build for development, run `make DEVELOP=true build`. You can then run
`./build/bin/nocsim` or `./build/bin/nocsim-gui`.

To install for production, run `make install`. You may wish to modify the
`PREFIX`. Run `make info` to see where nocsim will be installed; any binaries
will be placed in `BIN_DIR`, and the TCL library will be installed in
`TCL_DIR`.

### Testing `nocsim`

To run all automated checks, simply run the script `./scripts/ci.sh`. This
will require the following programs to be installed:

* [NASA-SW-VnV/ikos](https://github.com/NASA-SW-VnV/ikos)
* [terryyin/lizard](https://github.com/terryyin/lizard)
* [valgrind](http://valgrind.org/)

#### Testing Strategy

Tests are executed via the `scripts/ci.sh` script, which in turn executes
`scripts/ci/*.sh` one at a time, and considers there to be a build failure if
any of these scripts return a non-zero exit code.

* Two automated memory safety checkers are employed: valgrind and ikos, which
can help to catch memory leaks and unsafe behavior
* `nocsim` must always build with no warnings under `-Wall -Wextra -pedantic
-std=c99 -Werror`
* Lizard is used to validate that the cyclomatic complexity and file sizes of
`nocsim` are kept to manageable levels
* The script `scripts/ci/unittest.sh` executes a battery of unit tests via the
[tcltest](https://www.tcl-lang.org/man/tcl/TclCmd/tcltest.htm) library.
	* The unit tests are stored in `scripts/ci/unittest/*.tcl`
	* If a script outputs `FAILED`, or exits with a nonzero exit code (i.e.
	  in the case of a segfault), then the `unittest.sh` running considers
	  that test failed.

## Metrics of Interest

The `evalstats.py` script will gather a variety of metrics. These are described
as follows:

* Age: the age of all flits at time of arrival.
* Hops: the number of hops that a flit takes after it leaves its originating
  PE.
* Pending Flits: the number of flits pending across all PEs and cycles.
  A flit is pending once its originating PE has "decided" to inject it, but
  it has not yet been routed into the NoC.
* Change in Pending Flits: the numerically calculated slope in the above
  Pending Flits metric.
* Throughput: the number of flits which arrived on a given cycle.

All metrics are calculated with respect to injection rate.

**NOTE**: for the purposes of these results, it is assumed that all PEs have
the same injection rate. Although nocsim supports specifying a separate
injection rate for each PE, this is an implementation detail.

## Aside on FIFOs & Backpressure

`nocsim` assumes that each PE maintains an internal FIFO of outgoing flits to
be injected. In the future, `nocsim` will allow for the FIFO size to be
explicitly restricted, but this is not currently permitted.

Backpressure is implemented by causing routers to "backroute" any incoming
flit coming from its attached PE on any tick where all incoming non-PE links
contain a flit (since the number of incoming and outgoing links to a router
must be equal, and each incoming flit must be routed).

When a flit is "backrouted", is placed at the head, rather than the tail of
the FIFO.

## Limitations & Caveats

`nocsim` is a software simulator for a construct that is to be implemented in
hardware. As a result, it has several limitations and caveats inherent to
its design.

The most significant limitation is that `nocsim` has no mechanism to compute
the cost of a given topography or routing algorithm when implemented in
hardware. For example, it would likely prove very difficult to
place-and-route an undirected torus, as the links on the "edges" would need
to be very long.

`nocsim` also cannot account for issues such as propagation delay, maximum
clock rate, critical path, and the like. While a routing solution may perform
well under `nocsim`, it might be impossible to synthesize with an acceptable
clockrate on real hardware.

Finally, `nocsim` makes the assumption that any link may be characterized
either by a cardinal direction, or as connecting a PE to a router (or the
inverse). As a result, any grid element may link to not more than 5 other
grid elements.

## Future Work

There are number of ways in which `nocsim` and its associated scripts and
other tooling could be improved in the future.

* Additional routing behaviors could be implemented.

* Support for generating other, more complex topographies could be added.

* A mathematical model could be designed, against which to compare the results
  of `nocsim`.

## `nocsim-gui`

A graphical user interface is available for nocsim. See the
[README](./gui/README/md)`. 
