This directory contains various useful automation scripts. They are generally
commented as to their purpose.

# CI

CI scripts are invoked by executing `scripts/ci.sh`, which will execute all
scripts in `scripts/ci/*.sh`.

**NOTE** when adding a new CI script, it must ALSO be added to
`.circleci/config.yml`, as each individual script is configured as a separate
CircleCI job (so they can all run in parallel).

# Docker

CircleCI executes the automation scripts in an environment defined by
`scripts/Dockerfile`. This Dockerfile is published [on Docker
Hub](https://cloud.docker.com/u/herc/repository/docker/herc/nocsim_ci).

Any time the build environment is updated, you need to run `cd scripts &&
docker build . && docker push herc/nocsim_ci:latest`.

**HINT**: you may also need to run `docker tag ... herc/nocsim_ci` before
`docker push`. The source image ID will be given by the last line of `docker
build`, for example `Successfully built de7e6ad8ed35`.
