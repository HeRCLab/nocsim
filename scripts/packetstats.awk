$1 ~ /tick/ { tickno = $2 }

($1 ~ /config/ && $2 ~ /title/) {
	$1 = ""
	$2 = ""
	gsub(/^[ \t]+/,"")
	title = $0
}

$1 ~ /inject/ {
	fromrow = $5
	fromcol = $6
	torow = $9
	tocol = $10
	cartesian[$2] = sqrt((tocol - fromcol) ** 2 + (torow - fromrow) ** 2)
	tick[$2] = tickno
	injected[tickno] ++
}

$1 ~ /route/ {
	hops[$2] ++
}

$1 ~ /arrived/ {
	arrived[$2] = tickno
}

END {
	# printf("ID\tCDIST\tHOPS\tINJ'D\tARRIVED\n") | "cat 1>&2"
	# for (key in cartesian) {
	#         printf("%s\t%3.2f\t%u\t%u\t%s\n",
	#                key,
	#                cartesian[key],
	#                hops[key],
	#                tick[key],
	#                (key in arrived) ? arrived[key] : "NO")
	# }

	OFS="\t"
	printf("TITLE\tINJ'D\tARRIVED\tAINJ'd\n") | "cat 1>&2"
	for (key in cartesian) {
		n_injected ++
		if (key in arrived) {
			n_arrived ++
			dist[key] = hops[key]
			ratio[key] = hops[key] / cartesian[key]
		}
		
	}
	print(title, n_injected, n_arrived, mean(injected));
}
