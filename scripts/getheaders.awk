BEGIN {
	FS = "\t"
}

(NR == 1) {
	OFS="\t"
	for (i = 1 ; i <= NF ; i++) {
		print($(i), i)
	}
	next
}
