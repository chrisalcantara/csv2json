#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <unistd.h>

#include "csv2json.h"

void
print_use()
{
	printf("Usage:\n");
	printf("\t$ csv2json data.csv > outfile.json\n");
	printf("\t$ cat data.csv | csv2json\n");
	printf("\t$ csv2json <data.csv\n");
	exit(EXIT_SUCCESS);
}

void
reset_counter(int *value)
{
	*value = 0;
}

void
reset_string(char *str)
{
	str[0] = '\0';
}

void
cutoff_string(char *str, int i)
{
	str[i - 1] = '\0';
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

/* Check if we are seeing a character
   that is between quotes.*/
void
check_if_in_quotes(char curr, char prev, int *in_quotes)
{
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
void
split_line(char *string, char *collection[])
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

			s = strlen(temp);

			(next == '\0') ? cutoff_string(temp, s + 1)
				       : cutoff_string(temp, s);

			output = malloc(s + 1 * sizeof(char *));
			strcpy(output, temp);

			collection[count] = output;

			reset_string(temp);

			string++;
			count++;
			continue;
		}
		string++;
	}
	free(temp);
}

void
free_columns(int cols, char **columns)
{
	/* Clean columns array */
	for (int i = 0; i < cols; ++i) {
		free(columns[i]);
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
			cutoff_string(buf, i);
			if (rows == -1) {
				split_line(buf, columns);

				/* quickly get number of columns */
				while (*columns != NULL) {
					cols++;
					columns++;
				}
			}
			rows++;
			reset_counter(&i);
		}
		file++;
	}

	s->rows = rows;
	s->cols = cols;

	free_columns(cols, columns);
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

			cutoff_string(buf, i);

			/* Handle getting sole header array */
			if (rows == 0) {
				split_line(buf, headers);
				reset_string(buf);
				reset_counter(&i);
				file++;
				rows++;
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

/* Utility to surround string with quotes.*/
void
add_quotes(char *str)
{
	size_t s;
	char buf[QUOTE_SIZE];

	if (str[0] != '"') {
		s = strlen(str) + 3;
		snprintf(buf, s, "\"%s\"", str);
		cutoff_string(buf, s);
		strcpy(str, buf);
	}
}

/* Takes the array of row structs and dumps it into a character array */
void
convert_to_json(char **final, struct size *s, struct row **r)
{

	char **objects;
	size_t total = 0;

	// object array
	// add two to include {open,close} brackets
	objects = malloc((s->rows + 2) * sizeof(char *));

	objects[0] = "[";
	for (int i = 0; i < s->rows; ++i) {
		char *obj, *with_comma, *no_comma;

		with_comma = "},";
		no_comma = "}";

		// 1000 for each row.x
		obj = malloc(OBJ_EACH_ROW_SIZE);

		strcat(obj, "{");
		for (int j = 0; j < s->cols; ++j) {

			char *key, *value, *temp;
			size_t key_pair_size, temp_size;

			char *format_with_comma, *format_no_comma;

			format_with_comma = "%s: %s, ";
			format_no_comma = "%s: %s";

			key = r[i]->headers[j];
			value = r[i]->values[j];

			add_quotes(key);
			add_quotes(value);

			key_pair_size = strlen(key) + strlen(value) + 10;

			/* Needed to hold the { key: value } */
			temp = malloc(key_pair_size);

			/* Determine which format string to use based
			   whether the item is within or at the end
			   of the array. */
			if (j == s->cols - 1)
				snprintf(temp, key_pair_size, format_no_comma,
				    key, value);
			else {
				snprintf(temp, key_pair_size, format_with_comma,
				    key, value);
			}

			temp_size = strlen(temp) + 1;
			cutoff_string(temp, temp_size);

			strcat(obj, temp);
			free(temp);
		}

		/* Needs the else or an extra command is included */
		/* where an end curly bracket should be. */
		if (i == s->rows - 1)
			strcat(obj, no_comma);
		else
			strcat(obj, with_comma);

		obj[strlen(obj)] = '\0';
		objects[i + 1] = obj;

		/* Get the total bytes needed to set */
		/* the final string pointer */
		total += strlen(obj);
	}
	objects[s->rows + 1] = "]";

	// Add one for the closing bracket
	*final = realloc(*final, total + 1);

	while (*objects != NULL) {
		strcat(*final, *objects);
		objects++;
	}
}

/* Free all rows. */
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
	fprintf(stdout, "%s", json);
}

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
