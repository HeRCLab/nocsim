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

parser.add_argument("--topography", "-t", default="mesh", choices = ["mesh", "torus"],
        help="Specify the top of topography to use. 'mesh': bidirectional "+
        "mesh, 'torus': 2D directional torus")

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

elif args.topography == "torus":
    routers = nocsim.simulation.generate_torus(args.size, args.size)

#  sort_method = nocsim.simulation.sort_method_none
sort_method = nocsim.simulation.sort_method_EWNS
score_method = None
if args.score_method == "cartesian":
    score_method = nocsim.simulation.score_method_cartesian
elif args.score_method == "DOR":
    score_method = nocsim.simulation.score_method_DOR

packets, backpressured, spawned, queued = nocsim.simulation.simulate(routers, args.flits_per_tick, args.ticks, sort_method, score_method)

deflected = [p.deflected for p in packets]

congestion = linreg(range(len(queued)), queued)[1]

results = {}
results["congestion"] = congestion
results["total packets"] = len(packets)
results["total cycles"] = args.ticks
results["score method"] = str(score_method)
results["sort method"] = str(sort_method)
results["mean deflected"] = statistics.mean(deflected)
results["median deflected"] = statistics.median(deflected)
results["stdev deflected"] = statistics.stdev(deflected)
results["mean # backpressured PEs / cycle"] = statistics.mean(backpressured)
results["median # backpressured PEs / cycle"] = statistics.median(backpressured)
results["stdev # backpressured PEs / cycle"] = statistics.stdev(backpressured)
results["variance # backpressured PEs / cycle"] = statistics.variance(backpressured)
results["linear regression (backpressured)"] = linreg(range(len(backpressured)), backpressured)
results["mean # spawned PEs / cycle"] = statistics.mean(spawned)
results["median # spawned PEs / cycle"] = statistics.median(spawned)
results["stdev # spawned PEs / cycle"] = statistics.stdev(spawned)
results["variance # spawned PEs / cycle"] = statistics.variance(spawned)
results["linear regression (spawned)"] = linreg(range(len(spawned)), spawned)
results["mean # queued PEs / cycle"] = statistics.mean(queued)
results["median # queued PEs / cycle"] = statistics.median(queued)
results["stdev # queued PEs / cycle"] = statistics.stdev(queued)
results["variance # queued PEs / cycle"] = statistics.variance(queued)
results["linear regression (queued)"] = linreg(range(len(queued)), queued)
throughput, injection_rate = nocsim.metrics.throughput(routers, packets, args.flits_per_tick, args.ticks)
results["throughput"] = throughput
results["injection rate"] = injection_rate
latency, throughput = nocsim.metrics.latency(routers, packets, args.flits_per_tick, args.ticks)
results["latency"] = latency
throughput, num_routers = nocsim.metrics.throughput_fpc(routers, packets, args.flits_per_tick, args.ticks)
results["# of routers"] = num_routers
latency_worst = nocsim.metrics.latency_worst(routers, packets, args.flits_per_tick, args.ticks)
results["worst case latency"] = latency_worst
overall_inflation, inflation_by_dist = nocsim.metrics.path_inflation(routers, packets)
results["median inflation"] = statistics.median(overall_inflation)
results["mean inflation"] = statistics.mean(overall_inflation)
results["stdev inflation"] = statistics.stdev(overall_inflation)
results["variance inflation"] = statistics.variance(overall_inflation)

for k in results.keys():
    print(k, ":", results[k])


for dist in inflation_by_dist.keys():
    print(dist, ":", statistics.median(inflation_by_dist[dist]), statistics.mean(inflation_by_dist[dist]))

print('\t'.join((k for k in results.keys())))
print('\t'.join((str(results[k]) for k in results.keys())))

#  with open("routers.json", "w") as f:
#      json.dump(nocsim.simulation.dump_routers(routers), f)
#
#  with open("packets.json", "w") as f:
#      json.dump(nocsim.simulation.dump_packets(packets), f)
