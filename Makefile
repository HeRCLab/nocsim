.PHONY: clean all all_figures

ticks=1000

all: data/DOR_mesh.tsv data/cartesian_mesh.tsv all_figures

all_figures: figures/throughput.pdf figures/latency.pdf figures/latency_worst.pdf figures/congestion.pdf figures/inflation.pdf

figures/throughput.pdf: data/throughput.csv
	mkdir -p figures
	gnuplot \
		-e 'set terminal pdf' \
		-e 'set datafile separator ","' \
		-e 'set key outside below' \
		-e 'set title "Throughput"' \
		-e 'set xlabel "injection rate (flit-routers per cycle)"' \
		-e 'set ylabel "throughput (flit-routers per cycle)"' \
		-e 'plot "data/throughput.csv" using 1:2 with linespoints title "mesh (cartesian scoring)", "data/throughput.csv" using 3:4 with linespoints title "torus (cartesian scoring)", "data/throughput.csv" using 5:6 with linespoints title "mesh (DOR scoring)", "data/throughput.csv" using 7:8 with linespoints title "torus (DOR scoring)"' \
		> $@.tmp
	mv $@.tmp $@

# cartesian mesh, cartesian torus, DOR mesh, DOR torus
data/throughput.csv: data/throughput_cartesian.csv data/throughput_DOR.csv
	paste -d, data/throughput_cartesian.csv data/throughput_DOR.csv > $@

# cartesian mesh, cartesian torus
data/throughput_cartesian.csv: data/throughput_cartesian_mesh.csv data/throughput_cartesian_torus.csv
	paste -d, data/throughput_cartesian_mesh.csv data/throughput_cartesian_torus.csv > $@

# DOR mesh, DOR torus
data/throughput_DOR.csv: data/throughput_DOR_mesh.csv data/throughput_DOR_torus.csv
	paste -d, data/throughput_DOR_mesh.csv data/throughput_DOR_torus.csv > $@

data/throughput_cartesian_mesh.csv : data/cartesian_mesh.tsv
	xsv select -d '\t' "injection rate","throughput" < data/cartesian_mesh.tsv | \
		sed '1d' > $@

data/throughput_DOR_mesh.csv: data/DOR_mesh.tsv
	xsv select -d '\t' "injection rate","throughput" < data/DOR_mesh.tsv | \
		sed '1d' > $@

data/throughput_cartesian_torus.csv : data/cartesian_torus.tsv
	xsv select -d '\t' "injection rate","throughput" < data/cartesian_torus.tsv | \
		sed '1d' > $@

data/throughput_DOR_torus.csv: data/DOR_torus.tsv
	xsv select -d '\t' "injection rate","throughput" < data/DOR_torus.tsv | \
		sed '1d' > $@

figures/latency.pdf: data/latency.csv
	mkdir -p figures
	gnuplot \
		-e 'set terminal pdf' \
		-e 'set datafile separator ","' \
		-e 'set key outside below' \
		-e 'set title "latency"' \
		-e 'set xlabel "injection rate (flit-routers per cycle)"' \
		-e 'set ylabel "average latency (cycles)"' \
		-e 'plot "data/latency.csv" using 1:2 with linespoints title "mesh (cartesian scoring)", "data/latency.csv" using 3:4 with linespoints title "torus (cartesian scoring)", "data/latency.csv" using 5:6 with linespoints title "mesh (DOR scoring)", "data/latency.csv" using 7:8 with linespoints title "torus (DOR scoring)"' \
		> $@.tmp
	mv $@.tmp $@

# cartesian mesh, cartesian torus, DOR mesh, DOR torus
data/latency.csv: data/latency_cartesian.csv data/latency_DOR.csv
	paste -d, data/latency_cartesian.csv data/latency_DOR.csv > $@

# cartesian mesh, cartesian torus
data/latency_cartesian.csv: data/latency_cartesian_mesh.csv data/latency_cartesian_torus.csv
	paste -d, data/latency_cartesian_mesh.csv data/latency_cartesian_torus.csv > $@

