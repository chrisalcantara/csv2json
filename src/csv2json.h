#ifndef CSV2JSON_H
#define CSV2JSON_H

struct size {
	int rows;
	int cols;
};

struct row {
	char **headers;
	char **values;
	int cols;
};

enum { MAX_COLUMNS = 500,
	SPLIT_LINE_TEMP_SIZE = 1000,
	QUOTE_SIZE = 1000,
	OBJ_EACH_ROW_SIZE = 1000 };

#endif // CSV2JSON_H
