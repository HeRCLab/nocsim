# nocviz TCL Scripting Manual

This document describes the nocviz TCL API.

Portions of commands in `lowercase` should be copied exactly, while portions of
commands in `UPPERCASE` should be replaced with a relevant value.

## General Procedures

### `op register OPID SCRIPT DESCRIPTION`

Register an operation which does not apply to a specific node. When the user
executes it, the specified `SCRIPT` will be executed as a TCL command. `OPID`
should be a unique for the given operation.  `DESCRIPTION` is a string which
should describe to the user the nature of the operation to be performed.

### `op unregister OPID`

Unregister an operation so it is no longer shown to the user.

### `data set KEY VAL`

`nocviz` includes a general key-value-pair store which is not associated with
any particular link or node which stores information that is presented to the
user in the graphical interface.

This method sets a given key to a given value, overwriting any existing value
if the key is already defined.

### `data get KEY`

Retrieve a value from the general key value pair store.

### `data fmt KEY FMT`

Provide a [TCL format string](https://www.tcl.tk/man/tcl8.4/TclCmd/format.htm)
describing how the value should be displayed. If not specified, the default
value is `%s`. Any existing format string is silently overwritten.

### `data keys`

Return a list of all keys in the general key value pair store.

### `data show SECTION KEY1 ... KEYn`

Declare that a list of keys should be presented to the user. Until this method
is called, the key value pairs will be hidden.

`SECTION` is a string which is used to group collections of keys. Keys grouped
within the same section will be displayed adjacently (and in the order they
were specified as arguments to this procedure). The value provided for
`SECTION` will be shown as a header.

This method may be called again to change which keys are shown in a given
section. A section which contains no keys will not be displayed.

If any of the keys given do not exist, an exception will be thrown.

The user interface will automatically poll the values stored in the key value
pair store and display updated values to the user as they are changed.

### `data delete KEY`

Deletes the specified key from the general key value pair store.

## Node Procedures

Each node represents something in the network, such as a router. Nodes are
connected by links. Each node has a row and column, used to determine it's
position, and an optional human-readable title. Nodes are identified by their
ID.

Each node also includes a key-value pair store which will be used to show
information to the user about the node. Any keys or values may be used to show
relevant information to the user.

### `node create ID`

Creates a node with the specified `ID`, if it does not already exist.

The `ID` value should uniquely identify the specified node as distinct from all
other nodes.

Node IDs should contain at least one character, and should only contain only
printable characters.

Throws an exception if a node with that ID exists already.

### `node destroy ID`

Destroy the specified node. This also destroys the node's key-value pair store,
as well as any links which have this node as an endpoint.

### `node row ID ROW` / `node col ID COL`

Specifies either the node's row or column respectively. Nodes will be created
with at row 0 column 0 by default.

The row and column values are used for layout in certain cases.

### `node title ID TITLE`

Specifies the node's title, which is an arbitrary, possibly non-unique string
that is shown to the user. If no title is specified, then the node's ID is used
instead.

### `node data set ID KEY VAL`

As with `data set`, but applies to the node's internal KVP store.

### `node data get ID KEY`

As with `data get`, but applies to the node's internal KVP store.

### `node data fmt ID KEY FMT`

As with `data fmt`, but applies to the node's internal KVP store.

### `node data keys ID`

As with `data keys`, but applies to the node's internal KVP store.

### `node data show ID SECTION KEY1 ... KEYn`

As with `data show`, but applies to the node's internal KVP store.

### `node data delete ID KEY`

As with `data delete`, but applies to the node's internal KVP store.

### `node list`

Returns a list of all node IDs instantiated with `node create` so far.

### `node match KEY PATTERN`

Return a list of all node IDs instantiate with `node create` so far which have
the specified key, and where the key's value matches the supplied regex
(`PATTERN`). TCL regular expression syntax should be used to define the
pattern.

### `node op register ID OPID SCRIPT DESCRIPTION`

Register a node operation, which the user will be able to perform on the node.
`ID` specifies the node to which the operation applies, `OPID` should be a
unique identifier, `SCRIPT` should be a TCL script which defines the operation
to be performed, and `DESCRIPTION` should be a string describing the operation
to the user.

### `node op unregister ID OPID`

Remove a previously defined node operation.

### `node op id`

When executed within the context of a node operation, this procedure will
return the ID of the node being operated on. Otherwise, it will raise an
exception.

## Link Procedures

Each link represents a connection between two nodes. Links may be either
directed or undirected. Links are defined by their endpoints. Like nodes, links
also contain a key value pair store of information which will be presented to
the user.

### `link create SRC_ID DEST_ID`

Creates a link between the specified nodes. The link will be directed by
default. Throws an exception if a link with the given endpoints already exists.

**NOTE:** for the purpose of determining link existence, the ordering of
`SRC_ID` and `DEST_ID` is ignored. For example:

```tcl
link create foo bar
link create bar foo
```

Would cause an exception to be thrown.

### `link directed ID1 ID2`

Sets the link between `ID1` and `ID2` to be a directed link.

### `link undirected ID1 ID2`

Sets the link between `ID1` and `ID2` to be an undirected link.

### `link destroy ID1 ID2`

Destroy the link between the given nodes.

### `link reverse ID1 ID2`

Reverses the direction the link is pointing.

### `link direction ID1 ID2`

Returns a list of `SRC_ID` and `DEST_ID` for the given link, in that order.

### `link find ID`

Return a list of all links where either end is a node with the given ID.

### `link find ID1 ID2`

Return a list of all links which have endpoints at both specified IDs. This
list will always contain either 0 or 1 elements.

### `link match KEY PATTERN`

Return a list of all links whose key value pair stores contain the key `KEY`
and where the corresponding value matches the TCL syntax regular expression
`PATTERN`.


### `link data set ID1 ID2 KEY VAL`

As with `data set`, but applies to the link's internal KVP store.

### `link data get ID1 ID2 KEY`

As with `data get`, but applies to the link's internal KVP store.

### `link data fmt ID1 ID2 KEY FMT`

As with `data fmt`, but applies to the link's internal KVP store.

### `link data keys ID1 ID2`

As with `data keys`, but applies to the link's internal KVP store.

### `link data show ID1 ID2 SECTION KEY1 ... KEYn`

As with `data show`, but applies to the link's internal KVP store.

### `link data delete ID1 ID2 KEY`

As with `data delete`, but applies to the link's internal KVP store.

### `link op register ID1 ID2 OPID SCRIPT DESCRIPTION`

Register a link operation, which the user will be able to perform on the link.
`ID1`, `ID2` specifies the link to which the operation applies, `OPID` should
be a unique identifier, `SCRIPT` should be a TCL script which defines the
operation to be performed, and `DESCRIPTION` should be a string describing the
operation to the user.

### `link op unregister ID1 ID2 OPID`

Remove a previously defined node operation.

### `link op id`

When executed within the context of a link operation, this procedure will
return a list of the two endpoint nodes of the link being operated on.
Otherwise, it will raise an exception.
