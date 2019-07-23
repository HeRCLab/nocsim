#!/bin/sh

set -u
set -e

. scripts/util.lib

echo '#### LINT #####################################################################'
tail_on_error make DEVELOP=true clean
tail_on_error make DEVELOP=true lint

# 1 line header
if [ "$(ikos-report --status-filter error -f csv src/nocsim.db 2>&1 | wc -l)" -gt 1 ] ; then
	ikos-report --status-filter error src/nocsim.db
	exit 1
fi
