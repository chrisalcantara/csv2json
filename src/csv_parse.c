#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csv2json.h"

static void
check_if_in_quotes(char curr, char prev, int *in_quotes)
{
	/* Check if we are seeing a character
	   that is between quotes.*/
	if (curr == '"' && *in_quotes == 0) {
		*in_quotes = 1;
	} else if (prev == '"' && curr == ',' && *in_quotes == 1)
		*in_quotes = 0;
}

/* Read in a string, ususally a CSV row:

     col1,col2,col3col4

   and use pointer arithmetic to manually
   the split the row. and them to *collection,
   which is pointer to an array of character pointers.

      char** collection = {*col1, *col2, *col3, *col4};
*/
static void
split_line(char *string, char **collection)
{
	char curr, prev, next;
	char *temp;

	int in_quotes, count;

	in_quotes = 0;
	count = 0;

	/* Temp is where we hold the concated string.
	   We'll give it a 1000 bytes to start. */
	temp = malloc(SPLIT_LINE_TEMP_SIZE);

	while (*string != '\0') {

		curr = *string;
		prev = *(string - 1);
		next = *(string + 1);

		check_if_in_quotes(curr, prev, &in_quotes);

		strncat(temp, &curr, 1);

		/* Split the string when we hit a common not in quotes
		   or if we're at the end of the string */
		if ((curr == ',' && in_quotes == 0) || next == '\0') {
			size_t s;
			char *output;

			/* Clean up temp string */
			s = strlen(temp);
			temp[s - 1] = '\0';

			output = malloc(s + 1 * sizeof(char *));
			strcpy(output, temp);

			collection[count] = output;

			temp[0] = '\0';
			string++;
			count++;
			continue;
		}
		string++;
	}
	free(temp);
}

/* Map structs for row headers and values to an array to
   be converted to JSON. */
void
make_rows(struct size *s, struct row **r, char *file)
{

	int i = 0;
	int rows = 0;

	char buf[BUFSIZ];
	char **headers, **values;

	headers = malloc(s->cols * sizeof(char *));

	/* Same split method as in split_lines, but also handles
	   headers and values, as well as maps to struct  */
	while (*file) {
		buf[i++] = *file;
		if (*file == '\n') {
			buf[i - 1] = '\0';

			/* Handle getting sole header array */
			if (rows == 0) {
				split_line(buf, headers);
				rows++;
				buf[0] = '\0';
				i = 0;
				file++;
				continue;
			}

			/* Otherwise, create value array block
			   and copy into it. */
			values = malloc(s->cols * sizeof(char *));
			split_line(buf, values);

			/* Data goes up by one, but rows nees the index. */
			r[rows - 1]->headers = headers;
			r[rows - 1]->values = values;
			r[rows - 1]->cols = s->cols;

			rows++;
			i = 0;
		}
		file++;
	}
}

/* Helps determine how large the data is, row- and column-wise */
void
get_data_size(struct size *s, char *file)
{
	int i, rows, cols;

	char buf[BUFSIZ];
	char **columns;

	/* Start by allocating 500 columns -- pointers to
	   get the number of columns. If the data is larger,
	   then up the MAX_COLUMNS. We free this pointer array
	   later.*/
	columns = malloc(MAX_COLUMNS * sizeof(char *));

	/* Here's where we'll get the row count
	   For row, we start at -1 to account for
	   the header row.*/
	i = 0;
	rows = -1;
	cols = 0;

	/* Use pointer arithmetic to split each row  */
	while (*file) {

		/* Buf is a placeholder */
		buf[i++] = *file;

		/* Do the split when we hit a new line  */
		if (*file == '\n') {

			buf[i] = '\0';

			if (rows == -1) {
				split_line(buf, columns);

				/* quickly get number of columns */
				while (*columns != NULL) {
					cols++;
					columns++;
				}
			}
			rows++;
			i = 0;
		}
		file++;
	}

	s->rows = rows;
	s->cols = cols;

	/* Clean columns array */
	for (int i = 0; i < cols; ++i) {
		free(columns[i]);
	}
}
