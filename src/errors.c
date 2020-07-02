#include <stdio.h>
#include <stdlib.h>

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
