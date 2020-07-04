#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"

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
		free(rows[n_rows]);
	}
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

	char *copy = str;

	char *end;
	int index, i;
	index = 0;

	/* trim leading space. */
	while (copy[index] == ' ' || copy[index] == '\t' || copy[index] == '\n')
		index++;
	if (index != 0) {
		i = 0;
		while (copy[i + index] != '\0') {
			copy[i] = copy[i + index];
			i++;
		}
		copy[i] = '\0';
	}

	/* trim trailing space */
	end = copy + str_len(copy) - 1;
	while (end > copy && isspace((unsigned char)*end))
		end--;

	end[1] = '\0';

	memcpy(copy, str, str_len(str));
}

void
trim(char *token)
{

	bool in_quotes = false;
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
