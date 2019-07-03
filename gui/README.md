# `nocsim-gui`

A simple GUI for interacting with `nocsim`. The `nocsim` code files are
symlinked into this directory, and the GUI binary is built independently from
the `nocsim` CLI binary.

# Building the GUI

`./configure`
`make`

Note that the `nocsim` GUI depends on [libAgar](http://libagar.org/) version
1.6.0 or better. At time of writing, this version of libAgar does not have a
stable release. SVN version r10268 is known to work correctly to build
`nocsim-gui`.

## Troubleshooting

### `./nocsim-gui: symbol lookup error: ./nocsim-gui: undefined symbol: ag_get_named_ptr`

libAgar is not in your `LD_LIBRARY_PATH`
