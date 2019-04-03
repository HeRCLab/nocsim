#include "nocsim.h"

int main(int argc, char** argv) {
	ll_node* head;
	const char* errstr;
	unsigned int tick;
	unsigned int seed;
	unsigned char flag_seed = 0;
	unsigned char flag_graphviz = 0;

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

	while ((opt = getopt(argc, argv, "Vs:g")) != -1) {
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
				flag_seed = 1;
				break;

			case 'g':
				flag_graphviz = 1;
				break;

			case '?':
				err(1, "unknown option: %c", optopt);
		}
	}

	for (; optind < argc ; optind++) {
		err(1, "extraneous option: %s", argv[optind]);
	}

	head = nocsim_grid_parse_file(stdin);

	if (flag_graphviz == 1) {
		nocsim_dump_graphviz(stdout, head);
		exit(0);
	}

	printf("config RNG_seed %u\n", ll2meta(head)->RNG_seed);
	printf("config max_ticks %u\n", ll2meta(head)->max_ticks);
	printf("config default_P_inject %f\n", ll2meta(head)->default_P_inject);
	printf("config title %s\n", ll2meta(head)->title);
	printf("meta num_PE %u\n", ll2meta(head)->num_PE);
	printf("meta num_router %u\n", ll2meta(head)->num_router);
	printf("meta num_node %u\n", ll2meta(head)->num_node);

	if (flag_seed == 1) {
		ll2meta(head)->RNG_seed = seed;
	} else {
		seed = ll2meta(head)->RNG_seed;
	}

	printf("meta RNG_seed %u\n", seed);

#ifdef __OpenBSD__
	srand_deterministic(seed);
#else
	srand(seed);
#endif

	printf("meta randsig1 %u\n", rand());
	printf("meta randsig2 %u\n", rand());
	printf("meta randsig3 %u\n", rand());

	tick = 0;
	for (unsigned int i = 0 ; i < ll2meta(head)->max_ticks ; i++) {
		nocsim_step(head);
	}

	printf("meta status done\n");
}
