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
