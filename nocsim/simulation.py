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

def generate_torus(width, height):
    """generate_torus

    Generates a 2d-directional torus configuration.

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
            for coord in [[row + 1, col], [row, col + 1]]:
                if coord[0] >= height:
                    routers[row][col].links.append(routers[0][coord[1]])

                elif coord[1] >= width:
                    routers[row][col].links.append(routers[coord[0]][0])

                else:
                    routers[row][col].links.append(routers[coord[0]][coord[1]])

    return routers

def sort_method_none(packets, router):
    return packets

def sort_method_EWNS(packets, router):
    N = None
    W = None
    S = None
    E = None

    for packet in packets:
        if len(packet.history) < 1:
            continue

        if packet.history[-1].row < router.row:
            N = packet
        elif packet.history[-1].row > router.row:
            S = packet
        elif packet.history[-1].col > router.col:
            E = packet
        else:
            W = packet

    result = [p for p in [E, W, N, S] if p is not None]
    for packet in packets:
        if packet not in result:
            result.append(packet)

    return result

def score_method_cartesian(links, packet):
    scored = []
    for link in links:
        dist = (packet.dest.col - link.col)**2 +(packet.dest.row - link.row)**2
        scored.append((dist, link))

    scored.sort(key=lambda x: x[0])

    return (x[1] for x in scored)

def score_method_DOR(links, packet):
    """score_method_DOR

    Implements dimension-ordered routing. First the packet is routed to the
    Y coordinate (row) of it's destination, then to the X coordinate (column).

    :param links:
    :param packet:
    """

    # Already on the right row, so we only allow links where the row does
    # not change
    if packet.dest.row == packet.router.row:
        preferred = [link for link in links if link.row == packet.router.row]
        alternate = [link for link in links if link.row != packet.router.row]

    # Stay in the same column and consider different rows
    else:
        preferred = [link for link in links if link.col == packet.router.col]
        alternate = [link for link in links if link.col != packet.router.col]

    # We now have only allowable links for DOR routing. We can now pass this
    # on to the cartesian scoring method, since it will select an appropriate
    # link from those that remain.

    scored_preferred = score_method_cartesian(preferred, packet)
    scored_alternate = score_method_cartesian(alternate, packet)

    return list(scored_preferred) + list(scored_alternate)

def simulate(routers, packets_per_tick=0.5, run_for=50000, sort_method=sort_method_none, score_method=score_method_cartesian):
    packets = []

    # number of times a PE wanted to spawn a flit, but the router did not
    # have an bailable incoming slot
    backpressured = []

    # spawned
    spawned = []
    queued = []

    height = len(routers)
    width = len(routers[0])

    idnum = 0

    # simulate
    #  for tick in tqdm.tqdm(range(run_for)):
    for tick in range(run_for):
        backpressured_this_tick = 0
        spawned_this_tick = 0
        queued_this_cycle = 0

        # calculate routes for this tick
        for row in range(height):
            for col in range(width):
                routers[row][col].route(sort_method, score_method)

        # generate flits for this tick
        for row in range(height):
            for col in range(width):
                src_router = routers[row][col]

                # with P=packets_per_tick, this PE decides it want's to
                # spawn a flit
                if random.random() <= packets_per_tick:
                    src_router.want_to_send += 1

                queued_this_cycle += src_router.want_to_send

                # if this PE does not want to send a flit right now, then
                # don't
                if src_router.want_to_send <= 0:
                    continue

                # check if this PE is currently back-pressured, and if not
                # send a flit
                if len(src_router.incoming_next) >= len(src_router.links):
                    backpressured_this_tick += 1
                    continue

                # pick a destination at random
                dest_row = row
                dest_col = col
                dest_router = None

                # don't route to ourselves
                while (dest_row == row) and (dest_col == col):
                    dest_row = random.randint(0, height - 1)
                    dest_col = random.randint(0, width - 1)
                    dest_router = routers[dest_row][dest_col]

                # we found a suitable router, add the packet to it
                p = nocsim.packet.Packet(src_router, dest_router, idnum, tick)
                idnum += 1
                spawned_this_tick += 1
                src_router.want_to_send -= 1
                queued_this_cycle -= 1
                src_router.add_incoming(p)
                packets.append(p)
                queued.append(queued_this_cycle)

        backpressured.append(backpressured_this_tick)
        spawned.append(spawned_this_tick)

        # flip over the simulation state for the next tick
        for row in range(height):
            for col in range(width):
                routers[row][col].flip()

    return packets, backpressured, spawned, queued


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
