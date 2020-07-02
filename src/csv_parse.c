#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"

#include "errors.h"
#include "utils.h"

static void
split_by_comma(int *count, char *line, char **collection)
{
	size_t token_length;
	char *token, *text, *output;

	text = strdup(line);

	while ((token = strsep(&text, ",")) != NULL) {
		trim(token);
		token_length = strlen(token);
		output = malloc(token_length * sizeof(char *));
		strncpy(output, token, token_length);
		collection[*count] = output;
		(*count)++;
	}
}

static bool
check_comma_in_quotes(char *line)
{
	char *check_for_quotes;
	char *check_for_comma;
	char next_item;

	if ((check_for_quotes = strstr(line, "\"")) != NULL) {
		if ((check_for_comma = strstr(check_for_quotes, ",")) != NULL) {
			if ((next_item = check_for_comma[1]) != '\"')
				return true;
		}
	}
	return false;
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

	int count;
	bool in_quotes;

	in_quotes = false;
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

		/* Split the string when we hit a comma not in quotes
		   or if we're at the end of the string */
		if ((curr == ',' && in_quotes == 0) || next == '\0') {

			size_t s;
			char *output;

			trim(temp);
			s = strlen(temp);

			output = malloc(s + 1 * sizeof(char *));
			strcpy(output, temp);

			collection[count] = output;

			temp[0] = '\0';
			string++, count++;
			continue;
		}
		string++;
	}
	free(temp);
}

void
make_split(int *i, char *line, char **arr, bool found, struct size *s,
    void (*split_by_comma)(int *, char *, char **),
    void (*split_line)(char *, char **))
{
	if (!found) {
		split_by_comma(i, line, arr);
		if (s->cols != *i)
			print_column_length_err();
	} else {
		split_line(line, arr);
	}
}

/* Map structs for row headers and values to an array to
   be converted to JSON. */
void
make_rows(struct size *s, struct row **r, FILE *file)
{
	char *line, **headers;

	size_t len, multiple_columns_size;
	ssize_t read;

	int rows;

	line = NULL;
	len = 0;
	rows = -1;

	multiple_columns_size = s->cols * sizeof(char *);

	headers = malloc(multiple_columns_size);

	/* Same split method as in split_lines, but also handles
	   headers and values, as well as maps to struct  */
	while ((read = getline(&line, &len, file)) != EOF) {
		int i;
		bool found;
		char **values;

		i = 0;

		found = check_comma_in_quotes(line);

		values = malloc(multiple_columns_size);

		if (rows == -1) {
			make_split(&i, line, headers, found, s, &split_by_comma,
			    &split_line);
			rows++, i = 0;
			continue;
		}

		make_split(
		    &i, line, values, found, s, &split_by_comma, &split_line);

		r[rows]->headers = headers;
		r[rows]->values = values;
		r[rows]->cols = s->cols;

		rows++, i = 0;
	}
}

/* Helps determine how large the data is, row- and column-wise */
void
get_data_size(struct size *s, FILE *file)
{
	char buff[1000];
	int rows, cols;

	char **columns;
	bool found;

	// start with items;
	columns = malloc(MAX_COLUMNS * sizeof(char *));

	// get number of rows in file
	// start at -1 so it excludes headers
	rows = -1;
	cols = 0;

	while (fgets(buff, 1000, file) != '\0') {

		if (rows == -1) {
			found = check_comma_in_quotes(buff);

			split_line(buff, columns);
			rows++;
			// quickly get number of colums
			while (*(columns++) != NULL)
				cols++;
			continue;
		}
		rows++;
	}
	rewind(file);

	s->rows = rows;
	s->cols = cols;

	free_columns(cols, columns);
}
