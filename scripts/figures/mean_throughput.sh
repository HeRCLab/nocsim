#!/bin/sh

injrate="$(awk -f scripts/getheaders.awk < out/results.tsv | grep "mean flits injected per tick per PE" | cut -f2)"
throughput="$(awk -f scripts/getheaders.awk < out/results.tsv | grep "mean throughput" | cut -f2)"

gnuplot \
	-e "$GP_TERMINAL" \
	-e 'set datafile separator "\t"' \
	-e 'set key outside below' \
	-e 'set title "Mean Throughput vs Injection Rate"' \
	-e 'set xlabel "injection rate (flit-PEs per cycle)"' \
	-e 'set ylabel "throughput (flits per PE per cycle)"' \
	-e "plot 'out/results_mesh_DOR.tsv' using $injrate:$throughput with linespoints title \"mesh, DOR\", \
'out/results_torus_DOR.tsv' using $injrate:$throughput with linespoints title \"torus, DOR\", \
'out/results_utorus_DOR.tsv' using $injrate:$throughput with linespoints title \"undirected torus, DOR\", \
'out/results_mesh_ADOR.tsv' using $injrate:$throughput with linespoints title \"mesh, ADOR\", \
'out/results_utorus_ADOR.tsv' using $injrate:$throughput with linespoints title \"undirected torus, ADOR\"" \
> out/mean_throughput.$GP_EXT
