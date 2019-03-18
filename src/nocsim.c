#include "nocsim.h"

int main(int argc, char** argv) {
	dprintf("beginning nocsim version %i.%i.%i\n",
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

	nocsim_grid_parse_file(stdin);
}
