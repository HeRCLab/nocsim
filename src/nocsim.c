#include "nocsim.h"

int main(int argc, char** argv) {
	ll_node* head;
	unsigned int tick;

	dbprintf("beginning nocsim version %i.%i.%i\n",
			NOCSIM_VERSION_MAJOR,
			NOCSIM_VERSION_MINOR,
			NOCSIM_VERSION_PATCH
	);

	int opt;

	while ((opt = getopt(argc, argv, ":V")) != -1) {
		switch(opt) {
			case 'V':
				printf("nocsim %i.%i.%i\n",
						NOCSIM_VERSION_MAJOR,
						NOCSIM_VERSION_MINOR,
						NOCSIM_VERSION_PATCH
				);
				return EXIT_SUCCESS;
			case '?':
				printf("unknown option: %c\n", optopt);
				return EXIT_FAILURE;
		}
	}

	for (; optind < argc ; optind++) {
		printf("extraneous option: %s\n", argv[optind]);
		return EXIT_FAILURE;
	}

	head = nocsim_grid_parse_file(stdin);

	tick = 0;
	nocsim_step(head, tick);
}
