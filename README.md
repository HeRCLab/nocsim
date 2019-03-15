# `nocsim` - Network On Chip Simulator

This repository contains a simulator used to compare the relative performance
of various Network On Chip routing algorithms and topographies.

This project is supported by a University of South Carolina Magellan Apprentice
grant.

## Running

Execute `python3 -m nocsim.cli` from the top-level directory of the project.
This will create two files: `packets.json` and `router.json`, which contain a
detailed tick-by-tick report of the simulation.

A visualization tool is also provided, which is implemented using d3.js. You
may run the visualization tool by executing `python3 -m http.server` in the
top-level directory of the project (after having run a simulation to
completion). You should then open a web browser and navigate to
`http://localhost:8000` to view the visualization.

## Dependencies

The following Python 3 modules need to be installed. You are encouraged to use
a virtual environment for this purpose.

* tqdm
* colormap
* easydev

## Metrics

For a given simulation, we might vary the minimum and maximum number of flits
injected per cycle (which are injected into random routers in the array), the
network topography, number of routers, and routing algorithm. We collect the
following metrics (based largely on those used by Kapre & Gray):

* Throughput in flit-routers per cycle with respect to injection rate
  flit-routers per cycle.
* Average Latency in cycles with respect to throughput in flit-routers per
  cycle.
* Throughput in flits per cycle with respect to routers.

**NOTE** nocsim makes the assumption that every router is associated with
exactly one PE.

**NOTE** nocsim makes the assumption that every packet is associated with
exactly one flit.

**NOTE** because nocsim is a software based simulation, it is not able to
measure the resource utilization or maximum achievable clock rate for each
configuration.

## Score Methods

nocsim supports several *Scoring Methods*, this is an abstraction used when a
router is deciding which link to send a packet down on a given cycle. The
packet and all links on that router are passed to the selected scoring method,
which returns a list of permitted links for the packet to be routed on, sorted
by how much the packet would "prefer" to be routed down that link.

Packets are sorted by the *Sorting Method*, and are routed down their most
preferred permitted link which has not been used in the current simulation
cycle (i.e. by another packet which was sorted higher).

While this may seem very abstract, this allows many different possible routing
algorithms to be implemented using the same infrastructure.

The following scoring methods are supported:

* "cartesian": in this method, any outgoing link is permitted, and links are
  sorted based on the cartesian distance from the router on the other end of
  the link to the packet's target router. This is not a practical routing
  method for FPGA-based NOC implementations, but is easy to understand as a
  point of comparison.

* "DOR" (domain-ordered-routing): as suggested by Kapre and Gray (among
  others). With this method, packets which are not already in the same row as
  their destination may only take links which change their row, and otherwise
  may only take links which change their column. If multiple permitted links
  are found, then cartesian distance is used as a tie-breaker (note that this
  is an implement ion detail of nocsim -- the same behavior could be
  implemented in hardware using a simple compactor).
