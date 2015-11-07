/*
 * Filename: src/addqueue.c
 *
 * Description: Implement addqueue command
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
#include <errno.h>


#define SPOOLER_PATH "/var/print_spooler"
#define SPOOLER_PERM 0700


/*
 * initqueue - creates spooler directory, if it doesn't exist
 */
static int initqueue(void)
{
	int rval;
	struct stat sbuf;

	rval = stat(SPOOLER_PATH, &sbuf);
	if (rval == -1 && errno != ENOENT) {
		perror("stat");
		return -1;
	}
	else if (rval == -1 && errno == ENOENT) {
		fprintf(stdout, "Creating print spooler directory: \"%s\"\n", SPOOLER_PATH);
		rval = mkdir(SPOOLER_PATH, SPOOLER_PERM);
		if (rval){
			perror("mkdir");
			return  -1;
		}
	}
	return 0;
}

/*
 * addqueue - implements addqueue command
 * @filename: the file name to be added in the queue
 *
 * comment
 */
int addqueue(char *filename)
{
	printf("filename:%s\n", filename);
	return 0;
}

/*
 * main: main entry point
 */
int main(int argc, char **argv)
{
	int i = 0;

	if (argc < 2)
		goto usage;

	if (initqueue())
		goto error;

	while (++i < argc) {
		if (addqueue(argv[i]))
			fprintf(stderr, "Failed to add \"%s\" in the queue\n", argv[i]);
	}

	return 0;
usage:
	fprintf(stderr,"Usage: %s <filename1> [<filename2> ... <filenameN>]\n", *argv);
error:
	return -1;
}
