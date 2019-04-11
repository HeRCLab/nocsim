#!/bin/sh

# generate all data and figures

# Environment variables:
#
# PARALLEL_OPTS:
#
# Added to calls to GNU parallel. For example, to distribute over a cluster of
# machines: --sshloginfile ~/.parallel/sshloginfile  --sshdelay 0.1

cd "$(dirname "$0")"
cd ..

make clean
make -j4

SIZE=10
SEED=13245
TICKS=64000

mkdir -p out
rm -f out/*
WHERE="$(pwd)"
sleep 1
(
for topo in mesh torus utorus; do
	for P in 0.001 0.05 0.075 0.1 0.125 0.15 0.175 0.2 0.225 0.25 0.275 0.3 0.325 0.350 0.375 0.4 0.425 0.45 0.475 0.5 ; do
		for behavior in DOR ADOR ; do
			printf "cd '$WHERE' ; python3 scripts/gengrid.py --size $SIZE \
--topography $topo --P_inject $P --seed $SEED --behavior $behavior \
--ticks $TICKS | ./src/nocsim 2>/dev/null | python3 scripts/evalstats.py \
> 'out/$topo.$P.$behavior.out' 2> 'out/$topo.$P.$behavior.err'\n"
		done
	done
done ) | parallel $PARALLEL_OPTS --bar

cat $(ls -1 out/*.err | head -n1) > out/headers.tsv
cat out/headers.tsv out/*.out > out/results.tsv
cat out/headers.tsv out/mesh.*.DOR.out > out/results_mesh_DOR.tsv
cat out/headers.tsv out/torus.*.DOR.out > out/results_torus_DOR.tsv
cat out/headers.tsv out/utorus.*.DOR.out > out/results_utorus_DOR.tsv
cat out/headers.tsv out/mesh.*.ADOR.out > out/results_mesh_ADOR.tsv
cat out/headers.tsv out/torus.*.ADOR.out > out/results_torus_ADOR.tsv
cat out/headers.tsv out/utorus.*.ADOR.out > out/results_utorus_ADOR.tsv
rm -f out/*.out out/*.err out/headers.tsv

export GP_TERMINAL="set terminal pdf font 'DejaVu Serif'"
export GP_EXT=pdf

( for f in scripts/figures/*.sh ; do
	printf "sh '$f'\n"
done ) | parallel $PARALLEL_OPTS --bar

pdfunite out/*.pdf out/all.pdf