# DOR mesh, DOR torus
data/latency_DOR.csv: data/latency_DOR_mesh.csv data/latency_DOR_torus.csv
	paste -d, data/latency_DOR_mesh.csv data/latency_DOR_torus.csv > $@

data/latency_cartesian_mesh.csv : data/cartesian_mesh.tsv
	xsv select -d '\t' "injection rate","latency" < data/cartesian_mesh.tsv | \
		sed '1d' > $@

data/latency_DOR_mesh.csv: data/DOR_mesh.tsv
	xsv select -d '\t' "injection rate","latency" < data/DOR_mesh.tsv | \
		sed '1d' > $@

data/latency_cartesian_torus.csv : data/cartesian_torus.tsv
	xsv select -d '\t' "injection rate","latency" < data/cartesian_torus.tsv | \
		sed '1d' > $@

data/latency_DOR_torus.csv: data/DOR_torus.tsv
	xsv select -d '\t' "injection rate","latency" < data/DOR_torus.tsv | \
		sed '1d' > $@

figures/latency_worst.pdf: data/latency_worst.csv
	mkdir -p figures
	gnuplot \
		-e 'set terminal pdf' \
		-e 'set datafile separator ","' \
		-e 'set key outside below' \
		-e 'set title "worst-case latency"' \
		-e 'set xlabel "injection rate (flit-routers per cycle)"' \
		-e 'set ylabel "worst-case latency (cycles)"' \
		-e 'plot "data/latency_worst.csv" using 1:2 with linespoints title "mesh (cartesian scoring)", "data/latency_worst.csv" using 3:4 with linespoints title "torus (cartesian scoring)", "data/latency_worst.csv" using 5:6 with linespoints title "mesh (DOR scoring)", "data/latency_worst.csv" using 7:8 with linespoints title "torus (DOR scoring)"' \
		> $@.tmp
	mv $@.tmp $@

# cartesian mesh, cartesian torus, DOR mesh, DOR torus
data/latency_worst.csv: data/latency_worst_cartesian.csv data/latency_worst_DOR.csv
	paste -d, data/latency_worst_cartesian.csv data/latency_worst_DOR.csv > $@

# cartesian mesh, cartesian torus
data/latency_worst_cartesian.csv: data/latency_worst_cartesian_mesh.csv data/latency_worst_cartesian_torus.csv
	paste -d, data/latency_worst_cartesian_mesh.csv data/latency_worst_cartesian_torus.csv > $@

# DOR mesh, DOR torus
data/latency_worst_DOR.csv: data/latency_worst_DOR_mesh.csv data/latency_worst_DOR_torus.csv
	paste -d, data/latency_worst_DOR_mesh.csv data/latency_worst_DOR_torus.csv > $@

data/latency_worst_cartesian_mesh.csv : data/cartesian_mesh.tsv
	xsv select -d '\t' "injection rate","worst case latency" < data/cartesian_mesh.tsv | \
		sed '1d' > $@

data/latency_worst_DOR_mesh.csv: data/DOR_mesh.tsv
	xsv select -d '\t' "injection rate","worst case latency" < data/DOR_mesh.tsv | \
		sed '1d' > $@

data/latency_worst_cartesian_torus.csv : data/cartesian_torus.tsv
	xsv select -d '\t' "injection rate","worst case latency" < data/cartesian_torus.tsv | \
		sed '1d' > $@

data/latency_worst_DOR_torus.csv: data/DOR_torus.tsv
	xsv select -d '\t' "injection rate","worst case latency" < data/DOR_torus.tsv | \
		sed '1d' > $@

