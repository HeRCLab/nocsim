import sys
import math
import statistics

title = None
cartesian = {}
tick = {}
injected = {}
tickno = -1
hops = {}
arrived = {}
pending = 0
pending_at = {}
num_PE = -1

for line in ([f.strip() for f in l.split() if f.strip != ""] for l in sys.stdin):
    if line[0] == "config" and line[1] == "title":
        title = " ".join(line[2:])

    if line[0] == "meta" and line[1] == "num_PE":
        num_PE = int(line[2])

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
        hops[flitid] += 1

    elif line[0] == "arrived":
        flitid  = int(line[1])
        arrived[flitid] = tickno

    elif line[0] == "tick":
        pending_at[tickno] = pending
        pending = 0
        tickno = int(line[1])

    elif line[0] == "push":
        pending += 1

    elif line[0] == "pop":
        pending += 1

def crunchstat(vals, title):
    headers = []
    results = []
    for s in ["mean", "median", "mode", "stdev", "variance"]:
        headers.append("{} {}".format(s, title))
    for f in [statistics.mean, statistics.median, statistics.mode, statistics.stdev, statistics.variance]:
        results.append(f(vals))

    return headers, results


headers = [
    "title",
    "total flits injected",
    "total flits arrived",
    "number of PEs",
    "total ticks",
]

results = [
        title,
        len(tick),
        len(arrived),
        num_PE,
        tickno + 1,
]

h1, r1 = crunchstat(
        [v/num_PE for v in injected.values()],
        "flits injected per tick per PE")
h2, r2 = crunchstat(hops.values(), "hops per flit")
h3, r3 = crunchstat(
        [v/num_PE for v in pending_at.values()],
        "pending flits per tick per PE")

headers += h1 + h2 + h3
results += r1 + r2 + r3

sys.stderr.write("\t".join(headers))
sys.stderr.write("\n")
sys.stderr.flush()
sys.stdout.write("\t".join((str(i) for i in results)))
sys.stdout.write("\n")

