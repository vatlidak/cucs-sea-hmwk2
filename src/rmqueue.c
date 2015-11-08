/*
 * Filename: src/rmqueue.c
 *
 * Description: Implement rmqueue command
 *
 * Copyright (C) 2015 V. Atlidakis
 *
 * COMS W4187 Fall 2015, Columbia University
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>

#define PRINT_SPOOLER_PATH "/var/print_spooler"


uid_t euid, ruid;


/*
 * initqueue - asserts proper installation of print spooler
 */
static inline int is_not_installed(void)
{
	int rval;
	struct stat sbuf;

	rval = stat(PRINT_SPOOLER_PATH, &sbuf);
	if (rval == -1) {
		fprintf(stdout, "print spooler not properly installed\n");
		return  -1;
	}
	return 0;
}

/*
 * addqueue - implements addqueue command
 * @filename: the file name to be deleted from the queue
 *
 * This functions is invoked with escallated priviedges!
 */
int rmqueue(char *filename)
{
	int rval;
	char *bname;
	char dst[PATH_MAX];

	return 0;

	bname = basename(filename);
	rval = snprintf(dst, PATH_MAX, "%s/%s_%d",
			PRINT_SPOOLER_PATH, bname, ruid);
	if (rval <= 0) {
		perror("snprintf");
		goto error;
	}
	printf("file \"%s\" copied to \"%s\"\n", filename, dst);

	return 0;
error:
	return -1;
}

/*
 * main: main entry point
 */
int main(int argc, char **argv)
{
	int rval;
	int i = 0;

	if (argc < 2)
		goto usage;

	if (is_not_installed())
		goto error;

	ruid = getuid();
	euid = geteuid();
	rval = setuid(ruid);
	if (rval) {
		fprintf(stderr, "Failed to reset to real uid: %d\n", ruid);
		goto error;
	}

	while (++i < argc) {
		rval = setuid(euid);
		if (rval) {
			fprintf(stderr,
				"Failed to escallate to effective uid: %d\n",
				euid);
			goto error;
		}
		if (rmqueue(argv[i]))
			fprintf(stderr,
				"Failed to rm \"%s\" from the queue\n",
				argv[i]);
		rval = setuid(ruid);
		if (rval) {
			fprintf(stderr, "Failed to reset to real uid: %d\n",
				ruid);
			goto error;
		}
	}

	return 0;
usage:
	fprintf(stderr,
		"Usage: %s <filename1> [<filename2> ... <filenameN>]\n",
		*argv);
error:
	return -1;
}
