# noc-tools

[![CircleCI](https://circleci.com/gh/HeRCLab/nocsim.svg?style=svg)](https://circleci.com/gh/HeRCLab/nocsim)

noc-tools is a collection of network-on-chip oriented tools written by the
University of South Carolina Heterogeneous and Reconfigurable Computing
research group.

* **nocsim** is a software simulator for network on chip implementations
* **nocviz** is a visualization tool for NoCs using a simple TCL driven API to
  allow it to be used to visualize or control a variety of NoCs, including
  nocsim

The noc-tools user manual may be accessed [here](doc/noc-tools.md).

## Building noc-tools

noc-tools depends on the following libraries:

* libbsd
* libtcl
* [libAgar](http://libagar.org/)

On Ubuntu 18.04, run sudo apt install --yes libbsd-dev libreadline-dev tcl-dev libtcl8.6

noc-tools is built against the development version of libAgar 1.6.0. At
present, [SVN revision
10385](https://github.com/JulNadeauCA/libagar/commit/2cf5cac0d8e52f5752a2d55aea3116655b54e3a3)
is suggested.

Once all dependencies have been installed, simply run `./configure && make`. On
Ubuntu 18.04, you will need to provide the path to your TCL installation using
the `--with-tcl=/usr/include/tcl8.6/` parameter.

### Re-Generating `./configure`

noc-tools uses the [BSDBuild](http://bsdbuild.hypertriton.com/) build system.
You should be able to use the provided configure script without any
modifications. However, noc-tools developers may need to re-generate this
script from time to time, such as when new configuration flags are added.

The script may be re-generated using `mkconfigure < configure.in > configure`.
`configure.in` is used to specify what flags and options should be presented.
The documentation for `mkconfigure` may be found
[here](http://bsdbuild.hypertriton.com//mdoc.cgi?man=mkconfigure.1).

**NOTE** You should not use `mkify` unless you know what you are doing, as it
will overwrite specially modified make libraries used by noc-tools with the
BSDBuild default versions.

## Installation

noc-tools does not prescribe any specific installation method. It is generally
used by `source`-ing `scripts/noc_tools_load.tcl`. To that end, it is important
that the `nocsim/` and `nocviz/` directories remain in the same relative paths
to said script.
