import nocsim.router
import tqdm
import nocsim.packet
import random
import math
import colormap
import json

def generate_mesh(width, height):
    """generate_mesh

    Generates a non-directional mesh configuration.

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
        dist = math.sqrt(
                math.pow(packet.dest.col - link.col, 2) +
                math.pow(packet.dest.row - link.row, 2)
            )
        scored.append((dist, link))

    scored.sort(key=lambda x: x[0])

    return (x[1] for x in scored)


def simulate(routers, packets_per_tick=[1,5], run_for=50000):
    packets = []

    height = len(routers)
    width = len(routers[0])

    # simulate
    for tick in tqdm.tqdm(range(run_for)):

        for row in range(height):
            for col in range(width):
                routers[row][col].route(sort_method_none,
                        score_method_cartesian)

        for i in range(random.randrange(*packets_per_tick)):
            row1 = random.randint(0, height - 1)
            col1 = random.randint(0, width - 1)
            row2 = random.randint(0, height - 1)
            col2 = random.randint(0, width - 1)
            src = routers[row1][col1]

            while len(src.incoming_next) >= len(src.links):
                row1 = random.randint(0, height - 1)
                col1 = random.randint(0, width - 1)
                src = routers[row1][col1]


            p = nocsim.packet.Packet(src, routers[row2][col2], tick, tick)
            routers[row1][col1].add_incoming(p)
            packets.append(p)

        for row in range(height):
            for col in range(width):
                routers[row][col].flip()

    return packets


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
