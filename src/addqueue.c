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
 * copy_file - helper duplicating a file
 *
 * @src: the source file name
 * @dst: the destination file name
 *
 */
static int copy_file(const char *src, const char *dst)
{
	int rval;
	char block[4096];
	int fd_src, fd_dst;
	int bytes_read, bytes_written;

	fd_src = open(src, O_RDONLY);
	if (fd_src == -1) {
		perror("open(src)");
		return -1;
	}
	fd_dst = open(dst, O_WRONLY | O_CREAT | O_EXCL, 0700);
	if (fd_dst == -1) {
		perror("open(dst)");
		return -1;
	}

	while ((bytes_read = read(fd_src, block, 4096)) > 0) {
		char *p_out = block;

		bytes_written = 0;
		do {
			rval = write(fd_dst, p_out, bytes_read);
			if (rval >= 0) {
				bytes_written += rval;
				p_out += rval;
			} else if (errno != EINTR) {
				goto out_error;
			}
		} while (bytes_written != bytes_read);
	}
	if (!bytes_read)
		goto out_ok;
out_error:
	close(fd_dst);
	close(fd_src);
	return -1;
out_ok:
	close(fd_dst);
	close(fd_src);
	return 0;
}

/*
 * addqueue - implements addqueue command
 * @filename: the file name to be added in the queue
 *
 * This functions is invoked with escallated priviedges!
 */
int addqueue(char *filename)
{
	int rval;
	char *bname;
	char dst[PATH_MAX];

	bname = basename(filename);
	snprintf(dst, PATH_MAX, "%s/%s_%d", PRINT_SPOOLER_PATH, bname, ruid);
	rval = copy_file(filename, dst);
	if (rval) {
		fprintf(stderr, "Failed to copy file \"%s\" to \"%s\"\n",
			filename, dst);
		goto error;
	}
	rval = chown(dst, euid, ruid);
	if (rval) {
		perror("chown");
		goto error;
	}
	printf("file \"%s\" copied to \"%s\"\n", filename, PRINT_SPOOLER_PATH);

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
		if (addqueue(argv[i]))
			fprintf(stderr,
				"Failed to add \"%s\" in the queue\n",
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
