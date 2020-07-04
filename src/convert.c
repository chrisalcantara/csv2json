#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"

#include "csv_parse.h"
#include "utils.h"

/* Utility to surround string with quotes.*/
static void
add_quotes(char *str)
{
	char buf[QUOTE_SIZE];
	char *has_comma;
	char *has_new_line;

	if (str[0] != '"') {
		if ((has_comma = strstr(str, ",")) != NULL)
			has_comma[0] = '\0';
		if ((has_new_line = strstr(str, "\n")) != NULL) {
			has_new_line[0] = '\0';
		}
		snprintf(buf, strlen(str) + 3, "\"%s\"", str);
		strcpy(str, buf);
	}

	/* Remove extra space at the end */
	if ((has_new_line = strstr(str, "\n")) != NULL) {
		has_new_line[0] = '\0';
	}
}

/* Takes the array of row structs and dumps it into a character array */
void
convert_to_json(char ***objects_ptr, struct size *s, struct row **r)
{

	char *open, *close;
	char **objects;
	size_t objects_size;

	// object array
	// add two to include {open,close} brackets

	objects_size = (s->rows + 2) * sizeof(char *);

	objects = *objects_ptr;
	objects = malloc(objects_size);

	open = "[";
	close = "]";

	objects[0] = open;

	for (int i = 0; i < s->rows; ++i) {

		char *obj, *with_comma, *no_comma;

		with_comma = "},";
		no_comma = "}";

		/* // 1000 for each row.x */
		obj = malloc(OBJ_EACH_ROW_SIZE * sizeof(char *));

		strcat(obj, "{");
		for (int j = 0; j < s->cols; ++j) {

			char *key, *value;

			char *format_with_comma, *format_no_comma,
			    *format_to_use;

			char temp[1000];

			format_with_comma = "%s: %s,";
			format_no_comma = "%s: %s";

			key = r[i]->headers[j];
			value = r[i]->values[j];

			// last chance to remove
			// lingering white space
			// before wrapping in quotes
			trim_whitespace(key);
			trim_whitespace(value);

			add_quotes(key);
			add_quotes(value);

			format_to_use = (j == s->cols - 1) ? format_no_comma
							   : format_with_comma;

			snprintf(temp, 1000, format_to_use, key, value);
			temp[strlen(temp)] = '\0';

			strcat(obj, temp);
		}

		/* Need or an extra command is included */
		/* where an end curly bracket should be. */
		(i == s->rows - 1) ? strcat(obj, no_comma)
				   : strcat(obj, with_comma);

		obj[strlen(obj)] = '\0';
		objects[i + 1] = obj;
	}

	objects[s->rows + 1] = close;

	*objects_ptr = objects;
}
