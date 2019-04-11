#!/bin/sh

injrate="$(awk -f scripts/getheaders.awk < out/results.tsv | grep "mean flits injected per tick per PE" | cut -f2)"
age="$(awk -f scripts/getheaders.awk < out/results.tsv | grep "mean flit age" | cut -f2)"

gnuplot \
	-e "$GP_TERMINAL" \
	-e 'set datafile separator "\t"' \
	-e 'set key outside below' \
	-e 'set title "Mean Flit Age at Arrival vs Injection Rate"' \
	-e 'set xlabel "injection rate (flit-PEs per cycle)"' \
	-e 'set ylabel "age (cycles)"' \
	-e "plot 'out/results_mesh_DOR.tsv' using $injrate:$age with linespoints title \"mesh, DOR\", \
'out/results_torus_DOR.tsv' using $injrate:$age with linespoints title \"torus, DOR\", \
'out/results_utorus_DOR.tsv' using $injrate:$age with linespoints title \"udirected torus, DOR\", \
'out/results_mesh_ADOR.tsv' using $injrate:$age with linespoints title \"mesh, ADOR\", \
'out/results_torus_ADOR.tsv' using $injrate:$age with linespoints title \"torus, ADOR\", \
'out/results_utorus_ADOR.tsv' using $injrate:$age with linespoints title \"undirected torus, ADOR\"" \
> out/mean_age.$GP_EXT
