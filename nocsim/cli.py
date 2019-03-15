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

parser.add_argument("--score_method", "-S", default="cartesian",
        choices = ["cartesian", "DOR"],
        help="Change the method which is used to score possible links" + 
        " which a packet might take. (default: cartesian)")

parser.add_argument("--ticks", "-T", default=10000, type=int,
        help="Specify number of ticks to run for (default: 10000)")

args = parser.parse_args()

routers = None

if args.topography == "mesh":
    routers = nocsim.simulation.generate_mesh(args.size, args.size)

sort_method = nocsim.simulation.sort_method_none
score_method = None
if args.score_method == "cartesian":
    score_method = nocsim.simulation.score_method_cartesian
elif args.score_method == "DOR":
    score_method = nocsim.simulation.score_method_DOR

packets, backpressured, spawned = nocsim.simulation.simulate(routers, args.flits_per_tick, args.ticks, sort_method, score_method)

print("---- GENERAL ---------------------------------------------------------")
print("total packets: {}".format(len(packets)))
print(" total cycles: {}".format(args.ticks))
print(" score method: {}".format(str(score_method)))
print("  sort method: {}".format(str(sort_method)))

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

print("---- THROUGHPUT (flits per cycle) ------------------------------------")
throughput, num_routers = nocsim.metrics.throughput_fpc(routers, packets, args.flits_per_tick, args.ticks)
print("  throughput: {:3.3f} flit per cycle".format(throughput))
print("# of routers: {:3.3f}".format(num_routers))

with open("routers.json", "w") as f:
    json.dump(nocsim.simulation.dump_routers(routers), f)

with open("packets.json", "w") as f:
    json.dump(nocsim.simulation.dump_packets(packets), f)
