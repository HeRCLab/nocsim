#!/usr/bin/env python3

import argparse
import collections
import numpy

parser = argparse.ArgumentParser(description="""
Toy script to play with hub routing. This script generates a hub routing table
for a height x width NOC. This script assumes an undirected torus topology.
Assumes each node is attached to exactly one PE. This also assumes that there
is one master node that every other node will want to send / receive from.
The output format is TSV
""")

parser.add_argument("--width", "-w", default=10, type=int,
    help="Specify width of torus in nodes. (default: 10)")

parser.add_argument("--height", "-e", default=10, type=int,
    help="Specify height of torus in nodes. (default: 10)")

parser.add_argument("--master_row", "-r", default=0, type=int,
    help="Specify row of master node, zero-indexed. (default: 0)")

parser.add_argument("--master_col", "-c", default=0, type=int,
    help="Specify col of master node, zero-indexed. (default: 0)")

action = parser.add_mutually_exclusive_group(required=False)

action.add_argument("--from", "-f", nargs=2, type=int, default=None,
    help="Rather than the routing table TSV, print the list of hops to " +
    "route from the given row,col point to the master node.")

action.add_argument("--to", "-t", nargs=2, type=int, default=None,
    help="Rather than the routing table TSV, print the list of hops to " +
    "route to the given row,col point from the master node.")

args = parser.parse_args()

master = (args.master_row, args.master_col)
routes = collections.defaultdict(list)


#
#       :      :      :      :
#       |      |      |      |
#       v      v      v      v
#  ...->O <--> O <--> O <--> O <-...             N
#       |      |      |      |                   ^
#       |      |      |      |                   |
#       v      v      v      v              W <--*--> E
#  ...->O <--> O <--> O <--> O <-...             |
#       |      |      |      |                   v
#       |      |      |      |                   S
#       v      v      v      v
#  ...->O <--> O <--> O <--> O <-...
#       |      |      |      |
#       |      |      |      |
#       v      v      v      v
#  ...->O <--> O <--> O <--> O <-...
#       ^      ^      ^      ^
#       |      |      |      |
#       :      :      :      :
#
# We assume 0,0 is the top-left, and that everything is row major

def dist(p1, p2):
    p1 = numpy.asarray(p1)
    p2 = numpy.asarray(p2)

    # account for being "over the edge" of the torus
    return min([numpy.sqrt(numpy.sum((p[0]-p[1])**2)) for p in [
            (p1, p2),
            ((p1[0] + args.height, p1[1]), p2),
            ((p1[0] - args.height, p1[1]), p2),
            ((p1[0] + args.height, p1[1] + args.width), p2),
            ((p1[0] - args.height, p1[1] - args.width), p2),
            ((p1[0], p1[1] + args.width), p2),
            ((p1[0], p1[1] - args.width), p2)]])


# returns the distance between the line drawn from p1 to p2 and the point p3
# https://stackoverflow.com/a/39840218
def point2line(p1, p2, p3):
    p1 = numpy.asarray(p1)
    p2 = numpy.asarray(p2)
    p3 = numpy.asarray(p3)
    return numpy.linalg.norm(numpy.abs(numpy.cross(p2-p1, p1-p3)))/numpy.linalg.norm(p2-p1)

# map a logical point to a physical point
def map_point(p):
    p = list(p)
    if p[0] >= args.height:
        p[0] -= args.height

    while p[0] < 0:
        p[0] += args.height

    if p[1] >= args.width:
        p[1] -= args.width

    while p[1] < 0:
        p[1] += args.width

    return tuple(p)

# given that we are routing from src to dest, and we are current at the node
# current, what is the next node to route to?
def minimal_path(src, dest, current):
    choices = [
        map_point((current[0] + 1, current[1])),
        map_point((current[0], current[1] + 1)),
        map_point((current[0] - 1, current[1])),
        map_point((current[0], current[1] - 1))
    ]

    choices = [(c[0], c[1], point2line(src, dest, c), dist(c, dest)) for c in choices]
    choices.sort(key = lambda x : x[0])  # sort by row as a tiebreaker
    choices.sort(key = lambda x : x[2])  # sort by closeness to hub line
    choices.sort(key = lambda x : x[3])  # sort by distance to dest

    #  print(current)
    #  for c in choices:
    #      print("\t" + str(c))

    return map_point(choices[0][0:2])

def compute_route(src, dest):

    dest = list(dest)
    if dest[0] < src[0]:
        dest[0] += args.height

    if dest[1] < src[1]:
        dest[1] += args.width
    dest = tuple(dest)

    current = src
    path = []
    while map_point(current) != map_point(dest):
        path.append(current)
        current = minimal_path(src, dest, current)
    path.append(current)
    return path

# Build the routing table for a given node routing to the given destination.
# The table matches originating node to next-hop from the current node.
def build_table_for_node(node, dest):
    nodes = [(row, col) for row in range(0, args.height) for col in range(0, args.width)]
    table = []

    # loop over every possible source
    for cursor in nodes:

        # we are here
        if cursor == dest:
            continue

        # we can't route from ourself to ourself
        if node == dest:
            continue

        table.append((node, cursor, dest, minimal_path(cursor, dest, node)))

    return table

if args.__dict__["from"] is not None:
    for p in compute_route((args.__dict__["from"][0], args.__dict__["from"][1]),
            (args.master_row, args.master_col)):
        print(p)

elif args.to is not None:
    for p in compute_route((args.master_row, args.master_col),
            (args.to[0], args.to[1])):
        print(p)

else:

    print("hub routing, (w x h) {} x {}, master @ (row, col) {}, {}".format(args.width, args.height, args.master_row, args.master_col))

    table = []

    nodes = [(row, col) for row in range(0, args.height) for col in range(0, args.width)]

    for n in nodes:
        if n == master:
            continue
        table += build_table_for_node(n, master)

    print("ROUTING TABLE FOR ALL NODES")
    for t in table:
        # node, origin, dest, nexthop
        print("ROUTE\t" + "\t".join([str(e) for e in t]))

    # calculate pressure on links
    links = collections.defaultdict(int)

    for n in nodes:
        last = None
        for r in compute_route(n, master):
            if last is None:
                last = r
                continue
            links[(last, r)] += 1
            last = r

    # print contention table sorted by keys
    print("LINK PRESSURE FOR ALL LINKS")
    for row in sorted( ((v,k) for k,v in dict(links).items()), reverse=True):
        print("PRESSURE\t"+str(row[1])+"\t"+str(row[0]))
