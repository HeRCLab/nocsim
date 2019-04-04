#!/bin/sh

injrate="$(awk -f scripts/getheaders.awk < out/results.tsv | grep "median flits injected per tick per PE" | cut -f2)"
throughput="$(awk -f scripts/getheaders.awk < out/results.tsv | grep "median throughput" | cut -f2)"

gnuplot \
	-e "$GP_TERMINAL" \
	-e 'set datafile separator "\t"' \
	-e 'set key outside below' \
	-e 'set title "Median Throughput vs Injection Rate"' \
	-e 'set xlabel "injection rate (flit-PEs per cycle)"' \
	-e 'set ylabel "throughput (flits per PE per cycle)"' \
	-e "plot 'out/results_mesh_DOR.tsv' using $injrate:$throughput with linespoints title \"mesh, DOR\", \
'out/results_torus_DOR.tsv' using $injrate:$throughput with linespoints title \"torus, DOR\"" \
> out/median_throughput.$GP_EXT
