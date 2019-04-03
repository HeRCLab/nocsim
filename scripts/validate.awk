# This script takes in a nocsim simulation log, and validates that it does not
# indicate any invalid operations. If nocsim is free of bugs, this script
# should output nothing

$1 ~ /tick/ {
	tickno = $2
	for (key in links) {
		if (links[key] < -1) {
			printf("VALIDATE %i flits sourced from link %s\n", links[key], key)

		} else if (links[key] > 1) {
			printf("VALIDATE %i flits sourced from link %s\n", links[key], key)

		}
	}
	
	for (key in seen) {
		if (!(key in seen_next)) {
			printf("VALIDATE lost a flit %s\n", key)
		}
	}

	for (key in links) { delete links[key] }
	for (key in seen) { delete seen[key] }

	for (key in seen_next) {
		seen[key] = seen_next[key]
	}

	for (key in seen_next) { delete seen_next[key] }
}

$1 ~ /pop/ {
	where[$2] = $6
}

$1 ~ /route/ {
	links[$5] ++
	links[where[$2]] --
	where[$2] = $5
	seen_next[$2] = 1
}

$1 ~ /arrived/ {
	delete seen_next[$2];
	delete seen[$2];
}

# uncomment to also show the log
# {
#         print
# }
