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

#include "spooler.h"


uid_t euid, ruid;

/*
 * addqueue - implements addqueue command
 * @filename: the file name to be deleted from the queue
 *
 * This functions is invoked with escallated priviedges!
 */
int rmqueue(char *filename)
{
	int rval;
	char path[PATH_MAX];
	struct stat sbuf;

	snprintf(path, PATH_MAX, "%s/%s", PRINT_SPOOLER_PATH, filename);

	rval = stat(path, &sbuf);
	if (rval) {
		printf("%s: X stat error\n", filename);
		goto error;
	}
	if (sbuf.st_gid !=  ruid) {
		printf("%s: X permission error\n", filename);
		goto error;
	}
	rval = unlink(path);
	if (rval) {
		printf("%s: X unlink error\n", filename);
		goto  error;
	}
	printf("%s: Y\n", filename);
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
