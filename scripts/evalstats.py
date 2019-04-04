import sys
import math
import statistics

title = None
cartesian = {}
tick = {}
injected = {}
tickno = -1
hops = {}
age = {}
arrived = {}
pending = 0
pending_at = {}
num_PE = -1
num_router = -1
elapsed_ms = -1
routed = {}
arrived_at = {}
pending_slope = {}
done = False

for line in ([f.strip() for f in l.split() if f.strip != ""] for l in sys.stdin):
    if line[0] == "config" and line[1] == "title":
        title = " ".join(line[2:])

    if line[0] == "meta" and line[1] == "num_PE":
        num_PE = int(line[2])

    if line[0] == "meta" and line[1] == "status" and line[2] == "done":
        done = True

    if line[0] == "meta" and line[1] == "num_router":
        num_router = int(line[2])

    elif line[0] == "inject":
        flitid  = int(line[1])
        fromrow = int(line[4])
        torow   = int(line[5])
        fromcol = int(line[8])
        tocol   = int(line[9])

        cartesian[flitid] = math.sqrt(
                math.pow(tocol - fromcol, 2) + math.pow(torow - fromrow, 2))

        tick[flitid] = tickno

        if tickno not in injected:
            injected[tickno] = 0

        injected[tickno] += 1

    elif line[0] == "route":
        flitid  = int(line[1])
        if flitid not in hops:
            hops[flitid] = 0
        if flitid not in age:
            age[flitid] = 0
        hops[flitid] += 1
        age[flitid] += 1
        if tickno not in routed:
            routed[tickno] = 0
        routed[tickno] += 1

    elif line[0] == "backrouted":
        # don't count flits that were backrouted towards throughput
        if tickno not in routed:
            routed[tickno] = 0
        routed[tickno] -= 1

    elif line[0] == "arrived":
        flitid  = int(line[1])
        arrived[flitid] = tickno
        if tickno not in arrived_at:
            arrived_at[tickno] = 0
        arrived_at[tickno] += 1

    elif line[0] == "tick":
        pending_at[tickno] = pending
        if (tickno > 1) :
            pending_slope[tickno] = pending_at[tickno] - pending_at[tickno - 1]
        tickno = int(line[1])

    elif line[0] == "push":
        pending += 1

    elif line[0] == "pop":
        pending -= 1
        flitid  = int(line[1])
        # handle time in flight vs age (backrouting)
        hops[flitid] = 0

    elif line[0] == "meta" and line[1] == "elapsed_ms":
        elapsed_ms = int(line[2])
        if (elapsed_ms == 0):
            elapsed_ms = 1

def crunchstat(vals, title):
    headers = []
    results = []
    for s in ["mean", "median", "stdev", "variance"]:
        headers.append("{} {}".format(s, title))
    for f in [statistics.mean, statistics.median, statistics.stdev, statistics.variance]:
        results.append(f(vals))

    return headers, results


headers = [
    "title",
    "total flits injected",
    "total flits arrived",
    "number of PEs",
    "total ticks",
    "elapsed ms",
    "Hz",
]

results = [
        title,
        len(tick),
        len(arrived),
        num_PE,
        tickno + 1,
        elapsed_ms,
        tickno / (elapsed_ms / 1000)
]

h1, r1 = crunchstat(
        [v/num_PE for v in injected.values()],
        "flits injected per tick per PE")
h2, r2 = crunchstat(hops.values(), "hops per flit")
h3, r3 = crunchstat(
        [v/num_PE for v in pending_at.values()],
        "pending flits per tick per PE")
h4, r4 = crunchstat(
        age.values(),
        "flit age")
h5, r5 = crunchstat(
        [r/num_PE for r in arrived_at.values()],
        "throughput")
h6, r6 = crunchstat(
        [v/num_PE for v in pending_slope.values()],
        "change in pending flits per tick per PE")

headers += h1 + h2 + h3 + h4 + h5 + h6
results += r1 + r2 + r3 + r4 + r5 + r6

assert done

sys.stderr.write("\t".join(headers))
sys.stderr.write("\n")
sys.stderr.flush()
sys.stdout.write("\t".join((str(i) for i in results)))
sys.stdout.write("\n")

