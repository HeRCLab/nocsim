# noc-tools

[![CircleCI](https://circleci.com/gh/HeRCLab/nocsim.svg?style=svg)](https://circleci.com/gh/HeRCLab/nocsim)

noc-tools is a collection of network-on-chip oriented tools written by the
University of South Carolina Heterogeneous and Reconfigurable Computing
research group.

* **nocsim** is a software simulator for network on chip implementations
* **nocviz** is a visualization tool for NoCs using a simple TCL driven API to
  allow it to be used to visualize or control a variety of NoCs, including
  nocsim

The noc-tools user manual may be accessed [here](doc/noc-tools.md).

## Building noc-tools

noc-tools depends on the following libraries:

* libbsd
* libtcl
* [libAgar](http://libagar.org/)

On Ubuntu 18.04, run sudo apt install --yes libbsd-dev libreadline-dev tcl-dev libtcl8.6

noc-tools is built against the development version of libAgar 1.6.0. At
present, [SVN revision
10556](https://github.com/JulNadeauCA/libagar/commit/b29a63d7e9e7cda25f2126df08bd152384b65d40)
is suggested.

Once all dependencies have been installed, simply run `./configure && make`. On
Ubuntu 18.04, you will need to provide the path to your TCL installation using
the `--with-tcl=/usr/include/tcl8.6/` parameter.

## Installation

noc-tools does not prescribe any specific installation method. It is generally
used by `source`-ing `scripts/noc_tools_load.tcl`. To that end, it is important
that the `nocsim/` and `nocviz/` directories remain in the same relative paths
to said script.

## Development Procedures

### `git`

* Feature branches should be used
	* First create an issue describing the work to be done.
	* Next create a branch in which to work on the issue.
	* When the work is complete, open a pull request and have at least
	  one other person check over your changes.
	* Merge the pull request into `dev` using a squash merge.
* Avoid rewriting history on pushed branches (no rebase).
* All development happens in `dev`, master should never be broken.
	* Only merge working code into `master`.

### New Release

1. Run `git tag RELEASE_NUMBER` on the commit to release in the `dev` branch.
	* For example, `git tag 2.0.0`.
2. Run `git checkout master ; git merge dev ; git push --tags origin master ;
   git checkout dev ; git push --tags origin dev`
	* This will merge `dev` into `master`, and push the tagged commit
	  to both `dev` and `master`.
3. Update `configure.in` with the new version number/
4. Re-generate `./configure`.
5. Create a new section in `CHANGELOG.md`.
6. Push the update `configure.in`, `configure`, and `CHANGELOG.md` to `dev`
   (but not `master`) as a new commit.

### Re-Generating `./configure`

noc-tools uses the [BSDBuild](http://bsdbuild.hypertriton.com/) build system.
You should be able to use the provided configure script without any
modifications. However, noc-tools developers may need to re-generate this
script from time to time, such as when new configuration flags are added.

The script may be re-generated using `mkconfigure < configure.in > configure`.
`configure.in` is used to specify what flags and options should be presented.
The documentation for `mkconfigure` may be found
[here](http://bsdbuild.hypertriton.com//mdoc.cgi?man=mkconfigure.1).

**NOTE** You should not use `mkify` unless you know what you are doing, as it
will overwrite specially modified make libraries used by noc-tools with the
BSDBuild default versions.

### Updating the `Dockerfile`

This procedure describes updating the [DockerHub copy of the nocsim_CI
environment](https://cloud.docker.com/u/herc/repository/docker/herc/nocsim_ci).

From time to time (say when dependencies change), one needs to update
`scripts/Dockerfile` and have the change propagated into Docker Hub.

1. cd `scripts`
2. `docker build .`
3. `docker tag IMAGEID herc/nocsim_ci`
	* This will be displayed on the last line of `docker build`, for
	  example `Successfully built de7e6ad8ed35`.
4. `docker push herc/nocsim_ci:latest`

Note that you will need a Docker Hub account. Someone with `owner` rights on
the `herc` organization will need to add you to the organization.
