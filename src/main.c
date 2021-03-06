#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "main.h"

#include "convert.h"
#include "csv_parse.h"
#include "print.h"
#include "utils.h"

int
main(int argc, char **argv)
{

	int pipe_used;
	char **json;

	struct size s;
	struct row **rows;

	FILE *file;

	json = NULL, rows = NULL;

	pipe_used = !isatty(STDIN_FILENO);
	if (!pipe_used && argc < 2)
		print_use();

	file = (pipe_used) ? stdin : fopen(argv[1], "r");
	if (file == NULL) {
		perror("Error: Can't open file\n");
		exit(EXIT_FAILURE);
	}

	/* Get number of colums and rows from input that will to create
	   arrays of headers and value structs. */
	get_data_size(&s, file);

	/* /\* Rows is a pointer to an array of struct pointers, so we'll */
	/*    allocate space for the array and fill it with pointers to */
	/*    empty row structs. */
	/*  *\/ */
	populate_rows(&s, &rows);

	/* /\* Here is where the values of input is mapped to structs and */
	/*    added to the overall array of row structs.*\/ */
	make_rows(&s, rows, file);

	/* /\* /\\* Taking the row array, we convert struct into JSON string.
	 * *\\/ *\/ */
	convert_to_json(&json, &s, rows);

	/* /\* /\\* We print the JSON to stdout, which can be directed to a file
	 * *\/ */
	/* /\*    or the terminal *\\/ *\/ */
	print_json(json);

	/* /\* /\\* Clean up loose memory *\\/ *\/ */
	free(rows);
	free(json);

	exit(EXIT_SUCCESS);

	return 0;
}
