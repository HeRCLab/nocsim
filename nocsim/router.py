import sys

class Router:

    def __init__(this, row, col, idnum):
        this.row           = row
        this.col           = col
        this.id            = idnum
        this.want_to_send  = 0
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
        if len(this.incoming) > len(this.links):
            sys.stderr.write("------ BAD ---\n")
            sys.stderr.write(str(this) + "\n")
            sys.stderr.write(str(this.incoming) + "\n")

        packets = sortmethod(this.incoming, this)

        if len(packets) != len(this.incoming):
            # this should never happen
            sys.stderr.write("------ BAD ---\n")
            sys.stderr.write("dropped packets! sort method is broken\n")
            sys.stderr.write(str(this.incoming) + "\n")
            sys.stderr.write(str(packets) + "\n")

        used = []
        deflected = False
        routed = 0
        for packet in packets:
            # packet has arrived!
            if packet.dest == this:
                packet.done = True
                continue

            for link in scoremethod(this.links, packet):
                if link in used:
                    deflected = True
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

