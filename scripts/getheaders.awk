# This script is used to get a list of TSV headers and their
# column numbers. It is used by figure generation scripts to
# determine which columns of a TSV to slice to generate each
# figure.

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
