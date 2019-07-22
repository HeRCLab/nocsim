#!/bin/sh

set -u
set -e

echo '#### LINT #####################################################################'
make DEVELOP=true clean
make DEVELOP=true lint

# 1 line header
if [ "$(ikos-report --status-filter error -f csv src/nocsim.db 2>&1 | wc -l)" -gt 1 ] ; then
	exit 1
fi