figures/congestion.pdf: data/congestion.csv
	mkdir -p figures
	gnuplot \
		-e 'set terminal pdf' \
		-e 'set datafile separator ","' \
		-e 'set key outside below' \
		-e 'set title "congestion"' \
		-e 'set logscale y' \
		-e 'set xlabel "injection rate (flit-routers per cycle)"' \
		-e 'set ylabel "congestion score"' \
		-e 'plot "data/congestion.csv" using 1:2 with linespoints title "mesh (cartesian scoring)", "data/congestion.csv" using 3:4 with linespoints title "torus (cartesian scoring)", "data/congestion.csv" using 5:6 with linespoints title "mesh (DOR scoring)", "data/congestion.csv" using 7:8 with linespoints title "torus (DOR scoring)"' \
		> $@.tmp
	mv $@.tmp $@

# cartesian mesh, cartesian torus, DOR mesh, DOR torus
data/congestion.csv: data/congestion_cartesian.csv data/congestion_DOR.csv
	paste -d, data/congestion_cartesian.csv data/congestion_DOR.csv > $@

# cartesian mesh, cartesian torus
data/congestion_cartesian.csv: data/congestion_cartesian_mesh.csv data/congestion_cartesian_torus.csv
	paste -d, data/congestion_cartesian_mesh.csv data/congestion_cartesian_torus.csv > $@

# DOR mesh, DOR torus
data/congestion_DOR.csv: data/congestion_DOR_mesh.csv data/congestion_DOR_torus.csv
	paste -d, data/congestion_DOR_mesh.csv data/congestion_DOR_torus.csv > $@

data/congestion_cartesian_mesh.csv : data/cartesian_mesh.tsv
	xsv select -d '\t' "injection rate","congestion" < data/cartesian_mesh.tsv | \
		sed '1d' > $@

data/congestion_DOR_mesh.csv: data/DOR_mesh.tsv
	xsv select -d '\t' "injection rate","congestion" < data/DOR_mesh.tsv | \
		sed '1d' > $@

data/congestion_cartesian_torus.csv : data/cartesian_torus.tsv
	xsv select -d '\t' "injection rate","congestion" < data/cartesian_torus.tsv | \
		sed '1d' > $@

data/congestion_DOR_torus.csv: data/DOR_torus.tsv
	xsv select -d '\t' "injection rate","congestion" < data/DOR_torus.tsv | \
		sed '1d' > $@


figures/inflation.pdf: data/inflation.csv
	mkdir -p figures
	gnuplot \
		-e 'set terminal pdf' \
		-e 'set datafile separator ","' \
		-e 'set key outside below' \
		-e 'set title "inflation"' \
		-e 'set xlabel "injection rate (flit-routers per cycle)"' \
		-e 'set ylabel "inflation (mean # hops / minimal path)"' \
		-e 'plot "data/inflation.csv" using 1:2 with linespoints title "mesh (cartesian scoring)", "data/inflation.csv" using 3:4 with linespoints title "torus (cartesian scoring)", "data/inflation.csv" using 5:6 with linespoints title "mesh (DOR scoring)", "data/inflation.csv" using 7:8 with linespoints title "torus (DOR scoring)"' \
		> $@.tmp
	mv $@.tmp $@

# cartesian mesh, cartesian torus, DOR mesh, DOR torus
data/inflation.csv: data/inflation_cartesian.csv data/inflation_DOR.csv
	paste -d, data/inflation_cartesian.csv data/inflation_DOR.csv > $@

# cartesian mesh, cartesian torus
data/inflation_cartesian.csv: data/inflation_cartesian_mesh.csv data/inflation_cartesian_torus.csv
	paste -d, data/inflation_cartesian_mesh.csv data/inflation_cartesian_torus.csv > $@

# DOR mesh, DOR torus
data/inflation_DOR.csv: data/inflation_DOR_mesh.csv data/inflation_DOR_torus.csv
	paste -d, data/inflation_DOR_mesh.csv data/inflation_DOR_torus.csv > $@

data/inflation_cartesian_mesh.csv : data/cartesian_mesh.tsv
	xsv select -d '\t' "injection rate","mean inflation" < data/cartesian_mesh.tsv | \
		sed '1d' > $@

data/inflation_DOR_mesh.csv: data/DOR_mesh.tsv
	xsv select -d '\t' "injection rate","mean inflation" < data/DOR_mesh.tsv | \
		sed '1d' > $@

