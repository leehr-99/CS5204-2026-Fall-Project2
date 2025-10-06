#include <stdio.h> // for fprintf
#include <stdlib.h>  // for malloc
#include <unistd.h> // for optarg

#include "pagemap.h"

int main(int argc, char **argv) {
	
	char *bs = "4K";
	char *out = "out";

	int c;
	while ((c = getopt(argc, argv, "b:m:o:")) != -1) {
		switch (c) {
			case 'b':
				bs = optarg;
				break;
			case 'o':
				out = optarg;
				break;
			default:
				fprintf(stderr,
					"Usage: %s [-b {4K|1G}] [-o <output>]\n", argv[0]);
				exit(1);
		}
	}

	

	return 0;
}
