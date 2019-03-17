import math
import sys
import tqdm
import random

class Packet:

    def __init__(this, src, dest, idnum, created_tick):
        this.src       = src
        this.dest      = dest
        this.steps     = 0
        this.deflected = 0
        this.router    = src
        this.id        = idnum
        this.history   = []
        this.done      = False
        this.created_tick = created_tick

    def __str__(this):
        return "<Packet {}@{},{}: {},{} - ({}) -> {},{}>".format(
                this.id, this.router.row, this.router.col,
                this.src.row, this.src.col, this.steps, this.dest.row,
                this.dest.col)

    def __repr__(this):
        return this.__str__()

    def notify_route(this, link, deflected):
        this.steps += 1
        if deflected:
            this.deflected += 1
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
