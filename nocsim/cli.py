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

def linreg(X, Y):
    # https://mubaris.com/posts/linear-regression/

    # Mean X and Y
    mean_x = statistics.mean(X)
    mean_y = statistics.mean(Y)

    # Total number of values
    m = len(X)

    # Using the formula to calculate b1 and b2
    numer = 0
    denom = 0
    for i in range(m):
        numer += (X[i] - mean_x) * (Y[i] - mean_y)
        denom += (X[i] - mean_x) ** 2
    b1 = numer / denom
    b0 = mean_y - (b1 * mean_x)

    return b0, b1

routers = None

if args.topography == "mesh":
    routers = nocsim.simulation.generate_mesh(args.size, args.size)

sort_method = nocsim.simulation.sort_method_none
score_method = None
if args.score_method == "cartesian":
    score_method = nocsim.simulation.score_method_cartesian
elif args.score_method == "DOR":
    score_method = nocsim.simulation.score_method_DOR

packets, backpressured, spawned, queued = nocsim.simulation.simulate(routers, args.flits_per_tick, args.ticks, sort_method, score_method)

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
print ("                  linear regression: {}".format(linreg(range(len(backpressured)), backpressured)))

print("---- SPAWNED ---------------------------------------------------------")
print("    mean # spawned PEs / cycle: {}".format(statistics.mean(spawned)))
print("  median # spawned PEs / cycle: {}".format(statistics.median(spawned)))
print("   stdev # spawned PEs / cycle: {}".format(statistics.stdev(spawned)))
print("variance # spawned PEs / cycle: {}".format(statistics.variance(spawned)))
print ("            linear regression: {}".format(linreg(range(len(spawned)), spawned)))

print("---- QUEUED ----------------------------------------------------------")
print("    mean # queued PEs / cycle: {}".format(statistics.mean(queued)))
print("  median # queued PEs / cycle: {}".format(statistics.median(queued)))
print("   stdev # queued PEs / cycle: {}".format(statistics.stdev(queued)))
print("variance # queued PEs / cycle: {}".format(statistics.variance(queued)))
print("            linear regression: {}".format(linreg(range(len(queued)), queued)))

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

sys.stdout.write("ticks\t")
sys.stdout.write("flits per tick\t")
sys.stdout.write("size\t")
sys.stdout.write("topography\t")
sys.stdout.write("total packets")
sys.stdout.write("\t")
sys.stdout.write("total cycles")
sys.stdout.write("\t")
sys.stdout.write("score method")
sys.stdout.write("\t")
sys.stdout.write("sort method")
sys.stdout.write("\t")
sys.stdout.write(" mean # backpressured PEs / cycle")
sys.stdout.write("\t")
sys.stdout.write("median # backpressured PEs / cycle")
sys.stdout.write("\t")
sys.stdout.write("stdev # backpressured PEs / cycle")
sys.stdout.write("\t")
sys.stdout.write("variance # backpressured PEs / cycle")
sys.stdout.write("\t")
sys.stdout.write("linear regression")
sys.stdout.write("\t")
sys.stdout.write("mean # spawned PEs / cycle")
sys.stdout.write("\t")
sys.stdout.write("median # spawned PEs / cycle")
sys.stdout.write("\t")
sys.stdout.write("stdev # spawned PEs / cycle")
sys.stdout.write("\t")
sys.stdout.write("variance # spawned PEs / cycle")
sys.stdout.write("\t")
sys.stdout.write("linear regression")
sys.stdout.write("\t")
sys.stdout.write("mean # queued PEs / cycle")
sys.stdout.write("\t")
sys.stdout.write(" median # queued PEs / cycle")
sys.stdout.write("\t")
sys.stdout.write("stdev # queued PEs / cycle")
sys.stdout.write("\t")
sys.stdout.write("variance # queued PEs / cycle")
sys.stdout.write("\t")
sys.stdout.write("linear regression")
sys.stdout.write("\t")
sys.stdout.write("throughput (flit-routers per c)")
sys.stdout.write("\t")
sys.stdout.write("injection rate (flit-routers per c)")
sys.stdout.write("\t")
sys.stdout.write("latency (flit-routers per c)")
sys.stdout.write("\t")
sys.stdout.write("throughput (flit-routers per c)")
sys.stdout.write("\t")
sys.stdout.write("throughput (flit per c)")
sys.stdout.write("\t")
sys.stdout.write("# of routers")
sys.stdout.write("\t")
sys.stdout.write("\n")

sys.stdout.write(str(args.ticks))
sys.stdout.write("\t")
sys.stdout.write(str(args.flits_per_tick))
sys.stdout.write("\t")
sys.stdout.write(str(args.size))
sys.stdout.write("\t")
sys.stdout.write(str(args.topography))
sys.stdout.write("\t")
sys.stdout.write(str(len(packets)))
sys.stdout.write("\t")
sys.stdout.write(str(args.ticks))
sys.stdout.write("\t")
sys.stdout.write(str(args.score_method))
sys.stdout.write("\t")
sys.stdout.write(str(sort_method))
sys.stdout.write("\t")
sys.stdout.write(str(statistics.mean(backpressured)))
sys.stdout.write("\t")
sys.stdout.write(str(statistics.median(backpressured)))
sys.stdout.write("\t")
sys.stdout.write(str(statistics.stdev(backpressured)))
sys.stdout.write("\t")
sys.stdout.write(str(statistics.variance(backpressured)))
sys.stdout.write("\t")
sys.stdout.write(str(linreg(range(len(backpressured)), backpressured)))
sys.stdout.write("\t")
sys.stdout.write(str(statistics.mean(spawned)))
sys.stdout.write("\t")
sys.stdout.write(str(statistics.median(spawned)))
sys.stdout.write("\t")
sys.stdout.write(str(statistics.stdev(spawned)))
sys.stdout.write("\t")
sys.stdout.write(str(statistics.variance(spawned)))
sys.stdout.write("\t")
sys.stdout.write(str(linreg(range(len(spawned)), spawned)))
sys.stdout.write("\t")
sys.stdout.write(str(statistics.mean(queued)))
sys.stdout.write("\t")
sys.stdout.write(str(statistics.median(queued)))
sys.stdout.write("\t")
sys.stdout.write(str(statistics.stdev(queued)))
sys.stdout.write("\t")
sys.stdout.write(str(statistics.variance(queued)))
sys.stdout.write("\t")
sys.stdout.write(str(linreg(range(len(queued)), queued)))
sys.stdout.write("\t")
sys.stdout.write(str(throughput))
sys.stdout.write("\t")
sys.stdout.write(str(injection_rate))
sys.stdout.write("\t")
sys.stdout.write(str(latency))
sys.stdout.write("\t")
sys.stdout.write(str(throughput))
sys.stdout.write("\t")
sys.stdout.write(str(throughput))
sys.stdout.write("\t")
sys.stdout.write(str(num_routers))
sys.stdout.write("\n")

#  with open("routers.json", "w") as f:
#      json.dump(nocsim.simulation.dump_routers(routers), f)
#  
#  with open("packets.json", "w") as f:
#      json.dump(nocsim.simulation.dump_packets(packets), f)
