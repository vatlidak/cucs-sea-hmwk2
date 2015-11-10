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

#include "defines.h"


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
 * showqueue - implements showqueue command
 * @dirame: print spooler directory
 *
 * This functions is invoked with escallated priviedges!
 */
int showqueue(char *dirname)
{
	int i;
	int rval;
	int nfile;
	char path[PATH_MAX];

	DIR *pdir;
	struct stat sbuf;
	struct dirent *pdirent;

	struct tm lt;
	char strdate[16];
	char strtime[16];


	pdir = opendir(dirname);
	if (!pdir) {
		perror("opendir");
		goto error;
	}
	i = 0;
	nfile = 0;
	printf("--------------------------------------------------------\n");
	printf("nfile\tuid\tdate-time\t\tfile id\n");
	printf("--------------------------------------------------------\n");
	while ((pdirent = readdir(pdir)) != NULL) {
		++i;
		if (!strcmp(pdirent->d_name, ".") ||
		    !strcmp(pdirent->d_name, ".."))
			continue;
		snprintf(path, PATH_MAX, "%s/%s", dirname, pdirent->d_name);
		rval = stat(path, &sbuf);
		if (rval) {
			perror("stat");
			goto error;
		}
		localtime_r(&sbuf.st_mtime, &lt);
		memset(strtime, 0, sizeof(strtime));
		strftime(strtime, sizeof(strtime), "%H:%M:%S", &lt);
		memset(strdate, 0, sizeof(strdate));
		strftime(strdate, sizeof(strdate), "%F", &lt);
		printf("%d\t%d\t%s-%s\t%s\n",
		       ++nfile, sbuf.st_gid, strdate, strtime, pdirent->d_name);
	}
	if (!i)
		goto error;

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
