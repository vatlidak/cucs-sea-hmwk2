#define PRINT_SPOOLER_PATH "/var/spool/print"
#define FILEID_LEN 16

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
