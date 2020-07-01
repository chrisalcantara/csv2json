#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csv2json.h"

/* Utility to surround string with quotes.*/
static void
add_quotes(char *str)
{
	char buf[QUOTE_SIZE];
	if (str[0] != '"') {
		snprintf(buf, strlen(str) + 3, "\"%s\"", str);
		buf[strlen(buf)] = '\0';
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

		/* // 1000 for each row.x */
		obj = malloc(OBJ_EACH_ROW_SIZE);

		strcat(obj, "{");
		for (int j = 0; j < s->cols; ++j) {

			char *key, *value, *temp;
			size_t key_pair_size;

			char *format_with_comma, *format_no_comma;

			format_with_comma = "%s: %s,";
			format_no_comma = "%s: %s";

			key = r[i]->headers[j];
			value = r[i]->values[j];

			add_quotes(key);
			add_quotes(value);

			key_pair_size = strlen(key) + 1 + strlen(value) + 1;

			temp = malloc(1);

			// last item
			if (j == s->cols - 1) {
				key_pair_size = key_pair_size
				    + strlen(format_no_comma) + 1;

				temp = realloc(temp, key_pair_size);

				snprintf(temp, key_pair_size, format_no_comma,
				    key, value);
			} else {

				key_pair_size = key_pair_size
				    + strlen(format_with_comma) + 1;

				temp = realloc(temp, key_pair_size);

				snprintf(temp, key_pair_size, format_with_comma,
				    key, value);
			}

			temp[strlen(temp)] = '\0';
			strcat(obj, temp);
			free(temp);
		}

		/* Need or an extra command is included */
		/* where an end curly bracket should be. */
		(i == s->rows - 1) ? strcat(obj, no_comma)
				   : strcat(obj, with_comma);

		obj[strlen(obj)] = '\0';
		objects[i + 1] = obj;

		/* Get the total bytes needed to set */
		/* the final string pointer */
		total += strlen(obj);
	}
	objects[s->rows + 1] = "]";

	// Add one for the closing bracket
	*final = realloc(*final, (total + 1) * sizeof(char *));

	while (*objects != NULL) {
		strcat(*final, *objects);
		objects++;
	}
}
