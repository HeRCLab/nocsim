#!/usr/bin/env python3

import math
import sys
import tqdm
import random

class Router:

    def __init__(this, row, col, idnum):
        this.row           = row
        this.col           = col
        this.id            = idnum
        this.links         = []
        this.incoming      = []
        this.incoming_next = []
        this.routed        = []
        this.deflected     = []

    def __str__(this):
        s = "<Router {}@({},{}) -> ".format(this.id, this.row, this.col)
        for link in this.links:
            s += "{}@({},{}), ".format(link.id, link.row, link.col)
        s = s[:-2]
        s += " >"
        return s

    def __repr__(this):
        return this.__str__()

    def to_graphviz(this):
        s = '"{}" [\nlabel = "{}: {} / {}\\n({}, {})"\npos = "{},{}!"\n]\n'.format(
                this.id, this.id, sum(this.deflected), sum(this.routed),
                this.row, this.col, this.col*200, this.row*200)
        for link in this.links:
            s += '"{}" -> "{}"\n'.format(this.id, link.id)
        return s

    def add_incoming(this, packet):
        this.incoming_next.append(packet)

    def flip(this):
        this.incoming = this.incoming_next;
        this.incoming_next = []

    def route(this, sortmethod, scoremethod):
        if not len(this.links) >= len(this.incoming):
            sys.stderr.write("------ BAD ---\n")
            sys.stderr.write(str(this) + "\n")
            sys.stderr.write(str(this.incoming) + "\n")

        packets = sortmethod(this.incoming)

        used = []
        deflected = 0
        routed = 0
        for packet in packets:
            # packet has arrived!
            if packet.dest == this:
                packet.done = True
                continue

            for link in scoremethod(this.links, packet):
                if link in used:
                    deflected += 1
                    continue
                else:
                    used.append(link)
                    packet.notify_route(link, deflected)
                    link.add_incoming(packet)
                    routed += 1
                    break

        this.routed.append(routed)
        this.deflected.append(deflected)
        this.incoming = []

class Packet:

    def __init__(this, src, dest, idnum):
        this.src       = src
        this.dest      = dest
        this.steps     = 0
        this.deflected = 0
        this.router    = src
        this.id        = idnum
        this.history   = []
        this.done      = False

    def __str__(this):
        return "<Packet {},{} - ({}) -> {},{}>".format(
                this.src.row, this.src.col, this.steps, this.dest.row,
                this.dest.col)

    def __repr__(this):
        return this.__str__()

    def notify_route(this, link, deflected):
        this.steps += 1
        this.deflected += deflected
        this.history.append(this.router)
        this.router = link

    def format_history(this, color):
        s = ""
        prev = this.history[0]
        for router in this.history[1:]:
            s += '"{}" -> "{}" [color="{}"]\n'.format(prev.id, router.id, color)
            prev = router
        s += '"{}" -> "{}" [color="{}"]\n'.format(prev.id, this.router.id,
                color)
        return s

def sort_method_none(packets):
    return packets

def score_method_cartesian(links, packet):
    scored = []
    for link in links:
        dist = math.sqrt(
                math.pow(packet.dest.col - link.col, 2) +
                math.pow(packet.dest.row - link.row, 2)
            )
        scored.append((dist, link))

    scored.sort(key=lambda x: x[0])

    return (x[1] for x in scored)

def main():
    width = 10
    height = 10
    gen_packets = 10000
    run_for = 100000
    packets_per_tick = 50
    routers = []
    packets = []

    # instantiate all the routers
    count = 0
    for row in range(height):
        routers.append([])
        for col in range(width):
            routers[row].append(Router(row, col, count))
            count += 1

    # link them together
    for row in range(height):
        for col in range(width):
            for coord in [[row - 1, col], [row + 1, col], [row, col - 1], [row, col + 1]]:
                if coord[0] < 0 or coord[0] >= height:
                    continue
                if coord[1] < 0 or coord[1] >= width:
                    continue
                routers[row][col].links.append(routers[coord[0]][coord[1]])

    # simulate
    for tick in tqdm.tqdm(range(run_for)):

        for row in range(height):
            for col in range(width):
                routers[row][col].route(sort_method_none,
                        score_method_cartesian)

        if gen_packets > 0:
            for i in range(packets_per_tick):
                gen_packets -= 1

                row1 = random.randint(0, height - 1)
                col1 = random.randint(0, width - 1)
                src = routers[row1][col1]

                p = Packet(src, routers[height//2][width//2], tick)
                sys.stderr.write(str(p))
                sys.stderr.write("\n")
                routers[row1][col1].add_incoming(p)
                packets.append(p)

        for row in range(height):
            for col in range(width):
                routers[row][col].flip()

    print("digraph G {")
    for row in range(height):
        for col in range(width):
            print(routers[row][col].to_graphviz())
    print(packets[0].format_history("red"))
    print(packets[1].format_history("green"))
    print(packets[2].format_history("blue"))
    print("}")
    sys.stderr.write(str(packets[0].done) + "\n")
    sys.stderr.write(str(packets[1].done) + "\n")
    sys.stderr.write(str(packets[2].done) + "\n")


if __name__ == "__main__":
    main()
