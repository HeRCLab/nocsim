#!/usr/bin/env python3

import os
import sys
import json

sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), "../"))

import nocsim
import nocsim.simulation

routers = nocsim.simulation.generate_mesh(10, 10)
packets = nocsim.simulation.simulate(routers)

with open("routers.json", "w") as f:
    json.dump(nocsim.simulation.dump_routers(routers), f)

with open("packets.json", "w") as f:
    json.dump(nocsim.simulation.dump_packets(packets), f)
