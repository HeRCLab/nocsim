# general purpose library of statistics functions, include this by running 'awk
# -f stats.awk -f yourprogram.awk'

function mean(list,    key, sum, len) {
	for (key in list) {
		sum += list[key]
		len ++
	}
	return sum / len
}
