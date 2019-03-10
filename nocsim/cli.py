#!/usr/bin/env python3

import os
import sys
import json
import argparse
import statistics

sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), "../"))

import nocsim
import nocsim.simulation
import nocsim.metrics

parser = argparse.ArgumentParser()

parser.add_argument("--topography", "-t", default="mesh", choices = ["mesh"],
        help="Specify the top of topography to use. 'mesh': bidirectional mesh")

parser.add_argument("--size", "-s", default=10, type=int,
        help="Specify the grid size as an integer (default: 10)")

parser.add_argument("--flits_per_tick", "-p", default=0.5, type=float,
        help="Specify chance that a given PE tries to send a flit " +
        "each tick (default: 0.5)")

parser.add_argument("--ticks", "-T", default=10000, type=int,
        help="Specify number of ticks to run for (default: 10000)")

# TODO: allow selecting routing algorithm

args = parser.parse_args()

routers = None

if args.topography == "mesh":
    routers = nocsim.simulation.generate_mesh(args.size, args.size)

packets, backpressured, spawned = nocsim.simulation.simulate(routers, args.flits_per_tick, args.ticks)

print("---- GENERAL ---------------------------------------------------------")
print("total packets: {}".format(len(packets)))
print("total cycles : {}".format(args.ticks))

print("---- BACKPRESSURE ----------------------------------------------------")
print("    mean # backpressured PEs / cycle: {}".format(statistics.mean(backpressured)))
print("  median # backpressured PEs / cycle: {}".format(statistics.median(backpressured)))
print("   stdev # backpressured PEs / cycle: {}".format(statistics.stdev(backpressured)))
print("variance # backpressured PEs / cycle: {}".format(statistics.variance(backpressured)))

print("---- SPAWNED ---------------------------------------------------------")
print("    mean # spawned PEs / cycle: {}".format(statistics.mean(spawned)))
print("  median # spawned PEs / cycle: {}".format(statistics.median(spawned)))
print("   stdev # spawned PEs / cycle: {}".format(statistics.stdev(spawned)))
print("variance # spawned PEs / cycle: {}".format(statistics.variance(spawned)))


print("---- THROUGHPUT ------------------------------------------------------")
throughput, injection_rate = nocsim.metrics.throughput(routers, packets, args.flits_per_tick, args.ticks)
print("    throughput: {:3.3f} flit-routers per cycle".format(throughput))
print("injection rate: {:3.3f} flit-routers per cycle".format(injection_rate))

print("---- LATENCY ---------------------------------------------------------")
latency, throughput = nocsim.metrics.latency(routers, packets, args.flits_per_tick, args.ticks)
print("   latency: {:3.3f} flit-routers per cycle".format(latency))
print("throughput: {:3.3f} flit-routers per cycle".format(throughput))

with open("routers.json", "w") as f:
    json.dump(nocsim.simulation.dump_routers(routers), f)

with open("packets.json", "w") as f:
    json.dump(nocsim.simulation.dump_packets(packets), f)
