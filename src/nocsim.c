#include "nocsim.h"

int a[10];

int main(int argc, char** argv) {

    size_t i = 0;
    for (;i < 10; i++) {
        a[i] = i;
    }
    a[i] = i;
    printf("%i", a[i]);	


	nocsim_state* state;
	char* errstr;
	unsigned int seed;
	unsigned char flag_seed = 0;
	unsigned char flag_graphviz = 0;
	struct timeval start_time;
	struct timeval end_time;
	unsigned long long elapsed_ms;

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
#ifdef __OpenBSD__
				seed = (unsigned int) strtonum(optarg,
						0, UINT_MAX, &errstr);
				// TODO: figure out how to make this work for
				// strtoul also
				if (errstr != NULL) {
					err(1, "could not parse RNG seed '%s'",
							optarg);
				}
#else
				seed = (unsigned int) strtoul(optarg, &errstr, 10);
#endif
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

	state = nocsim_grid_parse_file(stdin);

	if (flag_graphviz == 1) {
		nocsim_dump_graphviz(stdout, state);
		exit(0);
	}

	printf("config RNG_seed %u\n", state->RNG_seed);
	printf("config max_ticks %u\n", state->max_ticks);
	printf("config default_P_inject %f\n", state->default_P_inject);
	printf("config title %s\n", state->title);
	printf("meta num_PE %u\n", state->num_PE);
	printf("meta num_router %u\n", state->num_router);
	printf("meta num_node %u\n", state->num_node);

	if (flag_seed == 1) {
		state->RNG_seed = seed;
	} else {
		seed = state->RNG_seed;
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

	gettimeofday(&start_time, NULL);
	for (unsigned int i = 0 ; i < state->max_ticks ; i++) {
		nocsim_step(state);
	}
	gettimeofday(&end_time, NULL);

	elapsed_ms = \
		((end_time.tv_sec - start_time.tv_sec) * 1000) +
		((end_time.tv_usec - start_time.tv_usec) / 1000);

	printf("meta elapsed_ms %llu\n", elapsed_ms);

	printf("meta status done\n");

	exit(0);
}
