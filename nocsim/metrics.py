def throughput(routers, packets, packets_per_tick, duration):
    """throughput

    Compute the throughput in flit-routers per cycle with respect to
    injection rate in flit-routers per cycle.

    :param routers:
    :param packets:
    :param packets_per_tick: range [lower, upper] of flits to inject per tick
    :param duration: number of cycles for which the simulation was run
    """

    num_routers = len(routers) * len(routers[0])
    num_flits = len(packets)

    routed_packets = [p for p in packets if p.done]

    the_throughput = (len(routed_packets) / duration) / num_routers
    the_injection_rate = (packets_per_tick)

    return (the_throughput, the_injection_rate)

def latency(routers, packets, packets_per_tick, duration):
    """latency

    Compute the average latency in cycles with respect to throughput in
    flit-routers per cycle

    :param routers:
    :param packets:
    :param packets_per_tick:
    :param duration:
    """

    num_routers = len(routers) * len(routers[0])
    num_flits = len(packets)

    routed_packets = [p for p in packets if p.done]

    the_throughput = (len(routed_packets) / duration) / num_routers

    the_latency = sum([len(p.history) for p in routed_packets]) / len(routed_packets)

    return (the_latency, the_throughput)

def latency_worst(routers, packets, packets_per_tick, duration):
    num_routers = len(routers) * len(routers[0])
    num_flits = len(packets)

    routed_packets = [p for p in packets if p.done]

    worst = 0
    for p in routed_packets:
            if len(p.history) > worst:
                worst = len(p.history)

    return worst

def throughput_fpc(routers, packets, packets_per_tick, duration):
    """throughput_fpc

    Compute the throughput in flits per cycle with respect to routers.

    :param routers:
    :param packets:
    :param packets_per_tick:
    :param duration:
    """

    num_routers = len(routers) * len(routers[0])
    num_flits = len(packets)

    routed_packets = [p for p in packets if p.done]

    return (len(routed_packets) / duration, num_routers)

