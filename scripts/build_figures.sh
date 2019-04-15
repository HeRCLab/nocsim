#!/bin/sh

# This script generates a collection of figures and results, which it places in
# the "out" directory in the top-level of this project.

# These figures and results are specifically for the poster "A Review of
# FPGA-Oriented Network-on-Chip Routing Algorithms & Topographies" presented at
# Discover USC, April 2019. They also serve as a useful representative example
# of how to use nocsim to generate re-producible results.

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

# Grid size
SIZE=10

# RNG seed to use, to ensure deterministic output
SEED=13245

# Number of ticks to run for
TICKS=640

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

export GP_TERMINAL="set terminal eps font 'DejaVu Serif'"
export GP_EXT=eps

( for f in scripts/figures/*.sh ; do
	printf "sh '$f'\n"
done ) | parallel $PARALLEL_OPTS --bar

export GP_TERMINAL="set terminal pdf font 'DejaVu Serif'"
export GP_EXT=pdf

( for f in scripts/figures/*.sh ; do
	printf "sh '$f'\n"
done ) | parallel $PARALLEL_OPTS --bar

export GP_TERMINAL="set terminal svg font 'DejaVu Serif'"
export GP_EXT=svg

( for f in scripts/figures/*.sh ; do
	printf "sh '$f'\n"
done ) | parallel $PARALLEL_OPTS --bar

export GP_TERMINAL="set terminal png font 'DejaVu Serif'"
export GP_EXT=png

( for f in scripts/figures/*.sh ; do
	printf "sh '$f'\n"
done ) | parallel $PARALLEL_OPTS --bar

pdfunite out/*.pdf out/all.pdf
