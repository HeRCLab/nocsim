import nocsim.router
import tqdm
import nocsim.packet
import random
import math
import colormap
import json

def generate_mesh(width, height):
    """generate_mesh

    Generates a 2d bi-directional mesh configuration.

    :param width:
    :param height:
    """
    routers = []

    # instantiate all the routers
    count = 0
    for row in range(height):
        routers.append([])
        for col in range(width):
            routers[row].append(nocsim.router.Router(row, col, count))
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

    return routers

def sort_method_none(packets):
    return packets

def score_method_cartesian(links, packet):
    scored = []
    for link in links:
        dist = (packet.dest.col - link.col)**2 +(packet.dest.row - link.row)**2
        scored.append((dist, link))

    scored.sort(key=lambda x: x[0])

    return (x[1] for x in scored)


def simulate(routers, packets_per_tick=0.5, run_for=50000):
    packets = []

    # number of times a PE wanted to spawn a flit, but the router did not
    # have an bailable incoming slot
    backpressured = []

    # spawned
    spawned = []

    height = len(routers)
    width = len(routers[0])

    idnum = 0

    # simulate
    for tick in tqdm.tqdm(range(run_for)):
        backpressured_this_tick = 0
        spawned_this_tick = 0

        # calculate routes for this tick
        for row in range(height):
            for col in range(width):
                routers[row][col].route(sort_method_none,
                        score_method_cartesian)

        # generate flits for this tick
        for row in range(height):
            for col in range(width):
                src_router = routers[row][col]

                # with P=packets_per_tick, generate a flit from this PE
                if random.random() > packets_per_tick:
                    continue

                # backpressured
                if len(src_router.incoming_next) >= len(src_router.links):
                    backpressured_this_tick += 1
                    continue

                # pick a destination at random
                dest_row = random.randint(0, height - 1)
                dest_col = random.randint(0, width - 1)
                dest_router = routers[dest_row][dest_col]

                # we found a suitable router, add the packet to it
                p = nocsim.packet.Packet(src_router, dest_router, idnum, tick)
                idnum += 1
                spawned_this_tick += 1
                src_router.add_incoming(p)
                packets.append(p)

        backpressured.append(backpressured_this_tick)
        spawned.append(spawned_this_tick)

        # flip over the simulation state for the next tick
        for row in range(height):
            for col in range(width):
                routers[row][col].flip()

    return packets, backpressured, spawned


def dump_routers(routers):
    height = len(routers)
    width = len(routers[0])
    data = {}
    for row in range(height):
        for col in range(width):
            router = routers[row][col]
            data[router.id] = {
                "row": router.row,
                "col": router.col,
                "id": router.id,
                "links": list([r.id for r in router.links]),
                "routed": router.routed,
                "deflected": router.deflected,
                "totalrouted": sum(router.routed),
                "totaldeflected": sum(router.deflected)
            }
    return data

def dump_packets(packets):
    data = {}

    for packet in packets:
        if packet.created_tick not in data:
            data[packet.created_tick] = []

        data[packet.created_tick].append({
            "src": packet.src.id,
            "dest": packet.dest.id,
            "steps": packet.steps,
            "deflected": packet.deflected,
            "id": packet.id,
            "history": [r.id for r in packet.history],
            "created": packet.created_tick,
            "done": packet.done
        })

    return data
