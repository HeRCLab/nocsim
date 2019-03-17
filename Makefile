.PHONY: clean all all_figures

ticks=100

all: data/DOR_mesh.tsv data/cartesian_mesh.tsv all_figures

all_figures: figures/throughput_cartesian.pdf figures/throughput_DOR.pdf

data:
	mkdir -p data/

figures:
	mkdir -p figures/

figures/throughput_cartesian.pdf: data/cartesian_mesh.tsv figures
	xsv select -d '\t' "injection rate","throughput" < data/cartesian_mesh.tsv | \
		sed '1d'  | \
		gnuplot \
			-e 'set terminal pdf' \
			-e 'set datafile separator ","' \
			-e 'set title "Throughput (cartesian sorting)"' \
			-e 'set xlabel "injection rate (flit-routers per cycle)"' \
			-e 'set ylabel "throughput (flit-routers per cycle)"' \
			-e 'plot "/dev/stdin" with linespoints' > $@.tmp
	mv $@.tmp $@

figures/throughput_DOR.pdf: data/DOR_mesh.tsv figures
	xsv select -d '\t' "injection rate","throughput" < data/DOR_mesh.tsv | \
		sed '1d' | \
		gnuplot \
			-e 'set datafile separator ","' \
			-e 'set terminal pdf' \
			-e 'set title "Throughput (DOR sorting)"' \
			-e 'set xlabel "injection rate (flit-routers per cycle)"' \
			-e 'set ylabel "throughput (flit-routers per cycle)"' \
			-e 'plot "/dev/stdin" with linespoints' > $@.tmp
	mv $@.tmp $@


data/DOR_mesh.tsv: data
	python3 nocsim/cli.py -T $(ticks) -S DOR -p 0.1 2>/dev/null | tail -n 2 > $@
	python3 nocsim/cli.py -T $(ticks) -S DOR -p 0.2 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -T $(ticks) -S DOR -p 0.3 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -T $(ticks) -S DOR -p 0.4 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -T $(ticks) -S DOR -p 0.5 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -T $(ticks) -S DOR -p 0.6 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -T $(ticks) -S DOR -p 0.7 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -T $(ticks) -S DOR -p 0.8 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -T $(ticks) -S DOR -p 0.9 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -T $(ticks) -S DOR -p 1.0 2>/dev/null | tail -n 1 >> $@

data/cartesian_mesh.tsv: data
	python3 nocsim/cli.py -T $(ticks) -S cartesian -p 0.1 2>/dev/null | tail -n 2 > $@
	python3 nocsim/cli.py -T $(ticks) -S cartesian -p 0.2 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -T $(ticks) -S cartesian -p 0.3 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -T $(ticks) -S cartesian -p 0.4 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -T $(ticks) -S cartesian -p 0.5 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -T $(ticks) -S cartesian -p 0.6 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -T $(ticks) -S cartesian -p 0.7 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -T $(ticks) -S cartesian -p 0.8 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -T $(ticks) -S cartesian -p 0.9 2>/dev/null | tail -n 1 >> $@
	python3 nocsim/cli.py -T $(ticks) -S cartesian -p 1.0 2>/dev/null | tail -n 1 >> $@

clean:
	rm -rf data/
	rm -rf figures/
