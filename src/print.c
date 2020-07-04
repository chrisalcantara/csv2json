
#include <stdio.h>
#include <stdlib.h>

void
print_use(void)
{
	printf("Usage:\n");
	printf("\t$ csv2json data.csv > outfile.json\n");
	printf("\t$ cat data.csv | csv2json\n");
	printf("\t$ csv2json <data.csv\n");
	exit(EXIT_SUCCESS);
}

/* Send final data to output. */
void
print_json(char **json)
{
	while (*json != NULL) {
		fprintf(stdout, "%s", *json);
		json++;
	}
}

void
print_column_length_err(void)
{
	fprintf(stderr, "%s",
	    "Error: No. of columns don't match with "
	    "the number of headers calculated. This "
	    "program splits by comma. Is there a stray "
	    "comma in the file's header row?\n");
	exit(EXIT_SUCCESS);
}
