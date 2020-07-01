#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "csv2json.h"

#include "convert.h"
#include "csv_parse.h"
#include "utils.h"

int
main(int argc, char **argv)
{
	FILE *file;
	struct size s;
	struct row **rows;

	bool pipe_used;
	char input_char;
	size_t input_size;

	char *json;
	char *input;

	input_size = 0;
	input = malloc(1 * sizeof(char *));

	/* Determine if data is coming from
	   stdin or file. */
	pipe_used = !isatty(STDIN_FILENO);

	if (!pipe_used && argc < 2)
		print_use();

	if (pipe_used) {
		while ((input_char = getchar()) != EOF)
			read_input(&input, &input_size, &input_char);

	} else {
		file = fopen(argv[1], "r");
		if (file == NULL) {
			perror("Error: Can't open file or it doesn't "
			       "exist");
			exit(EXIT_FAILURE);
		}

		while ((input_char = fgetc(file)) != EOF)
			read_input(&input, &input_size, &input_char);
	}

	/* Allocate space for the final json. This will be resized
	   multiple times during the conversation */
	json = malloc(10 * sizeof(char *));

	/* Get number of colums and rows from input that will to create
	   arrays of headers and value structs. */
	get_data_size(&s, input);

	/* Rows is a pointer to an array of struct pointers, so we'll
	   allocate space for the array and fill it with pointers to
	   empty row structs.
	 */
	rows = populate_rows(&s);

	/* Here is where the values of input is mapped to structs and
	   added to the overall array of row structs.*/
	make_rows(&s, rows, input);

	/* Taking the row array, we convert struct into JSON string. */
	convert_to_json(&json, &s, rows);

	/* We print the JSON to stdout, which can be directed to a file
	   or the terminal */
	print_json(json);

	/* Clean up loose memory */
	free(input);
	free(json);
	free_structs(rows, s.rows);

	exit(EXIT_SUCCESS);

	return 0;
}
