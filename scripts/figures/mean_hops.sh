#!/bin/sh

injrate="$(awk -f scripts/getheaders.awk < out/results.tsv | grep "mean flits injected per tick per PE" | cut -f2)"
hops="$(awk -f scripts/getheaders.awk < out/results.tsv | grep "mean hops per flit" | cut -f2)"

gnuplot \
	-e "$GP_TERMINAL" \
	-e 'set datafile separator "\t"' \
	-e 'set key outside below' \
	-e 'set title "Mean Hops Per Flit vs Injection Rate"' \
	-e 'set xlabel "injection rate (flit-PEs per cycle)"' \
	-e 'set ylabel "mean hops (hops per flit)"' \
	-e "plot 'out/results_mesh_DOR.tsv' using $injrate:$hops with linespoints title \"mesh, DOR\", \
'out/results_torus_DOR.tsv' using $injrate:$hops with linespoints title \"torus, DOR\"" \
> out/mean.$GP_EXT
