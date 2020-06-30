
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct size {
	int rows;
	int cols;
};

struct row {
	char **headers;
	char **values;
	int cols;
};

enum { MAX_COLUMNS = 500 };

void
add_quotes(char *str)
{
	char buf[1000];
	if (str[0] != '"') {
		snprintf(buf, strlen(str) + 3, "\"%s\"", str);
		buf[strlen(buf)] = '\0';
		strcpy(str, buf);
	}
}

void
split_line(char *string, char *collection[])
{
	char curr, prev, next;
	char *temp = malloc(1000);

	int in_quotes = 0;
	int count = 0;

	while (*string != '\0') {

		curr = *string;
		prev = *(string - 1);
		next = *(string + 1);

		if (curr == '"' && in_quotes == 0) {
			in_quotes = 1;
		} else if (prev == '"' && curr == ',' && in_quotes == 1)
			in_quotes = 0;

		strncat(temp, &curr, 1);

		if (curr == ',' && in_quotes == 0) {

			size_t s = strlen(temp);
			temp[s - 1] = '\0';

			char *output = malloc(s + 1 * sizeof(char *));
			strcpy(output, temp);

			collection[count++] = output;

			temp[0] = '\0';
			string++;
			continue;
		}
		string++;
	}
	free(temp);
}

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

	printf("%d %d\n", rows, cols);

	s->rows = rows;
	s->cols = cols;

	// clean up
	for (int i = 0; i < cols; ++i) {
		free(columns[i]);
	}
}

struct row **
populate_rows(struct size *s)
{
	struct row **rows = malloc(s->rows * sizeof(struct row *));
	for (int i = 0; i < s->rows; ++i) {
		struct row *r = malloc(sizeof(struct row));
		rows[i] = r;
	}
	return rows;
}

void
make_rows(struct size *s, struct row **r, FILE *file)
{
	char *line;
	size_t len;
	ssize_t read;
	int lines_read;

	line = NULL;
	len = 0;
	lines_read = -1;

	char **headers = malloc(s->cols * sizeof(char *));

	while ((read = getline(&line, &len, file)) != EOF) {
		char **values = malloc(s->cols * sizeof(char *));
		if (lines_read == -1) {
			split_line(line, headers);
			lines_read++;
			continue;
		}
		split_line(line, values);

		r[lines_read]->headers = headers;
		r[lines_read]->values = values;
		r[lines_read]->cols = s->cols;

		lines_read++;
	}
}

void
convert_to_json(char **final, struct size *s, struct row **r)
{

	int total;
	char **objects;

	// object array
	// add two to include {open,close} brackets
	objects = malloc((s->rows + 2) * sizeof(char *));

	objects[0] = "[";
	for (int i = 0; i < s->rows; ++i) {
		char *obj;
		char *with_comma = "},";
		char *no_comma = "}";

		// 1000 for each row.x
		obj = malloc(1000);

		strcat(obj, "{");
		for (int j = 0; j < s->cols; ++j) {

			char *format_with_comma = "%s: %s,";
			char *format_no_comma = "%s: %s";

			char *key, *value, *temp;
			size_t key_pair_size;

			key = r[i]->headers[j];
			value = r[i]->values[j];

			add_quotes(key);
			add_quotes(value);

			key_pair_size = strlen(key) + strlen(value) + 10;

			temp = malloc(key_pair_size);

			if (j == s->cols - 1)
				snprintf(temp, key_pair_size, format_no_comma,
				    key, value);
			else {
				snprintf(temp, key_pair_size, format_with_comma,
				    key, value);
			}
			temp[strlen(temp)] = '\0';
			strcat(obj, temp);
			free(temp);
		}

		if (i == s->rows - 1) {
			strcat(obj, no_comma);
		} else {
			strcat(obj, with_comma);
		}

		obj[strlen(obj)] = '\0';
		objects[i + 1] = obj;
	}
	objects[s->rows + 1] = "]";

	total = 0;
	while (*objects != NULL) {
		int size;
		size = strlen(*objects);
		total += size;
		*final = realloc(*final, total);
		strcat(*final, *objects);
		objects++;
	}
}

void
free_structs(struct row **rows, int n_rows)
{
	for (int i = 0; i < n_rows; ++i) {
		free(rows[n_rows]);
	}
}

/* int */
/* main(void) */
int
main(int argc, char **argv)

{
	if (argc < 2) {
		perror("Error: Need an input file");
		exit(EXIT_FAILURE);
	}

	FILE *file;
	struct size s;
	struct row **rows;

	char *json;
	json = malloc(1 * sizeof(char *));

	file = fopen(argv[1], "r");
	/* file = fopen("../data.csv", "r"); */
	if (file == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	get_data_size(&s, file);
	rows = populate_rows(&s);
	make_rows(&s, rows, file);

	convert_to_json(&json, &s, rows);

	if (argc == 3)
		fprintf(stdout, "%s\n", json);

	// garbage collection
	free(json);
	free_structs(rows, s.rows);

	fclose(file);
	exit(EXIT_SUCCESS);

	return 0;
}
