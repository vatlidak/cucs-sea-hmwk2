/*
 * Filename: src/showqueue.c
 *
 * Description: Implement showqueue command
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
#include <dirent.h>
#include <limits.h>
#include <time.h>

#include "spooler.h"


uid_t euid, ruid;

/*
 * sortbydatetime - helper for scandir sorting
 */
static inline int
sortbydatetime(const struct dirent **a, const struct dirent **b)
{
	int rval;
	struct stat sbuf1, sbuf2;
	char path1[PATH_MAX], path2[PATH_MAX];

	snprintf(path1, PATH_MAX, "%s/%s", PRINT_SPOOLER_PATH, (*a)->d_name);
	snprintf(path2, PATH_MAX, "%s/%s", PRINT_SPOOLER_PATH, (*b)->d_name);

	rval = stat(path1, &sbuf1);
	if (rval) {
		perror("stat");
		return 0;
	}
	rval = stat(path2, &sbuf2);
	if (rval) {
		perror("stat");
		return 0;
	}

	return sbuf1.st_mtime < sbuf2.st_mtime;
}

/*
 * showqueue - implements showqueue command
 * @dirame: print spooler directory
 *
 * This functions is invoked with escallated priviedges!
 */
int showqueue(char *dirname)
{
	int n;
	int rval;
	int nfile;
	char path[PATH_MAX];

	DIR *pdir;
	struct stat sbuf;
	struct dirent **pdirent;

	struct tm lt;
	char strdate[16];
	char strtime[16];


	pdir = opendir(dirname);
	if (!pdir) {
		perror("opendir");
		goto error;
	}
	nfile = 0;
	printf("--------------------------------------------------------\n");
	printf("nfile\tuid\tdate-time\t\tfile id\n");
	printf("--------------------------------------------------------\n");

	n = scandir(dirname, &pdirent, NULL, sortbydatetime);
	if (n == -1) {
		perror("scandir");
		goto error;
	}
	while (n--) {
		if (!strcmp(pdirent[n]->d_name, ".") ||
		    !strcmp(pdirent[n]->d_name, ".."))
			goto free_and_continue;
		snprintf(path, PATH_MAX, "%s/%s", dirname, pdirent[n]->d_name);
		rval = stat(path, &sbuf);
		if (rval) {
			perror("stat");
			goto free_and_continue;
		}
		localtime_r(&sbuf.st_mtime, &lt);
		memset(strtime, 0, sizeof(strtime));
		strftime(strtime, sizeof(strtime), "%H:%M:%S", &lt);
		memset(strdate, 0, sizeof(strdate));
		strftime(strdate, sizeof(strdate), "%F", &lt);
		printf("%d\t%d\t%s-%s\t%s\n",
		       ++nfile, sbuf.st_gid, strdate, strtime, pdirent[n]->d_name);
free_and_continue:
		free(pdirent[n]);
	}
	free(pdirent);
	printf("--------------------------------------------------------\n");
	return 0;
error:
	printf("--------------------------------------------------------\n");
	return -1;
}

/*
 * main: main entry point
 */
int main(int argc, char **argv)
{
	int rval;

	if (argc != 1)
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

	rval = setuid(euid);
	if (rval) {
		fprintf(stderr, "Failed to escallate to effective uid: %d\n",
			euid);
		goto error;
	}
	if (showqueue(PRINT_SPOOLER_PATH)) {
		fprintf(stderr, "Failed to show print spooler queue\n");
		goto error;
	}

	rval = setuid(ruid);
	if (rval) {
		fprintf(stderr, "Failed to reset to real uid: %d\n",
			ruid);
		goto error;
	}
	return 0;
usage:
	fprintf(stderr, "Usage: %s\n", *argv);
error:
	return -1;
}
