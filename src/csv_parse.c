#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"

#include "utils.h"

static void
trim(char *token)
{
	char *dup;
	size_t character_count, token_length, length_diff;

	// To reset pointer
	dup = token;
	character_count = 0;

	while (*token != '\0') {
		if (isspace(*token) || *token == ',')
			break;
		token++, character_count++;
	}
	token = dup;

	token_length = strlen(token);
	length_diff = token_length - character_count;

	if (token_length > character_count)
		token[token_length - length_diff] = '\0';
}

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

/* Map structs for row headers and values to an array to
   be converted to JSON. */
void
make_rows(struct size *s, struct row **r, FILE *file)
{
	char *line, **headers;
	size_t len;
	ssize_t read;
	int rows;

	line = NULL;
	len = 0;
	rows = -1;

	headers = malloc(s->cols * sizeof(char *));

	/* Same split method as in split_lines, but also handles
	   headers and values, as well as maps to struct  */
	while ((read = getline(&line, &len, file)) != EOF) {
		char **values = malloc(s->cols * sizeof(char *));
		if (rows == -1) {
			int i = 0;
			size_t token_length;
			char *token, *text, *output;

			text = strdup(line);

			while ((token = strsep(&text, ",")) != NULL) {
				trim(token);
				token_length = strlen(token);
				output = malloc(token_length * sizeof(char *));
				strncpy(output, token, token_length);
				headers[i++] = output;
			}

			if (s->cols != i) {

				fprintf(stderr, "%s",
				    "Error: No. of columns don't match with "
				    "the number of headers calculated. This "
				    "program splits by comma. Is there a stray "
				    "comma in the file's header row?\n");

				exit(0);
			}

			rows++;
			continue;
		}
		split_line(line, values);

		r[rows]->headers = headers;
		r[rows]->values = values;
		r[rows]->cols = s->cols;

		rows++;
	}
}

/* Helps determine how large the data is, row- and column-wise */
void
get_data_size(struct size *s, FILE *file)
{

	char buff[1000];
	int rows, cols;

	char **columns;

	// start with items;
	columns = malloc(MAX_COLUMNS * sizeof(char *));

	// get number of rows in file
	// start at -1 so it excludes headers
	rows = -1;
	cols = 0;
	while (fgets(buff, 1000, file) != '\0') {
		if (rows == -1) {
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
