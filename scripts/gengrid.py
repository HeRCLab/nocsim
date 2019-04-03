import sys
import argparse

parser = argparse.ArgumentParser()

parser.add_argument("--size", "-s", type=int, required=True)
parser.add_argument("--topography", "-t", choices=["mesh", "torus"], required=True)
parser.add_argument("--title", "-T", default=None)
parser.add_argument("--P_inject", "-P", type=float, required=True)
parser.add_argument("--seed", "-S", type=int, required=True)
parser.add_argument("--behavior", "-b", choices=["DOR"], required=True)
parser.add_argument("--output", "-o", default=sys.stdout)
parser.add_argument("--ticks", "-k", type=int, required=True)

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

