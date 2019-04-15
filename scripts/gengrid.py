#!/usr/bin/env python3

import sys
import argparse

parser = argparse.ArgumentParser(description="Generate nocsim grid definitions.")

parser.add_argument("--size", "-s", type=int, required=True,
        help="Grid size. All generated grids are square, and this is the "+
        "side length in routers.")

parser.add_argument("--topography", "-t", choices=["mesh", "torus", "utorus"],
        help="Specify the topogrophy to generate. 'mesh' refers to an " +
        "undirected mesh, 'torus' refers to a directed torus, and 'utorus' " +
        "refers to an undirected torus.", required=True)

parser.add_argument("--title", "-T", default=None,
        help="Simulation title, for reference purposes. If unspecified, " +
        "it will be generated based on other parameters.")

parser.add_argument("--P_inject", "-P", type=float, required=True,
        help="Probability in 0..1.0 that a given PE injects a flit " +
        "on a given cycle. This P is used for all nodes in the grid.")

parser.add_argument("--seed", "-S", type=int, required=True,
        help="RNG seed to use.")

parser.add_argument("--behavior", "-b", choices=["DOR", "ADOR"], required=True,
        help="Routing behavior to use for all routers in the grid. " +
        "DOR routing prioritizes packets based on what direction they " +
        "enter the router from. ADOR routing works similarly, but first " +
        "routes the oldest flit optimally before using DOR on remaining flits")

parser.add_argument("--output", "-o", default=sys.stdout,
        help="Specify output file. If unspecified, standard out is used. " +
        "If the output file is specified as 'auto', then then " +
        "./the_title.txt is used (with the_title being replaced with " +
        "the value of -T")

parser.add_argument("--ticks", "-k", type=int, required=True,
        help="Number of ticks to run the simulation for.")

args = parser.parse_args()

if args.title == None:
    args.title = "{}_{}_{}_{}_{}k".format(
            args.topography, args.size, int(args.P_inject*100), args.behavior,
            args.ticks // 1000)

if args.output == "auto":
    args.output = "./{}.txt".format(args.title)

if args.output is not sys.stdout:
    args.output = open(args.output, "w")

print("config RNG_seed {}".format(args.seed), file=args.output)
print("config max_ticks {}".format(args.ticks), file=args.output)
print("config default_P_inject {}".format(args.P_inject), file=args.output)
print("config title {}".format(args.title), file=args.output)

if args.topography == "mesh":
    count = 0
    routers = []
    for row in range(args.size):
        routers.append([])
        for col in range(args.size):
            R = "R{:03d}{:03d}".format(row, col)
            P = "P{:03d}{:03d}".format(row, col)
            routers[row].append(R)
            print("router {} {} {}".format(R, row, col), file=args.output)
            print("behavior {} {}".format(R, args.behavior), file=args.output)
            print("PE {} {} {}".format(P, row, col), file=args.output)
            print("link {} {}".format(P, R), file=args.output)
            print("link {} {}".format(R, P), file=args.output)

    # link them together
    for row in range(args.size):
        for col in range(args.size):
            for coord in [[row - 1, col], [row + 1, col], [row, col - 1], [row, col + 1]]:
                if coord[0] < 0 or coord[0] >= args.size:
                    continue
                if coord[1] < 0 or coord[1] >= args.size:
                    continue
                print("link {} {}".format(routers[row][col], routers[coord[0]][coord[1]]), file=args.output)
                print("link {} {}".format(routers[coord[0]][coord[1]], routers[row][col]), file=args.output)

elif args.topography == "torus":
    routers = []

    # instantiate all the routers
    for row in range(args.size):
        routers.append([])
        for col in range(args.size):
            R = "R{:03d}{:03d}".format(row, col)
            P = "P{:03d}{:03d}".format(row, col)
            routers[row].append(R)
            print("router {} {} {}".format(R, row, col), file=args.output)
            print("behavior {} {}".format(R, args.behavior), file=args.output)
            print("PE {} {} {}".format(P, row, col), file=args.output)
            print("link {} {}".format(P, R), file=args.output)
            print("link {} {}".format(R, P), file=args.output)

    # link them together
    for row in range(args.size):
        for col in range(args.size):
            for coord in [[row + 1, col], [row, col + 1]]:
                if coord[0] >= args.size:
                    print("link {} {}".format(routers[row][col], routers[0][coord[1]]))

                elif coord[1] >= args.size:
                    print("link {} {}".format(routers[row][col], routers[coord[0]][0]))

                else:
                    print("link {} {}".format(routers[row][col], routers[coord[0]][coord[1]]))

elif args.topography == "utorus":
    routers = []

    # instantiate all the routers
    for row in range(args.size):
        routers.append([])
        for col in range(args.size):
            R = "R{:03d}{:03d}".format(row, col)
            P = "P{:03d}{:03d}".format(row, col)
            routers[row].append(R)
            print("router {} {} {}".format(R, row, col), file=args.output)
            print("behavior {} {}".format(R, args.behavior), file=args.output)
            print("PE {} {} {}".format(P, row, col), file=args.output)
            print("link {} {}".format(P, R), file=args.output)
            print("link {} {}".format(R, P), file=args.output)

    # link them together
    for row in range(args.size):
        for col in range(args.size):
            for coord in [[row + 1, col], [row, col + 1]]:
                if coord[0] >= args.size:
                    print("link {} {}".format(routers[row][col], routers[0][coord[1]]))
                    print("link {} {}".format(routers[0][coord[1]], routers[row][col]))

                elif coord[1] >= args.size:
                    print("link {} {}".format(routers[row][col], routers[coord[0]][0]))
                    print("link {} {}".format(routers[coord[0]][0], routers[row][col]))

                else:
                    print("link {} {}".format(routers[row][col], routers[coord[0]][coord[1]]))
                    print("link {} {}".format(routers[coord[0]][coord[1]], routers[row][col]))