data/inflation_cartesian_torus.csv : data/cartesian_torus.tsv
	xsv select -d '\t' "injection rate","mean inflation" < data/cartesian_torus.tsv | \
		sed '1d' > $@

data/inflation_DOR_torus.csv: data/DOR_torus.tsv
	xsv select -d '\t' "injection rate","mean inflation" < data/DOR_torus.tsv | \
		sed '1d' > $@



data/DOR_mesh.tsv: nocsim/cli.py
	mkdir -p data
	python3 nocsim/cli.py -t mesh -T $(ticks) -S DOR -p 0.1 2>/dev/null | tail -n 2 > $@
	python3 nocsim/cli.py -t mesh -T $(ticks) -S DOR -p 0.2 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t mesh -T $(ticks) -S DOR -p 0.3 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t mesh -T $(ticks) -S DOR -p 0.4 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t mesh -T $(ticks) -S DOR -p 0.5 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t mesh -T $(ticks) -S DOR -p 0.6 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t mesh -T $(ticks) -S DOR -p 0.7 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t mesh -T $(ticks) -S DOR -p 0.8 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t mesh -T $(ticks) -S DOR -p 0.9 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t mesh -T $(ticks) -S DOR -p 1.0 2>/dev/null | tail -n 1 >> $@

data/cartesian_mesh.tsv: nocsim/cli.py
	mkdir -p data
	python3 nocsim/cli.py -t mesh -T $(ticks) -S cartesian -p 0.1 2>/dev/null | tail -n 2 > $@
	python3 nocsim/cli.py -t mesh -T $(ticks) -S cartesian -p 0.2 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t mesh -T $(ticks) -S cartesian -p 0.3 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t mesh -T $(ticks) -S cartesian -p 0.4 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t mesh -T $(ticks) -S cartesian -p 0.5 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t mesh -T $(ticks) -S cartesian -p 0.6 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t mesh -T $(ticks) -S cartesian -p 0.7 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t mesh -T $(ticks) -S cartesian -p 0.8 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t mesh -T $(ticks) -S cartesian -p 0.9 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t mesh -T $(ticks) -S cartesian -p 1.0 2>/dev/null | tail -n 1 >> $@

data/DOR_torus.tsv: nocsim/cli.py
	mkdir -p data
	python3 nocsim/cli.py -t torus -T $(ticks) -S DOR -p 0.1 2>/dev/null | tail -n 2 > $@
	python3 nocsim/cli.py -t torus -T $(ticks) -S DOR -p 0.2 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t torus -T $(ticks) -S DOR -p 0.3 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t torus -T $(ticks) -S DOR -p 0.4 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t torus -T $(ticks) -S DOR -p 0.5 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t torus -T $(ticks) -S DOR -p 0.6 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t torus -T $(ticks) -S DOR -p 0.7 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t torus -T $(ticks) -S DOR -p 0.8 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t torus -T $(ticks) -S DOR -p 0.9 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t torus -T $(ticks) -S DOR -p 1.0 2>/dev/null | tail -n 1 >> $@

data/cartesian_torus.tsv: nocsim/cli.py
	mkdir -p data
	python3 nocsim/cli.py -t torus -T $(ticks) -S cartesian -p 0.1 2>/dev/null | tail -n 2 > $@
	python3 nocsim/cli.py -t torus -T $(ticks) -S cartesian -p 0.2 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t torus -T $(ticks) -S cartesian -p 0.3 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t torus -T $(ticks) -S cartesian -p 0.4 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t torus -T $(ticks) -S cartesian -p 0.5 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t torus -T $(ticks) -S cartesian -p 0.6 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t torus -T $(ticks) -S cartesian -p 0.7 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t torus -T $(ticks) -S cartesian -p 0.8 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t torus -T $(ticks) -S cartesian -p 0.9 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -t torus -T $(ticks) -S cartesian -p 1.0 2>/dev/null | tail -n 1 >> $@


clean:
	rm -rf data/
	rm -rf figures/
