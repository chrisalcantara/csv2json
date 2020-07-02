#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csv2json.h"

void
print_use(void)
{
	printf("Usage:\n");
	printf("\t$ csv2json data.csv > outfile.json\n");
	printf("\t$ cat data.csv | csv2json\n");
	printf("\t$ csv2json <data.csv\n");
	exit(EXIT_SUCCESS);
}

/* Read data from stdin or from file and dump it into a
   pointer to a character array -- input -- that grows
   the more data that is read.  */
void
read_input(char **input, size_t *input_content_size, char *ch)
{
	*input = realloc(*input, *input_content_size * sizeof(char *));
	strncat(*input, ch, 1);
	(*input_content_size)++;
}

/* Clean columns array */
void
free_columns(int cols, char **columns)
{
	for (int i = 0; i < cols; ++i) {
		free(columns[i]);
	}
}

void
free_structs(struct row **rows, int n_rows)
{
	for (int i = 0; i < n_rows; ++i) {
		/* Freeing the struct is enough */
		free(rows[n_rows]);
	}
}

/* Send final data to output. */
void
print_json(char *json)
{
	fprintf(stdout, "%s\n", json);
}

/* Create a pointer array to struct row pointers. */
struct row **
populate_rows(struct size *s)
{
	/* Create block that can will hold s->rows of struct row pointers */
	struct row **rows = malloc(s->rows * sizeof(struct row *));

	/* Loop through and populate. */
	for (int i = 0; i < s->rows; ++i) {
		struct row *r = malloc(sizeof(struct row));
		rows[i] = r;
	}
	return rows;
}
