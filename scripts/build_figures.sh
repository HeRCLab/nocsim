#!/bin/sh

cd "$(dirname "$0")"
cd ..

make clean
make -j4

SIZE=10
SEED=13245
TICKS=500

mkdir -p out
rm -f out/*
(
for topo in mesh torus ; do
	# for P in 0.01 0.025 0.05 0.1 0.15 0.2 0.25 0.3 0.35 0.4 0.45 0.5 0.55 0.6 0.65 0.7 \
	#         0.75 0.8 0.85 0.9 0.95 0.99 1.0 ; do
	for P in 0.01 0.02 0.03 0.04 0.05 0.075 0.1 0.125 0.15 0.175 0.2 0.225 0.25 0.275 0.3 0.35 0.4; do
		for behavior in DOR ADOR ; do
			printf "python3 scripts/gengrid.py --size $SIZE \
--topography $topo --P_inject $P --seed $SEED --behavior $behavior \
--ticks $TICKS | ./src/nocsim 2>/dev/null | python3 scripts/evalstats.py \
> 'out/$topo.$P.$behavior.out' 2> 'out/$topo.$P.$behavior.err'\n"
		done
	done
done ) | parallel --bar

cat $(ls -1 out/*.err | head -n1) > out/headers.tsv
cat out/headers.tsv out/*.out > out/results.tsv
cat out/headers.tsv out/mesh.*.DOR.out > out/results_mesh_DOR.tsv
cat out/headers.tsv out/torus.*.DOR.out > out/results_torus_DOR.tsv
cat out/headers.tsv out/mesh.*.ADOR.out > out/results_mesh_ADOR.tsv
cat out/headers.tsv out/torus.*.ADOR.out > out/results_torus_ADOR.tsv
rm -f out/*.out out/*.err out/headers.tsv

export GP_TERMINAL="set terminal pdf font 'DejaVu Serif'" 
export GP_EXT=pdf

( for f in scripts/figures/*.sh ; do
	printf "sh '$f'\n"
done ) | parallel --bar

pdfunite out/*.pdf out/all.pdf
