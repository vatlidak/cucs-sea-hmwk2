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
#include <fcntl.h>
#include <limits.h>
#include <ctype.h>

#include "spooler.h"

uid_t euid, ruid;


/*
 * generate_random_string - use /dev/urandom to create a random string
 *
 * @dst: the destination to store the string created
 * @len: the lenght of the random string
 */
static inline int generate_random_string(char *dst, int len)
{
	int i;
	int rval;
	FILE *fp;

	fp = fopen("/dev/urandom", "r");
	if (!fp) {
		perror("fopen");
		goto error;
	}
	i = 0;
	while (i < len - 1) {
		do {
			rval = fread(dst + i, 1, 1, fp);
			if (rval != 1) {
				perror("fread");
				goto error_close;
			}
		} while (!isalpha(dst[i]) && !isalnum(dst[i]));
		i++;
	}
	dst[len - 1] = '\0';
	fclose(fp);
	return 0;

error_close:
	fclose(fp);
error:
	return -1;
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
		perror("open (src)");
		return -1;
	}
	fd_dst = open(dst, O_WRONLY | O_CREAT | O_EXCL, 0700);
	if (fd_dst == -1) {
		perror("open (dst)");
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
	char dst[PATH_MAX];
	char fid[FILEID_LEN+1];

	memset(fid, 0, FILEID_LEN+1);
	rval = generate_random_string(fid, FILEID_LEN+1);
	if (rval) {
		printf("%s: X Failed to generate random fid\n", filename);
		goto error;
	}

	snprintf(dst, PATH_MAX, "%s/%s", PRINT_SPOOLER_PATH, fid);
	rval = copy_file(filename, dst);
	if (rval) {
		printf("%s: X Failed to copy file\n", filename);
		goto error;
	}
	rval = chown(dst, euid, ruid);
	if (rval) {
		printf("%s: X Failed chown of file\n", filename);
		goto error;
	}
	printf("%s: Y %s\n", filename, fid);
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
