#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"

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

void
check_if_in_quotes(char curr, char prev, bool *in_quotes)
{
	/* Check if we are seeing a character
	   that is between quotes.*/
	if (curr == '"' && *in_quotes == false) {
		*in_quotes = true;
	} else if (prev == '"' && curr == ',' && *in_quotes == true)
		*in_quotes = false;
}

int
str_len(char *word)
{
	int i = 0;
	while (*(word++) != '\0')
		i++;
	return i;
}

void
trim_whitespace(char *str)
{
	char *end;
	int index, i;
	index = 0;

	/* trim leading space. */
	while (str[index] == ' ' || str[index] == '\t' || str[index] == '\n')
		index++;
	if (index != 0) {
		i = 0;
		while (str[i + index] != '\0') {
			str[i] = str[i + index];
			i++;
		}
		str[i] = '\0';
	}

	/* trim trailing space */
	end = str + str_len(str) - 1;
	while (end > str && isspace((unsigned char)*end))
		end--;

	end[1] = '\0';
	memcpy(str, str, str_len(str));
}

void
trim(char *token)
{
	bool in_quotes;
	char *dup;
	size_t character_count, token_length, length_diff;

	// To reset pointer
	dup = token;
	character_count = 0;

	while (*token != '\0') {
		char curr = *token;
		char prev = *(token - 1);

		check_if_in_quotes(curr, prev, &in_quotes);

		if (in_quotes) {
			token++, character_count++;
		} else {
			if ((isspace(*token) || *token == ','))
				break;
			token++, character_count++;
		}
	}
	token = dup;

	token_length = strlen(token);
	length_diff = token_length - character_count;

	if (token_length > character_count)
		token[token_length - length_diff] = '\0';
}
