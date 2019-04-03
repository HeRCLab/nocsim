#!/bin/sh

cd "$(dirname "$0")"
cd ..

make clean
make

SIZE=10
SEED=13245
TICKS=1600

mkdir -p out
rm -f out/*
(
for topo in mesh ; do
	for P in 0.05 0.1 0.15 0.2 0.25 0.3 0.35 0.4 0.45 0.5 0.55 0.6 0.7 \
		0.75 0.8 0.85 0.9 0.95 1.0 ; do
		for behavior in DOR ; do
			printf "python3 scripts/gengrid.py --size $SIZE \
--topography $topo --P_inject $P --seed $SEED --behavior $behavior \
--ticks $TICKS | ./src/nocsim 2>/dev/null | python3 scripts/evalstats.py \
> 'out/$topo.$P.$behavior.out' 2> 'out/$topo.$P.$behavior.err'\n"
		done
	done
done ) | parallel --bar

cat $(ls -1 out/*.err | head -n1) > out/results.tsv
cat out/*.out >> out/results.tsv
rm out/*.out out/*.err
