#include "nocsim.h"

int main(int argc, char** argv) {
	ll_node* head;
	const char* errstr;
	unsigned int tick;
	unsigned int seed;

	seed = (unsigned int) time(NULL);

	dbprintf("beginning nocsim version %i.%i.%i\n",
			NOCSIM_VERSION_MAJOR,
			NOCSIM_VERSION_MINOR,
			NOCSIM_VERSION_PATCH
	);

#ifdef __OpenBSD__
	dbprintf("detected platform OpenBSD\n");
#else
	dbprintf("detected platform POSIX\n");
#endif

	int opt;

	while ((opt = getopt(argc, argv, "Vs:")) != -1) {
		switch(opt) {
			case 'V':
				printf("nocsim %i.%i.%i\n",
						NOCSIM_VERSION_MAJOR,
						NOCSIM_VERSION_MINOR,
						NOCSIM_VERSION_PATCH
				);
				return EXIT_SUCCESS;
			case 's':
				/* RNG seed */
				seed = (unsigned int) strtonum(optarg,
						0, UINT_MAX, &errstr);
				if (errstr != NULL) {
					err(1, "could not parse RNG seed '%s'",
							optarg);
				}
				break;

			case '?':
				err(1, "unknown option: %c", optopt);
		}
	}

	for (; optind < argc ; optind++) {
		err(1, "extraneous option: %s", argv[optind]);
	}

	printf("metadata RNG_seed %u\n", seed);

#ifdef __OpenBSD__
	srand_deterministic(seed);
#else
	srand(seed);
#endif

	printf("metadata randsig1 %u\n", rand());
	printf("metadata randsig2 %u\n", rand());
	printf("metadata randsig3 %u\n", rand());

	head = nocsim_grid_parse_file(stdin);

	tick = 0;
	nocsim_step(head, tick);
}
