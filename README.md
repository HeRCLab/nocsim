# `nocsim` - Network On Chip Simulator

[![CircleCI](https://circleci.com/gh/HeRCLab/nocsim.svg?style=svg)](https://circleci.com/gh/HeRCLab/nocsim)

This repository contains a simulator used to compare the relative performance
of various Network On Chip routing algorithms and topographies.

This project was originally supported by a University of South Carolina
Magellan Apprentice grant, and was used to collected research results for the
poster *A Review of FPGA-Oriented Network-on-Chip Routing Algorithms &
Topographies* presented at Discover USC, April 2019. Work has continued under
support from the University of South Carolina Heterogeneous and Reconfigurable
Computing (HeRC) research group).

## About `nocsim`

`nocsim` is written in C, and allows for the simulation of (nearly) arbitrary
network on chip topographies and routing algorithms. The collection of nodes in
the network, their connections, as well as their injection and routing
behaviors are fully configured via TCL. The nocsim TCL scripting manual can be
found [here](./doc/TCL.md).

## How to Build & Install `nocsim`

The nocsim source code is located in the [src/](./src/) directory, and may be
compiled via `make`. Dependencies:

* libbsd
* readline
* libtcl

On Ubuntu 18.04: `sudo apt install --yes libbsd-dev libreadline-dev tcl-dev libtcl8.6`

To build for development, run `make DEVELOP=true build gui`. You can then run
`./build/bin/nocsim` or `./build/bin/nocsim-gui`.

To install for production, run `make build gui && make install`. You may wish
to modify the `PREFIX`. Run `make info` to see where nocsim will be installed;
any binaries will be placed in `BIN_DIR`, and the TCL library will be installed
in `TCL_DIR`.

**Note**: the install path of `nocsim`'s TCL library is burned into the binary
at compile time. If the TCL library is re-located, you need to re-compile the
binary.

### Testing `nocsim`

To run all automated checks, simply run the script `./scripts/ci.sh`. This
will require the following programs to be installed:

* [NASA-SW-VnV/ikos](https://github.com/NASA-SW-VnV/ikos)
* [terryyin/lizard](https://github.com/terryyin/lizard)
* [valgrind](http://valgrind.org/)

For more information, see the [CI README](./ci/).

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

## Limitations & Caveats

`nocsim` is a software simulator for a construct that is to be implemented in
hardware. As a result, it has several limitations and caveats inherent to
its design.

The most significant limitation is that `nocsim` has no mechanism to compute
the cost of a given topography or routing algorithm when implemented in
hardware.

`nocsim` also cannot account for issues such as propagation delay, maximum
clock rate, critical path, and the like. While a routing solution may perform
well under `nocsim`, it might be impossible to synthesize with an acceptable
clockrate on real hardware.

Finally, `nocsim` makes the assumption that any link may be characterized
either by a cardinal direction, or as connecting a PE to a router (or the
inverse). As a result, any grid element may link to not more than 5 other
grid elements.

## `nocsim-gui`

A graphical user interface is available for nocsim. See the
[README](./gui/README/md)`.
