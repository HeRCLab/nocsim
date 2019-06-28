# `nocsim` - Network On Chip Simulator

[![Build Status](https://travis-ci.org/HeRCLab/nocsim.svg?branch=master)](https://travis-ci.org/HeRCLab/nocsim)

This repository contains a simulator used to compare the relative performance
of various Network On Chip routing algorithms and topographies.

This project is supported by a University of South Carolina Magellan Apprentice
grant, and was used to collected research results for the poster *A Review of
FPGA-Oriented Network-on-Chip Routing Algorithms & Topographies* presented at
Discover USC, April 2019.

## About `nocsim`

`nocsim` is written in C, and allows for the simulation of arbitrary bufferless
network on chip topographies and routing algorithms.

`nocsim` consumes on standard input a "grid definition", which defines a set of
routers and PEs, as well as directional links between them. Each router has a
"behavior", which specifies how it routes the set of packets being sent into it
on each simulation tick. Each PE has an injection rate *P* which is the chance
that it "decides" to inject a flit (packet) into its attached router on each
simulation cycle.

The grid definition also includes metadata, such as a simulation title, RNG
seed (used to ensure deterministic output), and so on.

A collection of scripts are provided to reproduce the results presented in the
above noted poster in the [scripts/](./scripts/) directory. Among these are
[gengrid.py](./scripts/gengrid.py), which will programatically generate grid
definitions for a variety of common topographies and routing strategies, as well
as [evalstats.py](./scripts/evalstats.py) which is used to evaluate various
statistics about a simulation run of nocsim.

## How to build `nocsim`

The nocsim source code is located in the [src/](./src/) directory, and may be
compiled via `make`. Note that `libbsd` is required to compile nocsim.

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

* A graphical front-end could be written, allowing for detailed observation
  of the simulation state as it evolves.
