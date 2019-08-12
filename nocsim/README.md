# here there be dragons...

The noc-tools project originally started as "nocsim", a pure C based simulator
that used a custom DSL to describe the desired simulation run. Over time, it
was retro-fitted with TCL support, and later the project grew to support
visualization capabilities (nocviz), and to load nocsim as a library, rather
than as a top-level program.

This resulted in a lot of growing pain. Many parts of nocsim may appear oddly
designed, or have misleading names. This is usually because those components
were written on a previous iteration of the project, and have not been updated
yet.

Here is a rough breakdown of the nocsim project structure:

* `nocsim.c` used to contain `main()`, and now contains the TCL library
  initializer
* `interp.c` used to be the implementation of the TCL interpreter, and now
  holds the implementation for all of nocsim's commands.
* `simulation.c` contains the implementation of the simulation logic.
* `util.c` contains utility methods, including accessors for simulation data
  structures.
* `grid.c` contains methods relating to the management of routers, nodes, and
  links.
