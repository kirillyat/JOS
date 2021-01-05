#include <inc/lib.h>

void
umain(int argc, char **argv)
{
	int i, r;
	binaryname = "mkfifo";
	if (argc < 2)
		printf("No arguments\n");

	for (i = 1; i < argc; i++) {
		if((r = mkfifo(argv[i])) < 0)
			printf("can't create %s: %i\n", argv[i], r);
	}
}
